#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Test {
  char dummy;
};

void ANIL_function(int p);
void call_func(void (*F)(int), int value);
void Testcall_func(struct Test *this, void (*F)(int), int value);

void Testcall_func(struct Test *this, void (*F)(int), int value) { F(value); }

void ANIL_function(int p) { printf("Hello World from function.\n"); }

void call_func(void (*F)(int), int value) { F(value); }

int main() {

  call_func(ANIL_function, 5);

  struct Test t;
  Testcall_func(&t, ANIL_function, 5);

  return 0;
}