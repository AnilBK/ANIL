#include <stdio.h>

struct array {
  int elements[5];
  unsigned int size;
};

int main() {

  struct array arr;
  arr.elements[0] = 1;
  arr.elements[1] = 2;
  arr.elements[2] = 3;
  arr.elements[3] = 4;
  arr.elements[4] = 5;
  arr.size = 5;

  for (unsigned int i = 0; i < arr.size; i++) {
    int val = arr.elements[i];
    printf("%d \n", val);
  }

  return 0;
}