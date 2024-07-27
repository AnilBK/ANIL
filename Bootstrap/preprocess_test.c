#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String


struct StructInstance{String struct_type, String struct_name,bool is_templated,String templated_data_type,int scope,bool should_be_freed, bool is_pointer_type};

namespace StructInstance

function __init__(p_struct_type : String, p_struct_name : String, p_is_templated: bool,p_templated_data_type : String, p_scope:int)
  this.struct_type = p_struct_type
  this.struct_name = p_struct_name
  this.is_templated = p_is_templated
  this.templated_data_type = p_templated_data_type

  this.scope = p_scope

  this.should_be_freed = true

  this.is_pointer_type = false
endfunction

function should_struct_be_freed() -> bool:
  return this.should_be_freed
endfunction

endnamespace


// function get_destructor_for_struct(p_name : String) -> String:
//   let instanced_struct_names = Vector<StructInstance>{10};
//   for m_struct in instanced_struct_names[::-1]{
//     if m_struct.should_be_freed{
//       let des_code = String{"{destructor_fn_name}(&{struct_name});\n"};
//       return des_code
//     }
//   }
//   let code = String{""};
//   return code
// endfunction
///*///


int main() {

  // clang-format off

  ///*/// 

  let source_file = String{"../examples/01_variables.c"};
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
    let ImportedCodeLines = Vector<String>{50};

    for module_name in imported_modules{
      let relative_path = String{"../Lib/"};
      relative_path += module_name + ".c"

      relative_path.printLn()

      let module_file = String{""};
      let lines = module_file.readlinesFrom(relative_path)
      #lines.print()

      #ImportedCodeLines += lines
      for line in lines{
        ImportedCodeLines.push(line)
      }
    }
  }
  

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}