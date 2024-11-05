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
    print("{c}")
  }

  let str = "Hello World"
  // The following line is also valid, as the above syntax is shorthand for the statement below. 
  // let str = String{"Hello World"};
  str.printLn()

  str = "Reassign"
  str.printLn()
 
  let str2 = "Hi \n"
  str2.print()

  let str3 = str2.strip()
  str3.printLn()

  let len = str3.len()
  print("Length of the string is : {len}. \n")

  for val in str{
   print("{val} \n")
  }

  if "Wor" in str{
    print("Wor is in str. \n")
  }

  if str == "Hello World"{
    puts("Str is Hello World.");
  }

  str += "New message appended at the " + "end."
  str.printLn()

  str = ""

  let len4 = str.len()
  print("Length of the string is : {len4}. \n")
  str.printLn()

  let str4 = "String constructed from another string. \n"
  let str5 = String{str4};
  str5.printLn()

  let substr_str = str4.substr(0, 6);
  substr_str.printLn()
  
  print("Split Test: \n")
  let split_str = "Splitting.with.dots."
  split_str.printLn()
  let dot_split = split_str.split(".")
  dot_split.print()

  let split_str2 = "Splitting with Spaces."
  split_str2.printLn()
  let space_split = split_str2.split(" ")
  space_split.print()

  // DESTRUCTOR_CODE //

  ///*///
  
  return 0;
}