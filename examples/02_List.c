#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()
  import List

  let test_list = [];
  // The following line is also valid, as the above syntax is shorthand for the statement below. 
  // let test_list = List{};

  // append is an overloaded function.
  // append_str() and append_int() are explicit versions, 
  // which are called by the overloaded append functions.
  // test_list.append_str("Hello")
  // test_list.append_int(10)

  test_list.append("Hello")
  test_list.append("World")
  test_list.append("Puppy")

  test_list.append(10)
  test_list.append(20)
  test_list.append(30)
  test_list.append(40)
  test_list.append(50)

  let node = test_list.pop(7)

  test_list.print()

  if 10 in test_list{
    print("10 is in the list.\n")
  }

  if 5 not in test_list{
    print("5 is not in the list.\n")
  }

  print("Duplicating List:")
  let test_list2 = []
  test_list2 = test_list
  test_list2.print()

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}