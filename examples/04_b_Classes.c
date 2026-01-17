#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()

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

  @static
  function static_fn_test() -> int:
    return 10
  endfunction

  @static
  c_function static_fn()
    printf("Test.\n");
  endc_function
  endnamespace

  p1.say()
  p1.shout()
  
  let static_result = Point::static_fn_test();
  print("{static_result}\n");
  Point::static_fn();

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