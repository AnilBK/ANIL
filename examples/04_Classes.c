#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

// clang-format off

///*/// 
struct Rectangle{int length, int breadth};

function area() for Rectangle
  let area : int = 20;
  print("Area of the rectangle is {area}.\n")
endfunction


function print_param(param : int) for Rectangle
  print("Parameter provided to the rectangle is {param}.\n")
endfunction

///*///

int main() {

  // clang-format off

  ///*/// 
  
  let rect = Rectangle{10, 20};
  rect.area();
  rect.print_param(10);

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}