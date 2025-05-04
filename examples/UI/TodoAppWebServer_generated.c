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

///*//////*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

///*///

///*///

///*///

#include "cJSON.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

///*///

///*///
// Run this file and then open Todos.html in a browser to see the UI.

// gcc -o .\TodoAppWebServer .\TodoAppWebServer_generated.c cJSON.c -lws2_32

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

struct String {
  char *arr;
  int length;
  int capacity;
  bool is_constexpr;
};

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

struct File {
  FILE *file_ptr;
};

struct JSON {
  struct Vector_String parsed_strings;
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

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void Stringinit__STATIC__(struct String *this, char *text, int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length);
void String__init__OVDstructString(struct String *this, struct String text);
void Stringclear(struct String *this);
void Stringprint(struct String *this);
void StringprintLn(struct String *this);
void String__del__(struct String *this);
bool Stringstartswith(struct String *this, char *prefix);
struct String Stringsubstr(struct String *this, int start, int length);
struct String Stringstrip(struct String *this);
struct Vector_String Stringsplit(struct String *this, char delimeter);
bool String__contains__(struct String *this, char *substring);
bool String__eq__(struct String *this, char *pstring);
void String__add__(struct String *this, char *pstring);
void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length);
void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring);
void String__reassign__OVDstr(struct String *this, char *pstring);
void Stringset_to_file_contents(struct String *this, char *pfilename);
struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename);

void File__init__(struct File *this, char *p_file_name);
void Filewriteline(struct File *this, char *p_content);
void File__del__(struct File *this);
void JSON__init__(struct JSON *this);
bool JSONParse(struct JSON *this, char *json_string, int len);

struct Vector_String LoadTodosFromFile();
void WriteTodosToFile(struct Vector_String todos);
struct String VectorStringToJSONString(struct Vector_String todos);
void Handle404(struct Response res, struct Request req);
void HandleGetTodos(struct Response res, struct Request req);
void HandleSaveTodos(struct Response res, struct Request request);
size_t Vector_Stringlen(struct Vector_String *this);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index);
void Vector_String_call_destructor_for_all_elements(struct Vector_String *this);
void Vector_String_reset(struct Vector_String *this);
void Vector_String__del__(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
void Vector_Stringvalidate_index(struct Vector_String *this, int index);
struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s);
void Vector_String_set(struct Vector_String *this, int index,
                       struct String value);
void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_String_grow_if_required(struct Vector_String *this);
void Vector_String_push(struct Vector_String *this, struct String value);
void Vector_Stringpush(struct Vector_String *this, struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_String_shift_left_from(struct Vector_String *this, int index);
void Vector_Stringremove_at(struct Vector_String *this, int index);
void Vector_String_clear(struct Vector_String *this);
void Vector_Stringclear(struct Vector_String *this);
bool Vector_String__contains__(struct Vector_String *this, struct String value);
void Vector_Stringprint(struct Vector_String *this);

void JSON__del__(struct JSON *this) {
  Vector_String__del__(&this->parsed_strings);
}

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

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

size_t Stringlength_of_charptr(struct String *this, char *p_string) {
  // This should be some kind of static method.
  return strlen(p_string);
}

void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length) {
  // p_text_length : Length of the string without the null terminator.
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strncpy(this->arr, text, p_text_length);
  this->arr[p_text_length] = '\0';

  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = false;
}

void Stringinit__STATIC__(struct String *this, char *text, int p_text_length) {
  // WARNING: Only the compiler should write a call to this function.
  // The compiler uses this initialization function to create a temporary String
  // object when a string literal is passed to a function that expects a String
  // object.
  this->arr = text;
  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = true;
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length) {
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstructString(struct String *this, struct String text) {
  size_t p_text_length = Stringlen(&text);
  String__init__from_charptr(this, Stringc_str(&text), p_text_length);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) {
  if (!this->is_constexpr) {
    free(this->arr);
  }
}

bool Stringstartswith(struct String *this, char *prefix) {
  return strncmp(this->arr, prefix, strlen(prefix)) == 0;
}

struct String Stringsubstr(struct String *this, int start, int length) {
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;
}

struct String Stringstrip(struct String *this) {
  char *begin = this->arr;
  char *end = begin + Stringlen(this) - 1;

  // Remove leading whitespaces.
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces.
  while (end > begin && isspace(*end)) {
    end--;
  }

  // Length of the substring between 'begin' and 'end' inclusive.
  int new_length = end - begin + 1;

  struct String text;
  String__init__from_charptr(&text, begin, new_length);
  return text;
}

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // NOTE : Because of this function, before import String, we require import
  // Vector.
  struct Vector_String split_result;
  Vector_String__init__(&split_result, 2);

  int index = 0;
  int segment_start = 0;

  size_t tmp_len_0 = Stringlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char character = String__getitem__(this, i);

    if (character == delimeter) {

      if (segment_start < index) {
        struct String segment =
            Stringsubstr(this, segment_start, index - segment_start);
        Vector_Stringpush(&split_result, segment);
        String__del__(&segment);
      }
      segment_start = index + 1;
    }
    index = index + 1;
  }

  if (segment_start < Stringlen(this)) {
    struct String remaining_segment =
        Stringsubstr(this, segment_start, Stringlen(this) - segment_start);
    Vector_Stringpush(&split_result, remaining_segment);
    String__del__(&remaining_segment);
  }

  return split_result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = this->length + strlen(pstring) + 1;

  if (new_length > this->capacity) {
    size_t new_capacity;
    if (this->capacity == 0) {
      new_capacity = new_length * 2;
    } else {
      new_capacity = this->capacity;
      while (new_capacity <= new_length) {
        new_capacity *= 2;
      }
    }
    this->arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
    this->capacity = new_capacity;
  }

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
  this->length = new_length;
}

void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length) {
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
}

void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring) {
  char *src = Stringc_str(&pstring);
  size_t p_text_length = Stringlen(&pstring);
  Stringreassign_internal(this, src, p_text_length);
}

void String__reassign__OVDstr(struct String *this, char *pstring) {
  size_t p_text_length = Stringlength_of_charptr(this, pstring);
  Stringreassign_internal(this, pstring, p_text_length);
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  // Read from the file & store the contents to this string.

  // TODO: Implement this function in ANIL itself, because the function below is
  // a mangled function name.
  Stringclear(this);

  FILE *ptr = fopen(pfilename, "r");
  if (ptr == NULL) {
    printf("File \"%s\" couldn't be opened.\n", pfilename);
    return;
  }

  char myString[256];
  bool has_data = false;

  while (fgets(myString, sizeof(myString), ptr)) {
    String__add__(this, myString);
    has_data = true;
  }

  fclose(ptr);

  if (!has_data) {
    // Double-clear just in case
    Stringclear(this);
  }
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

void File__init__(struct File *this, char *p_file_name) {
  this->file_ptr = fopen(p_file_name, "w");
  if (this->file_ptr == NULL) {
    printf("Failed to open file %s.\n", p_file_name);
    exit(0);
  }
}

void Filewriteline(struct File *this, char *p_content) {
  // Write a line to the file with terminating newline.
  fprintf(this->file_ptr, "%s\n", p_content);
}

void File__del__(struct File *this) { fclose(this->file_ptr); }

void JSON__init__(struct JSON *this) {
  Vector_String__init__(&this->parsed_strings, 1);
}

bool JSONParse(struct JSON *this, char *json_string, int len) {
  // Parse the JSON and store the strings in 'parsed_strings'.
  Vector_Stringclear(&this->parsed_strings);

  // 1. Input Validation
  if (!json_string || len <= 0) {
    fprintf(stderr, "JSON Error: Invalid input parameters.\n");
    return false;
  }

  // 2. Create Null-Terminated Copy
  // cJSON_Parse requires a null-terminated string.
  char *temp_json_buffer = (char *)malloc(len + 1);
  if (!temp_json_buffer) {
    fprintf(stderr, "JSON Error: Failed to allocate memory for JSON buffer.\n");
    return false;
  }

  memcpy(temp_json_buffer, json_string, len);
  temp_json_buffer[len] = '\0';

  // 3. Parse JSON
  cJSON *root = cJSON_Parse(temp_json_buffer);

  // 4. Free Temporary Buffer (no longer needed after parsing)
  free(temp_json_buffer);
  temp_json_buffer = NULL; // Avoid dangling pointer use

  // 5. Check for Parse Errors
  if (root == NULL) {
    const char *error_ptr =
        cJSON_GetErrorPtr(); // Get error location if available
    if (error_ptr != NULL) {
      fprintf(stderr, "JSON Error: JSON parsing failed near: %s\n", error_ptr);
    } else {
      fprintf(stderr, "JSON Error: JSON parsing failed (unknown location).\n");
    }
    // root is already NULL, nothing to delete
    return false;
  }

  // 6. Validate Root Type (must be an array)
  if (!cJSON_IsArray(root)) {
    fprintf(stderr, "JSON Error: Root JSON element is not an array.\n");
    cJSON_Delete(root); // Free the parsed JSON structure
    return false;
  }

  // 7. Iterate Through JSON Array
  int array_size = cJSON_GetArraySize(root);
  bool success = true; // Assume success initially

  for (int i = 0; i < array_size; i++) {
    cJSON *item = cJSON_GetArrayItem(root, i);

    // 8. Validate Item Type (must be a string)
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
      // 9. Create a String object and add it to the vector
      struct String todo_str;
      String__init__OVDstr(&todo_str, item->valuestring);
      Vector_Stringpush(&this->parsed_strings, todo_str);
      String__del__(&todo_str);
    } else {
      // Handle items that are not strings (e.g., numbers, null, objects)
      fprintf(stderr,
              "JSON Warning: Item at index %d is not a string. Skipping.\n", i);
      // Depending on requirements, we might want to set success = false here
      // success = false;
      // break; // Or stop processing immediately on invalid item
    }
  } // End of loop

  // 10. Cleanup cJSON object
  cJSON_Delete(root);

  // Return true if parsing and iteration were generally successful
  return success;
}

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

void Vector_String__init__(struct Vector_String *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct String *)malloc(capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  String__del__(&this->arr[index]);
}

void Vector_String_call_destructor_for_all_elements(
    struct Vector_String *this) {
  for (size_t i = 0; i < Vector_Stringlen(this); i++) {
    Vector_String_call_destructor_for_element(this, i);
  }
}

void Vector_String_reset(struct Vector_String *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_String__del__(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_reset(this);
}

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringvalidate_index(struct Vector_String *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s) {
  struct String string_copy;
  String__init__OVDstructString(&string_copy, s);
  return string_copy;
}

void Vector_String_set(struct Vector_String *this, int index,
                       struct String value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_set_ith_item(this, index, value);
}

void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_set(this, index, Vector_String_copy_string(this, value));
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct String *new_arr =
      (struct String *)realloc(this->arr, new_capacity * sizeof(struct String));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_String_grow_if_required(struct Vector_String *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_Stringallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_Stringallocate_more(this, 1);
    }
  }
}

void Vector_String_push(struct Vector_String *this, struct String value) {
  Vector_String_grow_if_required(this);
  Vector_Stringpush_unchecked(this, value);
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_push(this, Vector_String_copy_string(this, value));
}

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_String_shift_left_from(struct Vector_String *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_String_clear(struct Vector_String *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct String *)malloc(this->capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_Stringclear(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_clear(this);
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_4 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_4; i++) {
    struct String string = Vector_String__getitem__(this, i);

    if (Stringlen(&string) == Stringlen(&value)) {

      if (String__eq__(&string, Stringc_str(&value))) {
        return true;
      }
    }
  }
  return false;
}

void Vector_Stringprint(struct Vector_String *this) {
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"", this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

///*///

struct Vector_String LoadTodosFromFile() {
  struct String str;
  String__init__OVDstrint(&str, "", 0);
  struct Vector_String storedTodos = StringreadlinesFrom(&str, "todos.txt");

  struct Vector_String todos;
  Vector_String__init__(&todos, 1);

  if (Vector_Stringlen(&storedTodos) > 0) {
    // If todos.txt already exists and has some lines, then read the todos from
    // it.
    size_t tmp_len_1 = Vector_Stringlen(&storedTodos);
    for (size_t i = 0; i < tmp_len_1; i++) {
      struct String todo = Vector_String__getitem__(&storedTodos, i);

      if (String__eq__(&todo, "")) {
        continue;
      }

      if (String__eq__(&todo, "\n")) {
        continue;
      }
      Vector_Stringpush(&todos, todo);
    }
  }

  Vector_String__del__(&storedTodos);
  String__del__(&str);
  return todos;
}

void WriteTodosToFile(struct Vector_String todos) {
  struct File file;
  File__init__(&file, "todos.txt");

  size_t tmp_len_2 = Vector_Stringlen(&todos);
  for (size_t i = 0; i < tmp_len_2; i++) {
    struct String todo = Vector_String__getitem__(&todos, i);
    Filewriteline(&file, Stringc_str(&todo));
  }
  File__del__(&file);
}

struct String VectorStringToJSONString(struct Vector_String todos) {
  struct String jsonString;
  String__init__OVDstrint(&jsonString, "[", 1);

  int todo_count = 0;
  size_t tmp_len_3 = Vector_Stringlen(&todos);
  for (size_t i = 0; i < tmp_len_3; i++) {
    struct String todo = Vector_String__getitem__(&todos, i);
    String__add__(&jsonString, "\"");
    String__add__(&jsonString, Stringc_str(&todo));
    String__add__(&jsonString, "\"");

    if (!(todo_count == Vector_Stringlen(&todos) - 1)) {
      String__add__(&jsonString, ",");
    }
    todo_count = todo_count + 1;
  }

  String__add__(&jsonString, "]");

  return jsonString;
}

void Handle404(struct Response res, struct Request req) {
  char html[58] = "<html><body><h1>404 Page Not Found</h1></body></html>\r\n";
  Responsesend(&res, html, 404);
}

void HandleGetTodos(struct Response res, struct Request req) {
  struct Vector_String todos = LoadTodosFromFile();
  struct String jsonString = VectorStringToJSONString(todos);

  Responsesend(&res, Stringc_str(&jsonString), 200);
  String__del__(&jsonString);
  Vector_String__del__(&todos);
}

void HandleSaveTodos(struct Response res, struct Request request) {

  if (RequestValidateIsPostAndJSON(&request, res) == false) {
    return;
  }

  char *body_start = RequestGetBodyStart(&request);
  int body_len = RequestGetContentLength(&request);

  struct JSON jsonParser;
  JSON__init__(&jsonParser);

  if (JSONParse(&jsonParser, body_start, body_len)) {
    struct Vector_String todos = jsonParser.parsed_strings;
    WriteTodosToFile(todos);
    Responsesend(&res, "{\"message\":\"Todos saved successfully\"}", 201);
  } else {
    fprintf(stderr, "Failed to parse JSON body for POST /api/todos\n");
    Responsesend(&res, "{\"error\":\"Invalid JSON format\"}", 400);
  }
  JSON__del__(&jsonParser);
}

///*///

int main() {

  ///*///  main()

  struct HTTPServer server;
  HTTPServer__init__(&server);

  // Use reflection to get all functions with annotations '@route' and register
  // those functions as routes.
  HTTPServerregister_route(&server, "GET", "/get_todos", HandleGetTodos);
  HTTPServerregister_route(&server, "OPTIONS", "/save_todos", HandleSaveTodos);
  HTTPServerregister_route(&server, "POST", "/save_todos", HandleSaveTodos);

  HTTPServerlisten(&server, 8080);

  HTTPServer__del__(&server);
  ///*///

  return 0;
}