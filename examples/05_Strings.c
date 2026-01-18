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

  const c_str1 = "C Style Strings\n"
  // Such strings are very limited in functionality and only looping as below is supported.
  for c in c_str1{
    print(c)
  }

  let str = "Hello World"
  // The following line is also valid, as the above syntax is shorthand for the statement below. 
  // let str = String{"Hello World"};
  print(str)

  str = "Reassign"
  print(str)
 
  let str2 = "Hi"
  print(str2)

  let str3 = str2.strip()
  print(str3)

  let len = str3.len()
  print("Length of the string is : {len}.")

  for val in str{
   print("{val}")
  }

  if "Wor" in str{
    print("Wor is in str.")
  }

  if str == "Hello World"{
    puts("Str is Hello World.");
  }

  str += "New message appended at the " + "end."
  print(str)

  str = ""

  let len4 = str.len()
  print("Length of the string is : {len4}.")
  print(str)

  let str4 = "String constructed from another string."
  let str5 = String{str4};
  print(str5)

  let str6 = "String constructed from another string 2."
  let str7 = str6
  print(str7)

  let substr_str = str4.substr(0, 6)
  print(substr_str)
  
  print("Split Test:")
  let split_str = "Splitting.with.dots."
  print(split_str)

  let dot_split = split_str.split(".")
  dot_split.print()  

  let split_str2 = "Splitting with Spaces."
  print(split_str2)

  let space_split = split_str2.split(" ")
  space_split.print()

  let score: int = 69420
  print(score)
  
  let scoreText = "Score: " + String::from(score)
  print(scoreText)

  // DESTRUCTOR_CODE //

  ///*///
  
  return 0;
}