// Run this file and then open Todos.html in a browser to see the UI.

// gcc -o .\TodoAppWebServer .\TodoAppWebServer_generated.c cJSON.c -lws2_32

#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String
import HTTPServer
import JSON
import File

function LoadTodosFromFile() -> Vector<String>:
  let str = ""
  let storedTodos = str.readlinesFrom("todos.txt")

  let todos = Vector<String>{1};

  if storedTodos.len() > 0{
    // If todos.txt already exists and has some lines, then read the todos from it.
    for todo in storedTodos{
        if todo == "" {
          continue;
        }
        if todo == "\n" {
          continue;
        }
        todos.push(todo)
    }
  }

  return todos
endfunction

function WriteTodosToFile(todos: Vector<String>)
  let file = File{"todos.txt"};

  for todo in todos{
    file.writeline(todo)
  }
endfunction

function VectorStringToJSONString(todos: Vector<String>) -> String:
  let jsonString = "["

  let todo_count : int = 0
  for todo in todos{
    jsonString += "\"" + todo + "\""
    if todo_count != todos.len() - 1 {
      jsonString += ","
    }
    todo_count = todo_count + 1
  }
  
  jsonString += "]"
  
  return jsonString
endfunction

function Handle404(res : Response, req : Request)
  const html = "<html><body><h1>404 Page Not Found</h1></body></html>\r\n"
  res.send(html, 404)
endfunction

@route("GET", "/get_todos")
function HandleGetTodos(res : Response, req : Request)
  let todos = LoadTodosFromFile()
  let jsonString = VectorStringToJSONString(todos)

  res.send(jsonString, 200)
endfunction

@routeWithValidation("OPTIONS", "/save_todos", "")
@routeWithValidation("POST", "/save_todos", "application/json")
function HandleSaveTodos(res : Response, request : Request)
  let jsonParser = JSON{};
  let todos = jsonParser.ParseRequest(request)

  if todos.len() == 0 {
    fprintf(stderr, "Failed to parse JSON body for POST /save_todos\n");
    res.send("{\"error\":\"Invalid JSON format\"}", 400) // Bad Request
  } else{
    WriteTodosToFile(todos)
    res.send("{\"message\":\"Todos saved successfully\"}", 201) // Created
  }
endfunction

///*///

int main() {

  // clang-format off
  ///*///  main()

  let server = HTTPServer{};
  
  // Use reflection to get all functions with annotations '@route' and register those functions as routes.
  def register_routes_reflection():
    forall annotated_fn_name, arg_value1, arg_value2 in annotated_functions_by_name(route) UNQUOTE: server.register_route("arg_value1", "arg_value2", annotated_fn_name)
    
    forall annotated_fn_name, arg_value1, arg_value2, arg_value3 in annotated_functions_by_name(routeWithValidation) UNQUOTE: server.register_route("arg_value1", "arg_value2", annotated_fn_name)
    forall annotated_fn_name, arg_value1, arg_value2, arg_value3 in annotated_functions_by_name(routeWithValidation) UNQUOTE: server.add_validation_rule("arg_value2", "arg_value1", "arg_value3")
  enddef  
  register_routes_reflection
  
  server.listen(8080)

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}