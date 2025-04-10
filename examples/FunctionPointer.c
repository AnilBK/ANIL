#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// clang-format off

function ANIL_function(p: int)
  print("Hello World from function.\n")
endfunction

function call_func(F: Fn(int) -> void, value : int)
  F(value);
endfunction

struct Test{char dummy};

namespace Test

function call_func(F: Fn(int) -> void, value : int)
  F(value);
endfunction

endnamespace

///*///
// clang-format on

int main() {
  // clang-format off

  ///*/// main()

  call_func(ANIL_function, 5)

  let t = Test{};
  t.call_func(ANIL_function, 5)

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}