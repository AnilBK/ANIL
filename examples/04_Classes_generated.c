#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct Rectangle {
  int length;
  int breadth;
};

void Rectangle__init__(struct Rectangle *this, int l, int b);
void Rectanglearea(struct Rectangle *this);

void Rectangleprint_param(struct Rectangle *this, int param);

void Rectangle__init__(struct Rectangle *this, int l, int b) {
  this->length = l;
  this->breadth = b;
}

void Rectanglearea(struct Rectangle *this) {
  int area = 20;
  printf("Area of the rectangle is %d.\n", area);
}

void Rectangleprint_param(struct Rectangle *this, int param) {
  printf("Parameter provided to the rectangle is %d.\n", param);
}

int main() {

  struct Rectangle rect;
  Rectangle__init__(&rect, 10, 20);
  Rectanglearea(&rect);
  Rectangleprint_param(&rect, 10);

  return 0;
}