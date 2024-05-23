#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import Vector
  import String
  
  print("Split Test: \n")

  let str = String{"Splitting.with.dots."};
  str.printLn()

  let split = str.split(".")
  split.print();

  let str2 = String{"Splitting with Spaces."};
  str2.printLn()

  let space_split = str2.split(" ")
  space_split.print();

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}