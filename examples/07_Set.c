#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///
  import Vector
  import String
  import Set

  let s1 = "apple"
  let s2 = "banana"
  let s3 = "cherry"

  let s = Set{10};
  s.add(s1)
  s.add(s1)
  s.add(s2)
  s.add(s2)
  s.add(s3)
  s.print()

  for str in s{
    str.printLn()
  }
  
  // DESTRUCTOR_CODE //
  ///*///
                    

  return 0;
}