#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import Vector
  import String
  
  let str = String{"Hello.World.Split.test"};
  str.printLn()


  let split = str.split()
  split.print();


  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}