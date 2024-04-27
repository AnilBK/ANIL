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

  let a = Vector<int>{10};
  a.push(10)
  a.push(20)
  a.push(30)
  a.push(40)
  a.push(50)
  a.print()

  if 10 in a{
    print(f "10 is in the vector. \n");
  }


  let b = Vector<float>{10};
  b.push(10)
  b.push(40)
  b.push(50)
  b.print()

  let string = Vector<char>{10};
  string.push("A")
  string.push("N")
  string.push("I")
  string.push("L")
  string.print()

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}