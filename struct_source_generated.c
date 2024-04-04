#include <stdbool.h>
#include <stdio.h>

struct Point {
  int x;
  int y;
};

void say(struct Point *this) {
  // We write normal C code inside impl.
  // Use this-> pointer to access member variables.
  printf("x : %d , y : %d \n", this->x, this->y);
}

void shout(struct Point *this) { printf("XXXXX = %d", this->x); }

struct Rectangle {
  float l;
  float b;
};

void Area(struct Rectangle *this) {
  printf("Area is : %d \n", this->l * this->b);
}

struct GenericStruct__float {
  float a;
  float b;
};

struct GenericStruct__int {
  int a;
  float b;
};

struct Vector__int {
  int *arr;
  int size;
  int capacity;
};

struct Vector__float {
  float *arr;
  int size;
  int capacity;
};

int main() {

  int meaning = 42;

  ///*///
  printf("Hello %d , %d \n ", meaning, meaning);

  struct Point p1;
  p1.x = 20;
  p1.y = 50;

  say(&p1);

  shout(&p1);

  struct Rectangle rect1;
  rect1.l = 20;
  rect1.b = 10;
  Area(&rect1);

  float arr[] = {1, 2, 3, 4, 5};
  unsigned int arr_array_size = 5;

  for (unsigned int i = 0; i < arr_array_size; i++) {
    int val = arr[i];
    printf("%d\n ", val);
  }

  struct GenericStruct__float t1;
  t1.a = 10;
  t1.b = 20;
  struct GenericStruct__int t2;
  t2.a = 10;
  t2.b = 20;

  struct Vector__int vec2;
  vec2.arr = NULL;
  vec2.size = 10;
  vec2.capacity = 20;
  struct Vector__float vec2;
  vec2.arr = NULL;
  vec2.size = 10;
  vec2.capacity = 20;

  ///*///

  return 0;
}