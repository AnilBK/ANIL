#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import String
  import List

  let str = String{"Hello"};
  str.printLn()
  
  str.set_to_file_contents("fileexample.c")

  str.printLn()

  let l = String{""};
  let line = List{};

  # Our own split by newlines algorithm.

  for c in str{
    if c == "\n"{
      line.append_str(l)
      l = ""
      continue;
    }

    l += c
    
  }

  line.print()

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}