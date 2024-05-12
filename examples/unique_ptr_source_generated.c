///*///

///*///
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct UniquePtr__char {
  char *arr;
};

// template UniquePtr<char> {
void UniquePtr_char__init__(struct UniquePtr__char *this, int capacity) {
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void UniquePtr_char__del__(struct UniquePtr__char *this) { free(this->arr); }

// template UniquePtr<char> }

int main() {

  ///*///

  struct UniquePtr__char ptr;
  UniquePtr_char__init__(&ptr, 100);

  UniquePtr_char__del__(&ptr);
  ///*///

  return 0;
}