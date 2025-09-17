#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// ^^^ The above line denotes the start of ANIL code.
function print_hello_world_in_ANIL()
  print("Hello World from ANIL function.\n")
endfunction

// In ANIL, we can implement functions in C.
c_function print_hello_world_in_c()
  printf("Hello World from C function.\n");
  printf("Hello World from C function.\n");
endc_function
// VVV The below line denotes the end of ANIL code.
///*///

int main() {

  // clang-format off

  ///*///  main()
  // ^^^ Every ANIL program should start with the line above.

  print("Hello World.\n")
  print_hello_world_in_ANIL()
  print_hello_world_in_c()

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}