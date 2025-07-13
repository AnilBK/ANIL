#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// main()

  import Vector
  import String

  let p_index : int = 5

  let s1 = "Hello World"

  if s1[2] == "\\"{
    s1 += "\\"
  }

  let string_to_insert = "*****"

  let left_part = s1.substr(0, p_index)
  left_part += string_to_insert + s1.substr(p_index, s1.len() - p_index)

  let vec = Vector<int>{10};

  vec.push(10)
  vec.push(20)

  let x : int = 69420
  let y : int = s1.len()
  let v : int = s1.len() + x + y - 100 - s1.len() + vec[0]

  print("The magic number is {x}.\n");
  print("Expression value is is {v}.\n");

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}