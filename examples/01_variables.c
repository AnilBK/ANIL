#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()

  let x : int = 69420
  print("The magic number is {x}.\n");

  // Char uses " " but the contents inside it should be of length 1. 
  let gender : char = "M"
  print("The gender is {gender}. \n");

  let arr<int> = [1, 2, 3, 4, 5, 10];
  for value2 in arr{
    print("{value2} \n")
  }

  if 10 in arr{
    print("10 is in arr. \n")
  }

  let arr2<float> = [1, 2, 3, 4, 5, 10];
  for value3 in arr2{
    print("{value3} \n")
  }

  for i in range(1..10){
    print("{i}\n")
  }

  for i in range(1..10,2){
    print("{i}\n")
  }

  for i in range(1..=10){
    print("{i}\n")
  }

  for i in range(1..=10,2){
    print("{i}\n")
  }

  for i in range(1..=10,-2){
    print("{i}\n")
  }



  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}