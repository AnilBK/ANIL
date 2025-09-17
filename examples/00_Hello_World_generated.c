#include <stdio.h>

// IMPORTS //

void print_hello_world_in_ANIL();
void print_hello_world_in_c();

// ^^^ The above line denotes the start of ANIL code.
void print_hello_world_in_ANIL() {
  printf("Hello World from ANIL function.\n");
}

// In ANIL, we can implement functions in C.
void print_hello_world_in_c() {
  printf("Hello World from C function.\n");
  printf("Hello World from C function.\n");
}
// VVV The below line denotes the end of ANIL code.

int main() {

  // ^^^ Every ANIL program should start with the line above.

  printf("Hello World.\n");
  print_hello_world_in_ANIL();
  print_hello_world_in_c();

  return 0;
}