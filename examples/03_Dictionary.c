#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()
  import Dictionary

  // Compile time dictionary. See constexpr_dict.c
  // Also See, Bootstrap/lexer_test.c.
  constexpr NUMBERS = {"ONE" : 1, "TWO" : 2}

  let dict = {};
  // The following line is also valid, as the above syntax is shorthand for the statement below. 
  // let dict = Dictionary<int>{};
  dict["One"] = NUMBERS["ONE"]
  dict["Two"] = NUMBERS["TWO"]
  dict["Three"] = 3

  dict.print()

  // DESTRUCTOR_CODE //
  ///*///
      
  // clang-format on

  return 0;
}