#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// clang-format off
function my_first_CPL_function()
    print("Hello World from function.")
endfunction
///*///
    // clang-format on

    int main() {
  // clang-format off

  ///*///
  import Vector
  import String

  let string = String{"Hello World from String.\n"};
  string.print()

  my_first_CPL_function();

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}