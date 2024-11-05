//gcc -o .\WebServer .\WebServer_generated.c -lws2_32
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import HTTPServer

function Handle404(res : Response)
  const html = "<html><body><h1>404 Page Not Found</h1></body></html>\r\n"
  res.send(html, 404)
endfunction

@route("GET /about")
function HandleAbout(res : Response)
  const html = "<html><body><h1>About Us Page</h1></body></html>\r\n"
  res.send(html, 200)
endfunction

@route("GET /home")
@route("GET / ")
function HandleHome(res : Response)
  const html = "<html><body><h1>Welcome to Home Page!</h1></body></html>\r\n"
  res.send(html, 200)
endfunction
///*///

int main() {

  // clang-format off
  ///*///  main()

  let server = HTTPServer{};
  
  // server.register_route("GET /home", HandleHome)
  // server.register_route("GET /about", HandleAbout)
  // server.register_route("GET / ", HandleHome)

  // Use reflection to get all functions with annotations '@route' and register those functions as routes.
  // This generates the code same as above.
  def register_routes_reflection():
    forall annotation_argument_value, annotated_fn_name in annotated_functions_by_name(route) UNQUOTE: server.register_route("annotation_argument_value", annotated_fn_name)
  enddef  
  register_routes_reflection
  
  server.listen(8080)

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}