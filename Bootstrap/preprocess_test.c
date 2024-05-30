#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*/// 

  import Vector
  import String

  let source_file = String{"../examples/string_source.c"};
  #source_file.printLn()

  # output_file_name = source_file.split(".")[0] + "_generated.c"
  #let _split = source_file.split(" ")
  #let output_file_name = _split[0]
  #output_file_name += "_generated.c"
  #output_file_name.print()


  let file = String{""};
  file.printLn()

  let Lines = file.readlinesFrom(source_file)
  Lines.print()
  
  let imported_modules = Vector<String>{5};

  for line in Lines{
    let Line = line.strip()
    if Line.startswith("import"){
      let import_split = Line.split(" ")
      let module_name = import_split[1]
      
      imported_modules.push(module_name)

      module_name.printLn()
      Line.printLn()

      let x = Line.startswith("import")
      if x{
        
      }
    }
  }

  
  if imported_modules.len() > 0{
    for module_name in imported_modules{
      let relative_path = String{"Lib\\"};
      relative_path += module_name
      relative_path += ".c"

      relative_path.printLn()
    }
  }
  

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}