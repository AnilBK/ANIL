#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///
  import List

  let test_list = [];
  // The following line is also valid, as the above syntax is shorthand for the statement below. 
  // let test_list = List{};

  test_list.append_str("Hello")
  test_list.append_str("World")
  test_list.append_str("Puppy")

  test_list.append_int(10)
  test_list.append_int(20)
  test_list.append_int(30)
  test_list.append_int(40)
  test_list.append_int(50)

  let node = test_list.pop(7)

  test_list.print()

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}