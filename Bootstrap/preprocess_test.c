#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String

// Insert a string at a given index in another string.
function insert_string(original_string : String, p_index: int, string_to_insert: String) -> String:
  //return original_string[:index] + string_to_insert + original_string[index:]
  let left_part = original_string.substr(0, p_index);
  left_part += string_to_insert + original_string.substr(p_index, original_string.len() - p_index)
  return left_part
endfunction

function escape_quotes(s: String) -> String:
  // Add \ in front of any " in the string.
  // if we find \", then we don't add \ in front of ".
  // result variable is in String readLines function.
  // So, if we use result2 here, the types mix:
  // TODO : Investigate.
  let result2 = "";
  let len = s.len()

  for i in range(0..len){
    let c = s[i]
    if c == "\""{
      if i == 0{
        result2 += "\\"
      }else{
        let i2 : int = i - 1
        if s[i2] != "\\"{
          result2 += "\\"
        }
      }
    }
    result2 += c
  }
  return result2
endfunction

function get_format_specifier(p_type: String) -> String:
  let return_type_str = "d"

  if p_type == "char"{
    return_type_str = "c"
  }else if p_type == "int"{
    return_type_str = "d"
  }else if p_type == "float"{
    return_type_str = "f"
  }else if p_type == "size_t"{
    return_type_str = "llu"
  }
  return return_type_str
endfunction

function get_mangled_fn_name(p_struct_type: String, p_fn_name: String) -> String:
  let s = String{p_struct_type};
  s += p_fn_name
  return s
endfunction

function get_templated_mangled_fn_name(p_struct_type1: String, p_fn_name1: String, p_templated_data_type1: String) -> String:
  let s1 = String{p_struct_type1};
  s1 += "_" + p_templated_data_type1 + p_fn_name1
  return s1
endfunction

struct StructInstance{String struct_type, String struct_name,bool is_templated,String templated_data_type,int scope,bool should_be_freed, bool is_pointer_type};

namespace StructInstance
function __init__(p_struct_type : String, p_struct_name : String, p_is_templated: bool,p_templated_data_type : String, p_scope:int)
  this.struct_type.__init__(p_struct_type)
  this.struct_name.__init__(p_struct_name)
  this.templated_data_type.__init__(p_templated_data_type)
  
  this.is_templated = p_is_templated

  this.scope = p_scope

  this.should_be_freed = true

  this.is_pointer_type = false
endfunction

function is_templated_instance() -> bool:
  return this.is_templated
endfunction

function should_struct_be_freed() -> bool:
  return this.should_be_freed
endfunction

function __del__()
  this.struct_type.__del__()
  this.struct_name.__del__()
  this.templated_data_type.__del__()
endfunction
endnamespace

struct Symbol{String name, String data_type};

namespace Symbol
function __init__(p_name : String, p_data_type : String)
  this.name.__init__(p_name)

  this.data_type.__init__("")
  // don't initialize data_type directly from p_data_type, so we can see,
  // reassign parsing is working as expected.  
  this.data_type = p_data_type
endfunction

function __del__()
  this.name.__del__()
  this.data_type.__del__()
endfunction
endnamespace


// function get_destructor_for_struct(p_name : String) -> String:
//   let instanced_struct_names = Vector<StructInstance>{10};
//   for m_struct in instanced_struct_names[::-1]{
//     if m_struct.should_be_freed{
//       let des_code = "{destructor_fn_name}(&{struct_name});\n"
//       return des_code
//     }
//   }
//   let code = ""
//   return code
// endfunction
///*///


int main() {

  // clang-format off

  ///*/// 

  let source_file = "../examples/01_variables.c"
  #source_file.printLn()

  # output_file_name = source_file.split(".")[0] + "_generated.c"
  #let _split = source_file.split(" ")
  #let output_file_name = _split[0]
  #output_file_name += "_generated.c"
  #output_file_name.print()


  let file = ""
  file.printLn()

  let Lines = file.readlinesFrom(source_file)
  Lines.print()
  
  let imported_modules = Vector<String>{5};

  for line in Lines{
    let Line = line.strip()
    if Line.startswith("import"){
      let import_split = Line.split(" ")
      // let module_name = import_split[1]
      // module_name.printLn()
      
      imported_modules.push(import_split[1])

      Line.printLn()
    }
  }

  
  if imported_modules.len() > 0{
    let ImportedCodeLines = Vector<String>{50};

    for module_name in imported_modules{
      let relative_path = "../Lib/"
      relative_path += module_name + ".c"

      relative_path.printLn()

      let module_file = ""
      let lines = module_file.readlinesFrom(relative_path)
      #lines.print()

      #ImportedCodeLines += lines
      for line in lines{
        ImportedCodeLines.push(line)
      }
    }
  }
  
  let s1 = "Hello World"
  let insert = "virus"
  let index:int = 2

  let new_string = insert_string(s1, index, insert)
  new_string.print()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}