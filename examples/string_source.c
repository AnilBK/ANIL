#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import String
  
  let str = String{"Hello"};
  str.printLn()
  
  for value in str{
    print(f "{value}.\n"); 
  }

  if str == "Hello"{
    print(f "Str is hello.\n"); 
  }

  str += " World";
  str.printLn()
  
  str += " Its me Anil"
  str.printLn()

  if "Anil" in str { 
    print(f "Anil is in the given string.\n"); 
  }

  let arr<int> = [1, 2, 3, 4, 5, 10];

  if 10 in arr{
    print(f "10 is in arr. \n");
  }

  for value2 in arr{
    print(f "{value2} \n"); 
  }

  let arr2<float> = [1, 2, 3, 4, 5, 10];
  for value3 in arr2{
    print(f "{value3} \n"); 
  }

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}