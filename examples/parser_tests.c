#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///

  # This is for testing arrays, created with constexpr dictionary & numeric constants.
  # Functions and struct member access still not supported.
  constexpr CONSTANTS = {"VALUE_1":1, "VALUE_4":4}
  let arr<int> = [CONSTANTS["VALUE_1"], 2, 3, CONSTANTS["VALUE_4"]];

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}