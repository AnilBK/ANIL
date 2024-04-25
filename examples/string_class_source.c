#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///

  import String

  let str = String{"Hello World"};
  str.printLn
  
  str = "Reassign"
  str.printLn

  str = ""
  str.printLn

  let str2 = String{"Hi \n"};
  str2.print

  let str3 = str2.strip()
  str3.printLn

  let len = str3.len()
  print(f "Length of the string is : {len}. \n");

  for val in str{
   print(f "{val} \n"); 
  }

  if "Wor" in str{
    print(f "Wor is in str. \n");
  }

  if str == "Hello World"{
    puts("Str is Hello World.");
  }

  str += "New message appended at the end. "
  str.printLn

  str = ""

  let len4 = str.len()
  print(f "Length of the string is : {len4}. \n");
  str.printLn

  let is_5_characters_long = str.is_of_length(5)
  
  let second_char = str[1]
  print(f "Second character is : {second_char}. \n");

  // DESTRUCTOR_CODE //


  ///*///
  // clang-format on
  // let str = String{"Hello World"};
  // let str2 = str.strip();
  /*

  Note : Two Templated Structs in a class causes error when writing
  __init__.
  TODO ??

  let string = Vector<char>{10};
  string.push 65
  string.push 66
  string.push 67
  string.print
  */
  return 0;
}