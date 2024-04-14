#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

// clang-format off

  ///*///
  import Vector

  let values = Vector<int>{8};
  values.push 1
  values.push 2
  values.print

  values.pushn 40 50 60
  values.print

  // DESTRUCTOR_CODE //
  ///*///
                    

  return 0;
}