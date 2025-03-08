#include <stdio.h>

// IMPORTS //

void print_hello_world_in_ANIL();

///*///
// ^^^ The above line denotes the start of ANIL code.
void print_hello_world_in_ANIL() {
  printf("Hello World from ANIL function.\n");
}
// VVV The below line denotes the end of ANIL code.
///*///

int main() {

  ///*///  main()
  // ^^^ Every ANIL program should start with the line above.

  printf("Hello World.\n");
  print_hello_world_in_ANIL();

  ///*///

  return 0;
}