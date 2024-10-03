#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// main()
  import Vector
  import String

  let a = Vector<int>{10};
  a.push(10)
  a.push(20)
  a.push(30)
  a.push(40)
  a.push(50)
  a.print()

  if 10 in a{
    print("10 is in the vector. \n")
  }

  print("Removing index 2 item.\n")
  a.remove_at(2)

  a.print()
  print("\n")

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

  let str = "Hello"
  let str2 = "World"
  let str3 = "Honey"
  let str4 = "Bunny"
  
  let test = Vector<String>{5};
  // Push Strings to Vector<String>
  test.push(str)
  test.push(str2)
  test.push(str3)
  test.push(str4)
  test.print()

  for tst in test[::-1]{
    tst.print()
  }

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}