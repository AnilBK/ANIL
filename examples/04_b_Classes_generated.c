#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct Point {
  int x;
  int y;
};

struct GenericStruct_float {
  float a;
  float b;
};

struct GenericStruct_int {
  int a;
  float b;
};

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

struct Vector_float {
  float *arr;
  int size;
  int capacity;
};

void Pointsay(struct Point *this);
void Pointshout(struct Point *this);
int Pointstatic_fn_test();
void Pointstatic_fn();

void Pointsay(struct Point *this) {
  // We write normal C code inside c_function.
  // Use this-> pointer to access member variables.
  printf("x : %d , y : %d \n", this->x, this->y);
}

void Pointshout(struct Point *this) {
  printf("Shouting Point Values, x = %d.\n", this->x);
}

int Pointstatic_fn_test() { return 10; }

void Pointstatic_fn() { printf("Test.\n"); }

int main() {

  struct Point p1;
  p1.x = 20;
  p1.y = 50;

  Pointsay(&p1);
  Pointshout(&p1);

  int static_result = Pointstatic_fn_test();
  printf("%d", static_result);
  printf("\n");
  Pointstatic_fn();

  struct GenericStruct_float t1;
  t1.a = 10;
  t1.b = 20;
  struct GenericStruct_int t2;
  t2.a = 10;
  t2.b = 20;

  struct Vector_int vec2;
  vec2.arr = NULL;
  vec2.size = 10;
  vec2.capacity = 20;
  struct Vector_float vec3;
  vec3.arr = NULL;
  vec3.size = 10;
  vec3.capacity = 20;

  return 0;
}