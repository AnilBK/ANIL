#include <stdbool.h>
#include <stdio.h>

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  int meaning = 42;

  // clang-format off

  ///*///
  print(f "Hello {meaning} , {meaning} \n");

  struct Point {int x, int y};
  let p1 = Point{20, 50};

  impl Point say
  // We write normal C code inside impl.
  // Use this-> pointer to access member variables.
  printf("x : %d , y : %d \n", this->x, this->y);
  endfunc

  impl Point shout
  printf("XXXXX = %d", this->x);
  endfunc

  p1.say
  p1.shout

  struct Rectangle{float l, float b};
  impl Rectangle Area
  printf("Area is : %d \n", this->l * this->b);
  endfunc

  let rect1 = Rectangle{20,10};
  rect1.Area

  let arr<float> = [1,2,3,4,5];

  for val in arr{
    print(f "{val}\n");
  }
  
  struct GenericStruct<X>{X a, float b};

  let t1 = GenericStruct<float>{10,20};
  let t2 = GenericStruct<int>{10,20};

  struct Vector<T>{T* arr,int size,int capacity};

  let vec2 = Vector<int>{NULL, 10, 20};
  let vec2 = Vector<float>{NULL, 10, 20};

  ///*///
  // clang-format on

  return 0;
}