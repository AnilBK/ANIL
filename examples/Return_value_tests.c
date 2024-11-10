#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String
import Optional

function return_test_bool(p_str : String) -> bool:
  let a = "Test String"
  return p_str in a
endfunction

function return_test_struct_in_struct(p_str : String) -> bool:
  let z = Vector<String>{5};
  return p_str in z
endfunction

function return_test_equals(p_str : String, p_str2 : String) -> bool:
  return p_str == p_str2
endfunction

function return_test_equals2(p_str : String) -> bool:
  let a = "Test String"
  return p_str == a
endfunction

function return_test_equals3() -> bool:
  let a = "Test String"
  let b = "Test String"
  return a == b
endfunction

function return_normal_value() -> String:
  let a = "Test String"
  return a
endfunction

function return_test_array() -> bool:
  let arr<int> = [1, 2, 3, 4, 5, 10];

  if 10 in arr{
    return true
  }

  return false
endfunction

function return_test_array2() -> bool:
  let value : int = 10
  let arr<int> = [1, 2, 3, 4, 5, 10];

  return value in arr
endfunction

///*///

int main() {

  // clang-format off

  ///*/// main()

   print("Code Generation Tests for return expressions, Just check if the code generated is valid.")

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}