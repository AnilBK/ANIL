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
  import File

  print("Writing to file hello.txt:\n")
  let outputFile = File{"hello.txt"};
  outputFile.writeline("Hello World :)")


  print("Reading from a file(10_FileIO.c) to a string:\n")
  let input_str = ""
  input_str.set_to_file_contents("10_FileIO.c")
  input_str.printLn()

  // DESTRUCTOR_CODE //
  ///*///
  
  // clang-format on

  return 0;
}