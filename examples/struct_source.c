#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  int meaning = 42;

  // clang-format off

  ///*///
  print("Hello {meaning} , {meaning} \n")

  struct Point {int x, int y};
  let p1 = Point{20, 50};

  namespace Point
  c_function say()
    // We write normal C code inside c_function.
    // Use this-> pointer to access member variables.
    printf("x : %d , y : %d \n", this->x, this->y);
  endc_function

  c_function shout()
    printf("Shouting Point Values, x = %d.\n", this->x);
  endc_function
  endnamespace

  p1.say()
  p1.shout()

  struct Rectangle{float l, float b};
  namespace Rectangle
  c_function Area()
    printf("Area is : %f.\n", this->l * this->b);
  endc_function
  endnamespace

  let rect1 = Rectangle{20,10};
  rect1.Area()

  let arr<float> = [1,2,3,4,5];

  for val in arr{
    print("{val}\n")
  }
  
  struct GenericStruct<X>{X a, float b};

  let t1 = GenericStruct<float>{10,20};
  let t2 = GenericStruct<int>{10,20};

  struct Vector<T>{T* arr,int size,int capacity};

  let vec2 = Vector<int>{NULL, 10, 20};
  let vec3 = Vector<float>{NULL, 10, 20};


  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}