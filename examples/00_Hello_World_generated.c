#include <stdio.h>

// IMPORTS //

void print_hello_world_in_CPL();

///*///
// ^^^ The above line denotes the start of CPL code.
void print_hello_world_in_CPL() { printf("Hello World from CPL function.\n"); }
// VVV The below line denotes the end of CPL code.
///*///

int main() {

  ///*///  main()
  // ^^^ Every CPL program should start with the line above.

  printf("Hello World.\n");
  print_hello_world_in_CPL();

  ///*///

  return 0;
}