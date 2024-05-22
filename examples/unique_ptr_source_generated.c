///*///

///*///
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct UniquePtr_char {
  char *arr;
};

// template UniquePtr<char> {
void UniquePtr_char__init__(struct UniquePtr_char *this, int capacity) {
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void UniquePtr_char__del__(struct UniquePtr_char *this) { free(this->arr); }

// template UniquePtr<char> }

int main() {

  ///*///

  struct UniquePtr_char ptr;
  UniquePtr_char__init__(&ptr, 100);

  UniquePtr_char__del__(&ptr);
  ///*///

  return 0;
}