#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// main()
  import Vector

  let a = Vector<char>{10};
  a.push("1")
  a.push("2")
  a.push("3")
  a.push("4")
  a.push("5")

  print("Printing in normal order(step 1): 1,2,3,4,5 \n")
  for a1 in a[::1]{
    print("{a1} ")
  }

  print("\nPrinting in normal order(step 1) in between (2,..): 3,4,5 \n")
  for a1 in a[2::1]{
    print("{a1} ")
  }

  print("\nPrinting in normal order(step 1) in between (2,4): 3,4 \n")
  for a1 in a[2:4:1]{
    print("{a1} ")
  }

  print("\nPrinting in normal order(step 2): 1,3,5 \n")
  for a1 in a[::2]{
    print("{a1} ")
  }

  print("\nPrinting in reverse order: 5,4,3,2,1 \n")
  for a1 in a[::-1]{
    print("{a1} ")
  }

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}