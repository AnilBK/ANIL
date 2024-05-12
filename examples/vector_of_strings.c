#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //
int main() {

// clang-format off

  ///*///
  import String
  import Vector
  
  let string = Vector<char>{10};
  string.push("A")
  string.push("N")
  string.push("I")
  string.push("L")
  string.print()

  let test = Vector<String>{5};

  #let split = string.split(",");
  # TODO : implement this 

  // DESTRUCTOR_CODE //
  ///*///
  ;
  // clang-format on

  return 0;
}