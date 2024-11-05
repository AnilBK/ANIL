#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// ^^^ The above line denotes the start of CPL code.
function print_hello_world_in_CPL()
  print("Hello World from CPL function.\n")
endfunction
// VVV The below line denotes the end of CPL code.
///*///

int main() {

  // clang-format off

  ///*///  main()
  // ^^^ Every CPL program should start with the line above.

  print("Hello World.\n")
  print_hello_world_in_CPL()

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}