#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef void (*fn_ptr)();

typedef struct  {
  char *key_str;
} Key;

typedef struct  {
  Key key;
  fn_ptr value;
}KeyValuePair;

typedef struct  {
    KeyValuePair pair[45];
}KeyValuePairs;

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///

struct FunctionDictionary{int added_values,KeyValuePairs pairs};

namespace FunctionDictionary
c_function __init__()
  this->added_values = 0;
endc_function

c_function add_key_value(p_key_str : str, p_value : fn_ptr)
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
endc_function

c_function HandleRequest(p_str : str) {
  for (int i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    if(strcmp(key, p_str) == 0){
      fn_ptr value = this->pairs.pair[i].value;
      value();
      return;
    }
  }
  printf("%s is not a valid route.\n", p_str);
endc_function
endnamespace

@default_route("home")
@route("/Home")
function Home()
  print("Hello World\n")
endfunction

@route("/About")
function About()
  print("Anil BK\n")
endfunction

///*///

int main() {

  // clang-format off

  ///*/// main()

  let router = FunctionDictionary{};

  // Add all the annotated functions using a macro.
  // The macro performs text replace operations so, "annotation_argument_value" in the macro below is not a string. 
  // It will be replaced by actual value of 'annotation_argument_value'.

  def reflection():
    forall annotation_argument_value, annotated_fn_name in annotated_functions_by_name(route) UNQUOTE: router.add_key_value("annotation_argument_value", annotated_fn_name)
    forall annotation_argument_value, annotated_fn_name in annotated_functions_by_name(default_route) UNQUOTE: router.add_key_value("annotation_argument_value", annotated_fn_name)
  enddef  
  reflection

  router.HandleRequest("/Home")
  router.HandleRequest("/About")
  router.HandleRequest("/Home/1")

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}