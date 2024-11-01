///*///

///*///
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct UniquePtr_char {
  char *arr;
};

void UniquePtr_char__init__(struct UniquePtr_char *this, int capacity);
void UniquePtr_char__del__(struct UniquePtr_char *this);

void UniquePtr_char__init__(struct UniquePtr_char *this, int capacity) {
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void UniquePtr_char__del__(struct UniquePtr_char *this) { free(this->arr); }

int main() {

  ///*/// main()

  struct UniquePtr_char ptr;
  UniquePtr_char__init__(&ptr, 100);

  UniquePtr_char__del__(&ptr);
  ///*///

  return 0;
}