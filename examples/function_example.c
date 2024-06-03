#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// clang-format off
import Vector
import String

function my_first_CPL_function()
    print("Hello World from function.")
endfunction

function get_format_specifier(p_type: String) -> String:
  let return_type_str = String{"d"};

  if p_type == "char"{
    return_type_str = "c"
  }else if p_type == "int"{
    return_type_str = "d"
  }else if p_type == "float"{
    return_type_str = "f"
  }else if p_type == "size_t"{
    return_type_str = "llu"
  }

  return return_type_str
endfunction

function get_mangled_fn_name(p_struct_type: String, p_fn_name: String) -> String:
  let s = String{p_struct_type};
  s += p_fn_name
  return s
endfunction

function get_templated_mangled_fn_name(p_struct_type1: String, p_fn_name1: String, p_templated_data_type1: String) -> String:
  let s1 = String{p_struct_type1};
  s1 += "_"
  s1 += p_templated_data_type1
  s1 += p_fn_name1
  return s1
endfunction

///*///
    // clang-format on

    int
    main() {
  // clang-format off

  ///*///
  let string = String{"Hello World from String.\n"};
  string.print()

  let s5 = String{string};

  my_first_CPL_function();

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}