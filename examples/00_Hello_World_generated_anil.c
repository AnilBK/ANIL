
#include <stdio.h>

void print_hello_world_in_ANIL();
int main();

void print_hello_world_in_ANIL() {
  printf("Hello World from ANIL function.\n");
}

int main() {
  printf("Hello World.\n");
  print_hello_world_in_ANIL();

  return 0;
}
