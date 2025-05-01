#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> // Defines INET_ADDRSTRLEN and inet_ntop

#pragma comment(lib, "ws2_32.lib") // Link with Winsock library

#define HTTP_BUFFER_SIZE 4096 // Buffer to accommodate headers + content

typedef struct SocketConnectionInfo {
  SOCKET server_socket;
  SOCKET client_socket;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int client_addr_len;
} SocketConnectionInfo;

typedef struct HttpResponse {
  SOCKET client_socket;
  int status_code;
  const char *content_type;
  const char *body;
} HttpResponse;

typedef struct HttpRequest {
  char method[16];   // Stores the HTTP method (e.g., "GET", "POST")
  char path[256];    // Stores the requested path (e.g., "/", "/save_todos")
  char protocol[16]; // Stores the HTTP protocol version (e.g., "HTTP/1.1")

  // Headers parsed from the request
  long content_length; // Value of the Content-Length header (-1 if not present
  // or invalid)
  char content_type[128]; // Value of the Content-Type header (empty string if
  // not present)

  // Information about the request body within the receive buffer
  const char
      *body_start; // Pointer to the beginning of the body data in the buffer
  // (NULL if no body separator "\r\n\r\n" was found or no body expected)
  int body_length; // The number of body bytes *currently present* in the
  // receive buffer
  // starting from body_start. This might be less than content_length
  // if the full body wasn't received in the first `recv` call.
} HttpRequest;

typedef struct Response Response;
typedef struct Request Request;

typedef void (*RouteHandler)(Response res, Request request);

typedef struct Route {
  const char *method;    // Method (e.g., "GET", "POST")
  const char *route_key; // Path (e.g., "/", "/about")
  RouteHandler handler;
} Route;

typedef struct Routes {
  Route routes[10];
  int route_count;
} Routes;

// Validation criteria for an HTTP request
typedef struct ValidationRules {
  const char
      *required_method; // e.g., "GET", "POST". NULL = Don't check method.
  bool require_body;  // true = Content-Length must be > 0 and body must exist.
  bool disallow_body; // true = Request should not have a body (CL=0 or absent).
  // Note: require_body=false, disallow_body=false means body is optional.
  const char
      *required_content_type; // e.g., "application/json". NULL = Don't check.
  // Only checked if a body is present or required.
  // --- Potential future extensions ---
  // long max_content_length;             // -1 = no limit
  // const char** required_headers;       // NULL-terminated list of required
  // header names bool require_query_param;
} ValidationRules;

void Responsesend(struct Response *this, char *body, int code);

bool HTTPServerInternal_ValidateRequest(struct Response *res,
                                        const HttpRequest *request,
                                        const ValidationRules *rules) {
  // Performs generic validation checks on an HTTP request based on rules.
  // Sends an appropriate HTTP error response (400, 405, 411, 415) if validation
  // fails. Returns true if the request passes all specified validations.

  if (!request || !rules) {
    fprintf(stderr, "ValidateRequest Error: Invalid arguments (request or "
                    "rules is NULL)\n");
    // This is an internal server error, shouldn't happen in normal operation
    Responsesend(res, "{\"error\":\"Internal Server Configuration Error\"}",
                 500);
    return false;
  }

  // 1. Check Method
  if (rules->required_method &&
      strcmp(request->method, rules->required_method) != 0) {
    fprintf(stderr,
            "Validation Error: Invalid method %s for %s (expected %s)\n",
            request->method, request->path, rules->required_method);
    char error_body[100];
    snprintf(error_body, sizeof(error_body),
             "{\"error\":\"Method %s not allowed for this endpoint\"}",
             request->method);
    Responsesend(res, error_body, 405); // Method Not Allowed
    return false;
  }

  // 2. Check Body Presence/Absence
  bool has_body = (request->content_length > 0 ||
                   request->body_start != NULL); // Heuristic for body presence

  if (rules->disallow_body && has_body) {
    // Check if Content-Length header explicitly indicated a body
    if (request->content_length > 0) {
      fprintf(stderr,
              "Validation Error: Request body not allowed for %s %s "
              "(Content-Length: %ld)\n",
              request->method, request->path, request->content_length);
      Responsesend(
          res, "{\"error\":\"Request body not permitted for this endpoint\"}",
          400); // Bad Request
      return false;
    }
    // Check if body data was present after headers even without Content-Length
    // (less common for disallowed routes)
    if (request->body_start != NULL && request->body_length > 0) {
      fprintf(
          stderr,
          "Validation Error: Unexpected request body data found for %s %s\n",
          request->method, request->path);
      Responsesend(res, "{\"error\":\"Unexpected request body data\"}",
                   400); // Bad Request
      return false;
    }
    // If CL <= 0 and body_start is NULL, it's fine even if disallow_body=true
  }

  if (rules->require_body) {
    if (request->content_length <= 0) {
      fprintf(stderr,
              "Validation Error: Request to %s %s requires Content-Length > 0 "
              "(Got: %ld)\n",
              request->method, request->path, request->content_length);
      Responsesend(
          res, "{\"error\":\"Content-Length required and must be positive\"}",
          411); // Length Required
      return false;
    }
    // Also check if the parser actually found the body start (important!)
    if (!request->body_start) {
      fprintf(stderr,
              "Validation Error: Request to %s %s requires body, but body "
              "start not found in buffer.\n",
              request->method, request->path);
      // This might indicate an incomplete request received by the server *or* a
      // parsing bug
      Responsesend(
          res,
          "{\"error\":\"Request body required but not found or incomplete\"}",
          400); // Bad Request
      return false;
    }
    // Note: We rely on handle_client having checked if body_length >=
    // content_length for completeness
  }

  // 3. Check Content-Type (Only if a body is expected/present and a type is
  // required)
  bool check_content_type =
      rules->required_content_type != NULL &&
      (rules->require_body || has_body); // Check type only if body relevant

  if (check_content_type) {
    // Compare Content-Type case-insensitively, ignoring potential parameters
    // (like ; charset=utf-8) Find the length of the base type (e.g.,
    // "application/json")
    size_t required_type_len = strlen(rules->required_content_type);
    if (strncasecmp(request->content_type, rules->required_content_type,
                    required_type_len) != 0) {
      // Maybe it has parameters? Check if the next char is whitespace or ';' or
      // end-of-string
      const char *type_end = request->content_type + required_type_len;
      if (!(*type_end == '\0' || isspace((unsigned char)*type_end) ||
            *type_end == ';')) {
        // It's genuinely a different type or a prefix mismatch
        fprintf(stderr,
                "Validation Error: Request to %s %s has wrong Content-Type: "
                "'%s' (expected '%s')\n",
                request->method, request->path, request->content_type,
                rules->required_content_type);
        char error_body[150];
        snprintf(error_body, sizeof(error_body),
                 "{\"error\":\"Unsupported Content-Type. Expected '%s'\"}",
                 rules->required_content_type);
        Responsesend(res, error_body, 415); // Unsupported Media Type
        return false;
      }
      // If the prefix matches and is followed by space/;/EOS, we accept it for
      // basic checks
      printf("DEBUG: Content-Type '%s' accepted as matching base type '%s'\n",
             request->content_type, rules->required_content_type);
    }
  }

  // If all applicable checks passed
  return true;
}

bool HTTPServerInternal_is_socket_valid(SOCKET socket) {
  return socket != INVALID_SOCKET;
}

// Function to send response back to the client
void HTTPServerInternal_SendResponse(HttpResponse *res, const char *body,
                                     int status_code) {
  char response[HTTP_BUFFER_SIZE]; // Make sure buffer is large enough
  const char *status_text;

  // Determine the status text based on status_code
  switch (status_code) {
  case 200:
    status_text = "200 OK";
    break;
  case 201:
    status_text = "201 Created";
    break; // Added for POST success
  case 400:
    status_text = "400 Bad Request";
    break; // Added for client errors
  case 404:
    status_text = "404 Not Found";
    break;
  case 405:
    status_text = "405 Method Not Allowed";
    break; // If method mismatches route
  case 411:
    status_text = "411 Length Required";
    break;
  case 415:
    status_text = "415 Unsupported Media Type";
    break; // For wrong Content-Type
  case 500:
    status_text = "500 Internal Server Error";
    break;
  default:
    status_text = "500 Internal Server Error";
    break;
  }

  // Determine the content type automatically (basic heuristic)
  const char *content_type = "text/html"; // Default
  if (body != NULL) {
    // Basic check: If body looks like JSON, set content type accordingly
    size_t body_actual_len = strlen(body);
    if (body_actual_len > 0 && (body[0] == '{' || body[0] == '[')) {
      // More robust check might involve trying to parse, but this is common
      content_type = "application/json";
    } else if (strstr(body, "<html>") != NULL) {
      content_type = "text/html";
    } else {
      content_type = "text/plain";
    }
  } else {
    // No body, content type doesn't matter as much, but set length to 0
    body = ""; // Ensure body is not NULL for strlen
  }

  int body_len = (int)strlen(body);

  // Prepare the complete response (headers + content)
  int response_length = snprintf(
      response, sizeof(response),
      "HTTP/1.1 %s\r\n"
      "Content-Type: %s\r\n"
      "Content-Length: %d\r\n"                         // Important!
      "Access-Control-Allow-Origin: *\r\n"             // Keep for AJAX
      "Access-Control-Allow-Headers: Content-Type\r\n" // Allow Content-Type
                                                       // header
      "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n" // Allow POST
      "Connection: close\r\n"
      "\r\n"
      "%s",
      status_text, content_type, body_len, body);

  if (response_length >= sizeof(response)) {
    fprintf(stderr, "ERROR: Response buffer too small!\n");
    // Handle error: maybe send a 500 error with minimal headers
    const char *error_resp =
        "HTTP/1.1 500 Internal Server Error\r\nContent-Length: "
        "0\r\nConnection: close\r\n\r\n";
    send(res->client_socket, error_resp, (int)strlen(error_resp), 0);
  } else {
    // Send the complete response
    send(res->client_socket, response, response_length, 0);
  }

  printf("Response Sent: %s (Content-Type: %s, Length: %d)\n", status_text,
         content_type, body_len);
}

// Parses the request line and basic headers (Content-Length, Content-Type)
// Returns true on success, false on parse error.
// Sets request->body_start to point after \r\n\r\n if found.
bool HTTPServerInternal_ParseHttpRequest(const char *buffer, int buffer_len,
                                         HttpRequest *request) {
  if (!buffer || buffer_len <= 0)
    return false;

  // Initialize request struct
  memset(request, 0, sizeof(HttpRequest));
  request->content_length = -1; // Indicate not found yet
  request->body_start = NULL;
  request->body_length = 0;

  const char *end_of_buffer = buffer + buffer_len;
  const char *current_pos = buffer;

  // 1. Parse Request Line (e.g., "POST /api/todos HTTP/1.1\r\n")
  const char *line_end = strstr(current_pos, "\r\n");
  if (!line_end)
    return false; // Malformed request

  int line_len = line_end - current_pos;
  if (sscanf(current_pos, "%15s %255s %15s", request->method, request->path,
             request->protocol) != 3) {
    fprintf(stderr, "Failed to parse request line\n");
    return false; // Malformed request line
  }
  current_pos = line_end + 2; // Move past \r\n

  // 2. Parse Headers (simple example)
  while (current_pos < end_of_buffer) {
    line_end = strstr(current_pos, "\r\n");
    if (!line_end)
      break; // End of headers not found cleanly?

    int header_line_len = line_end - current_pos;
    if (header_line_len == 0) {           // Empty line indicates end of headers
      request->body_start = line_end + 2; // Body starts after \r\n\r\n
      break;
    }

    // Check for Content-Length
    if (strncasecmp(current_pos, "Content-Length:", 15) == 0) {
      const char *value_start = current_pos + 15;
      while (*value_start == ' ' && value_start < line_end)
        value_start++; // Skip spaces
      request->content_length = atol(value_start);
    }
    // Check for Content-Type
    else if (strncasecmp(current_pos, "Content-Type:", 13) == 0) {
      const char *value_start = current_pos + 13;
      while (*value_start == ' ' && value_start < line_end)
        value_start++; // Skip spaces
      strncpy(request->content_type, value_start,
              sizeof(request->content_type) - 1);
      request->content_type[sizeof(request->content_type) - 1] =
          '\0'; // Null terminate
      // Trim trailing whitespace/CR if any
      char *p = request->content_type + strlen(request->content_type) - 1;
      while (p >= request->content_type && isspace((unsigned char)*p)) {
        *p-- = '\0';
      }
    }

    current_pos = line_end + 2; // Move to next header line
  }

  // 3. Calculate body length *present in the current buffer*
  if (request->body_start && request->body_start < end_of_buffer) {
    request->body_length = end_of_buffer - request->body_start;
  } else {
    request->body_length = 0;
    request->body_start = NULL; // Ensure it's NULL if no body found in buffer
  }

  // Basic validation
  if (strlen(request->method) == 0 || strlen(request->path) == 0) {
    return false;
  }

  printf("Parsed Request: %s %s %s\n", request->method, request->path,
         request->protocol);
  if (request->content_length >= 0)
    printf("  Content-Length: %ld\n", request->content_length);
  if (strlen(request->content_type) > 0)
    printf("  Content-Type: %s\n", request->content_type);
  if (request->body_start)
    printf("  Body starts at offset: %ld, Length in buffer: %d\n",
           request->body_start - buffer, request->body_length);

  return true;
}

///*///

///*/////gcc -o .\WebServer .\WebServer_generated.c -lws2_32
#include <stdio.h>

// IMPORTS //

struct Response {
  HttpResponse res;
};

struct Request {
  HttpRequest request;
};

struct HTTPServer {
  SocketConnectionInfo server;
  Routes routes;
};

void Responsesend(struct Response *this, char *body, int code);

int RequestGetContentLength(struct Request *this);
char *RequestGetBodyStart(struct Request *this);
bool RequestValidateIsPostAndJSON(struct Request *this, struct Response res);

void HTTPServer__init__(struct HTTPServer *this);
void HTTPServerregister_route(struct HTTPServer *this, char *method,
                              char *route_key, RouteHandler handler);
int HTTPServerstart_server(struct HTTPServer *this, int port);
void HTTPServerhandle_client(struct HTTPServer *this);
void HTTPServeraccept_connections(struct HTTPServer *this);
void HTTPServerlisten(struct HTTPServer *this, int port);
void HTTPServer__del__(struct HTTPServer *this);
void Handle404(struct Response res, struct Request req);
void HandleAbout(struct Response res, struct Request req);
void HandleHome(struct Response res, struct Request req);

void Responsesend(struct Response *this, char *body, int code) {
  HTTPServerInternal_SendResponse(&this->res, body, code);
}

int RequestGetContentLength(struct Request *this) {
  return this->request.content_length;
}

char *RequestGetBodyStart(struct Request *this) {
  if (this->request.body_start) {
    return this->request.body_start;
  } else {
    return NULL; // No body found
  }
}

bool RequestValidateIsPostAndJSON(struct Request *this, struct Response res) {
  // Requests validation.
  // Harcode validation rules.
  ValidationRules rules = {.required_method = "POST",
                           .require_body = true,
                           .disallow_body = false,
                           .required_content_type = "application/json"};

  return HTTPServerInternal_ValidateRequest(&res, &this->request, &rules);
}

void HTTPServer__init__(struct HTTPServer *this) {
  this->server.client_addr_len = sizeof(this->server.client_addr);
  this->routes.route_count = 0;
}

void HTTPServerregister_route(struct HTTPServer *this, char *method,
                              char *route_key, RouteHandler handler) {
  if (this->routes.route_count >=
      sizeof(this->routes.routes) / sizeof(this->routes.routes[0])) {
    fprintf(stderr, "Error: Maximum number of routes reached.\n");
    return;
  }

  // Should ideally duplicate method and route_key strings if they aren't static
  // literals
  Route route = {method, route_key, handler};
  this->routes.routes[this->routes.route_count++] = route;
  printf("Registered route: %s %s\n", method, route_key);
}

int HTTPServerstart_server(struct HTTPServer *this, int port) {
  WSADATA wsa_data;

  // Initialize Winsock
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
    printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
    return 1;
  }

  // Create socket
  this->server.server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->server.server_socket == INVALID_SOCKET) {
    printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  // Prepare the sockaddr_in structure
  this->server.server_addr.sin_family = AF_INET;
  this->server.server_addr.sin_addr.s_addr = INADDR_ANY;
  this->server.server_addr.sin_port = htons(port);

  // Bind the socket
  if (bind(this->server.server_socket,
           (struct sockaddr *)&this->server.server_addr,
           sizeof(this->server.server_addr)) == SOCKET_ERROR) {
    printf("Bind failed. Error Code: %d\n", WSAGetLastError());
    closesocket(this->server.server_socket);
    WSACleanup();
    return 1;
  }

  // Listen for incoming connections
  listen(this->server.server_socket, 5);

  printf("Server listening on port %d...\n", port);
  return 0;
}

void HTTPServerhandle_client(struct HTTPServer *this) {
  char buffer[HTTP_BUFFER_SIZE];
  int recv_size;
  SOCKET client_sock = this->server.client_socket;

  printf("DEBUG: Handling client on Socket: %d\n", (int)client_sock);

  DWORD timeout_ms = 15000;
  setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout_ms,
             sizeof(timeout_ms)); // Error check omitted for brevity

  recv_size = recv(client_sock, buffer, sizeof(buffer) - 1, 0);

  if (recv_size <= 0) {
    // Handle recv errors or client disconnect (closes socket)
    if (recv_size == SOCKET_ERROR) {
      int error_code = WSAGetLastError();
      if (error_code == WSAETIMEDOUT) {
        printf("DEBUG: Recv timed out on Socket: %d\n", (int)client_sock);
      } else {
        printf("Recv failed on Socket %d. Error Code: %d\n", (int)client_sock,
               error_code);
      }
    } else { // recv_size == 0
      printf("DEBUG: Client (Socket: %d) disconnected gracefully.\n",
             (int)client_sock);
    }
    closesocket(client_sock);
    printf("DEBUG: Closed Socket %d due to recv error/timeout/disconnect.\n",
           (int)client_sock);
    return;
  }

  buffer[recv_size] = '\0';

  HttpRequest request;
  if (!HTTPServerInternal_ParseHttpRequest(buffer, recv_size, &request)) {
    fprintf(stderr, "Failed to parse request on Socket %d.\n",
            (int)client_sock);
    HttpResponse resp_info = {client_sock};
    HTTPServerInternal_SendResponse(
        &resp_info, "<html><body><h1>400 Bad Request</h1></body></html>", 400);
    closesocket(client_sock);
    printf("DEBUG: Closed Socket %d due to parse error.\n", (int)client_sock);
    return;
  }

  // --- Basic Body Validation (Still useful here) ---
  // Check if the received body length seems plausible based on Content-Length
  // We still don't handle fragmented bodies perfectly here.
  if (request.content_length > 0) {
    if (!request.body_start) {
      fprintf(stderr,
              "WARN: Content-Length %ld specified, but no body found after "
              "headers on Socket %d.\n",
              request.content_length, (int)client_sock);
      HttpResponse resp_info = {client_sock};
      HTTPServerInternal_SendResponse(
          &resp_info,
          "<html><body><h1>400 Bad Request (Missing Body)</h1></body></html>",
          400);
      closesocket(client_sock);
      printf("DEBUG: Closed Socket %d due to missing body.\n",
             (int)client_sock);
      return;
    }
    if (request.body_length < request.content_length) {
      fprintf(stderr,
              "WARN: Content-Length %ld specified, but only received %d body "
              "bytes in first recv on Socket %d. (Potential truncated body)\n",
              request.content_length, request.body_length, (int)client_sock);
      // Decide if this is fatal. For simplicity, let's make it fatal.
      // A robust server might try receiving more data.
      HttpResponse resp_info = {client_sock};
      HTTPServerInternal_SendResponse(
          &resp_info,
          "<html><body><h1>400 Bad Request (Incomplete Body "
          "Received)</h1></body></html>",
          400);
      closesocket(client_sock);
      printf("DEBUG: Closed Socket %d due to incomplete body.\n",
             (int)client_sock);
      return;
    }
    // Note: request.body_length might be > request.content_length if pipelining
    // occurs. The handler should rely on request.content_length for parsing the
    // body.
  }
  // --- End Basic Body Validation ---

  // --- Routing ---
  HttpResponse res_info = {client_sock};
  Response resp = {res_info};
  bool route_found = false;

  for (int i = 0; i < this->routes.route_count; i++) {
    if (strcmp(request.method, this->routes.routes[i].method) == 0 &&
        strcmp(request.path, this->routes.routes[i].route_key) == 0) {
      printf("DEBUG: Matched route %s %s for Socket %d.\n", request.method,
             request.path, (int)client_sock);

      // --- Handle OPTIONS request generically for CORS preflight ---
      // This check should generally happen *before* trying to dispatch to a
      // specific handler unless a route *needs* custom OPTIONS logic.
      if (strcmp(request.method, "OPTIONS") == 0) {
        printf("DEBUG: Handling OPTIONS preflight request generically.\n");
        // Send simple 200 OK with CORS headers (already added in SendResponse)
        HTTPServerInternal_SendResponse(&res_info, NULL, 200); // No body needed
        route_found = true; // Mark as handled
        break;              // Stop processing routes
      }

      struct Request req;
      req.request = request;
      this->routes.routes[i].handler(resp, req);

      route_found = true;
      break;
    }
  }

  // --- Handle 404 Not Found ---
  if (!route_found) {
    printf("DEBUG: No route matched for %s %s on Socket %d. Sending 404.\n",
           request.method, request.path, (int)client_sock);
    struct Request req;
    req.request = request;
    Handle404(resp, req);
  }

  // --- Shutdown and Close ---
  printf("DEBUG: Request handling complete for Socket %d. Shutting down send "
         "side.\n",
         (int)client_sock);
  if (shutdown(client_sock, SD_SEND) == SOCKET_ERROR) {
    if (WSAGetLastError() != WSAENOTCONN) {
      printf("WARN: shutdown(SD_SEND) failed for Socket %d. Error: %d\n",
             (int)client_sock, WSAGetLastError());
    }
  }

  printf("DEBUG: Closing client socket %d (Normal completion).\n",
         (int)client_sock);
  closesocket(client_sock);
}

void HTTPServeraccept_connections(struct HTTPServer *this) {
  printf("DEBUG: Entering accept_connections loop...\n");
  while (1) {
    printf("DEBUG: Waiting for connection (accept)...\n");
    this->server.client_addr_len =
        sizeof(this->server.client_addr); // Reset size
    this->server.client_socket =
        accept(this->server.server_socket,
               (struct sockaddr *)&this->server.client_addr,
               &this->server.client_addr_len);

    if (!HTTPServerInternal_is_socket_valid(this->server.client_socket)) {
      int error = WSAGetLastError();
      // Handle specific errors if needed (e.g., WSAEINTR)
      if (error == WSAEINTR)
        continue; // Interrupted, try again
      printf("Accept failed. Error Code: %d. Continuing loop.\n", error);
      Sleep(100); // Avoid busy-spinning on persistent errors
      continue;
    }

    // Get client IP (optional)
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &this->server.client_addr.sin_addr, client_ip,
              INET_ADDRSTRLEN);
    printf("DEBUG: Client connected from %s! Socket: %d\n", client_ip,
           (int)this->server.client_socket);

    HTTPServerhandle_client(this); // Handle the client fully

    printf("DEBUG: Finished handling client from %s. Socket: %d\n", client_ip,
           (int)this->server.client_socket);
    // Socket is closed inside handle_client
  }
  printf("DEBUG: Exiting accept_connections loop (should not happen with "
         "while(1)).\n");
}

void HTTPServerlisten(struct HTTPServer *this, int port) {

  if (HTTPServerstart_server(this, port) == 0) {
    HTTPServeraccept_connections(this);
  }
}

void HTTPServer__del__(struct HTTPServer *this) {
  closesocket(this->server.server_socket);
  WSACleanup();
}

///*///

void Handle404(struct Response res, struct Request req) {
  char html[58] = "<html><body><h1>404 Page Not Found</h1></body></html>\r\n";
  Responsesend(&res, html, 404);
}

void HandleAbout(struct Response res, struct Request req) {
  char html[53] = "<html><body><h1>About Us Page</h1></body></html>\r\n";
  Responsesend(&res, html, 200);
}

void HandleHome(struct Response res, struct Request req) {
  char html[61] =
      "<html><body><h1>Welcome to Home Page!</h1></body></html>\r\n";
  Responsesend(&res, html, 200);
}
///*///

int main() {

  ///*///  main()

  struct HTTPServer server;
  HTTPServer__init__(&server);

  // server.register_route("GET" "/about", HandleAbout)
  // server.register_route("GET" "/home", HandleHome)
  // server.register_route("GET", "/ ", HandleHome)

  // Use reflection to get all functions with annotations '@route' and register
  // those functions as routes. This generates the code same as above.
  HTTPServerregister_route(&server, "GET", "/about", HandleAbout);
  HTTPServerregister_route(&server, "GET", "/home", HandleHome);
  HTTPServerregister_route(&server, "GET", "/", HandleHome);

  HTTPServerlisten(&server, 8080);

  HTTPServer__del__(&server);
  ///*///

  return 0;
}