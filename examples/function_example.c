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

function get_mangled_fn_name(p_struct_type: String, p_fn_name: String) -> String:
  let _p1 = p_struct_type.c_str();
  let s = String{_p1};
  s += p_fn_name
  return s
endfunction

function get_templated_mangled_fn_name(p_struct_type1: String, p_fn_name1: String, p_templated_data_type1: String) -> String:
  let _p11 = p_struct_type1.c_str();
  let s1 = String{_p11};
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

  let s1 = string.c_str()
  let s = String{s1};

  my_first_CPL_function();

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}