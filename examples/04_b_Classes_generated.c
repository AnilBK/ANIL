#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct Point {
  int x;
  int y;
};

void Pointsay(struct Point *this) {
  // We write normal C code inside c_function.
  // Use this-> pointer to access member variables.
  printf("x : %d , y : %d \n", this->x, this->y);
}

void Pointshout(struct Point *this) {
  printf("Shouting Point Values, x = %d.\n", this->x);
}

struct GenericStruct_float {
  float a;
  float b;
};

// template GenericStruct<float> {
// template GenericStruct<float> }

struct GenericStruct_int {
  int a;
  float b;
};

// template GenericStruct<int> {
// template GenericStruct<int> }

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

// template Vector<int> {
// template Vector<int> }

struct Vector_float {
  float *arr;
  int size;
  int capacity;
};

// template Vector<float> {
// template Vector<float> }

int main() {

  ///*///  main()

  struct Point p1;
  p1.x = 20;
  p1.y = 50;

  Pointsay(&p1);
  Pointshout(&p1);

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

  ///*///

  return 0;
}