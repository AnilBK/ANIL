#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

// clang-format off

  ///*/// main()
  import File

  let outputFile = File{"hello.txt"};
  outputFile.writeline("Hello World :)")

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}