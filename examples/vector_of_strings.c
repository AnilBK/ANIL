#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //
int main() {

// clang-format off

  ///*///
  import Vector
  import String
  
  let string = Vector<char>{10};
  string.push("A")
  string.push("N")
  string.push("I")
  string.push("L")
  string.print()

  let str = String{"Hello"};
  let str2 = String{"World"};
  let str3 = String{"Honey"};
  let str4 = String{"Bunny"};

  let test = Vector<String>{5};
  test.push(str)
  test.push(str2)
  test.push(str3)
  test.push(str4)
  test.print()


  #let split = string.split(",");
  # TODO : implement this 

  // DESTRUCTOR_CODE //
  ///*///
  ;
  // clang-format on

  return 0;
}