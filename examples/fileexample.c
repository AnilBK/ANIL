#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import String
  
  let str = String{"Hello"};
  str.printLn()
  
  str.set_to_file_contents("fileexample.c")

  let len = str.len()
  print(f "String length is : {len}");

  str.printLn()

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}