#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  // let color2 = Color["red"]
  // This will produce error below, because Color has "Red" and not "red".

  // Currently we Support {"String" : int} dictionary only.
  
  ///*/// main()

  // Compile time dictionaries.
  // These acts as contants, and the individual values of these constants
  // will be placed in code at compile time.

  constexpr Color = {"Red":255, "Green":200}
  constexpr Age = {"Anil":22, "Universe":10000}

  let color = Color["Red"]
  let color2 = Color["Green"]

  let age1 = Age["Anil"]
  let age2 = Age["Universe"]

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}