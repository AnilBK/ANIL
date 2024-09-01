#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
// clang-format off
import Vector
import String

# Our own split by newlines algorithm.
function string_split(str: String) -> Vector<String>:
  let lines = Vector<String>{5};

  let line = ""

  for c in str{
    if c == "\n"{
      lines.push(line)
      line = ""
      continue;
    }

    line += c
  }

  // Add the remaining strings which don't end with \n.
  if line != ""{
    lines.push(line)
    line = ""
  }

  return lines
endfunction

///*///

int main() {

  ///*/// main() 
  let str = "Hello"
  str.printLn()
  
  str.set_to_file_contents("fileexample.c")

  str.printLn()

  let splitted_lines = string_split(str);
  for split_line in splitted_lines{
    split_line.printLn()
  }

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

  return 0;
}