#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String
import Set
import Dictionary
import List
import Random
import Dict_int_string

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
  let result = "";
  let len = s.len()

  for i in range(0..len){
    let c = s[i]
    if c == "\""{
      if i == 0{
        result += "\\"
      }else{
        if s[i - 1] != "\\"{
          result += "\\"
        }
      }
    }
    result += c
  }
  return result
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

// Cached Items.
// If any structs have __init__ method, then we register them here.
// This could be stored in StructDefination.
let structs_with_constructors = Set{5};

function has_constructors(p_struct_type : String) -> bool:
  return p_struct_type in structs_with_constructors
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

let random = Random{};

struct SymbolTable{Dict_int_string symbols, Vector<int> scope_stack};

namespace SymbolTable
function __init__()
  this.symbols.__init__()
  this.scope_stack.__init__(5)
endfunction

function current_scope() -> int:
  return this.scope_stack[-1]
endfunction

function new_unique_scope_id() -> int:
  if this.scope_stack.len() == 0{
    // return 0
  }

  let latest_scope = this.current_scope()
  let new_scope : int = latest_scope + 1

  while new_scope in this.scope_stack{
    new_scope = random.randrange(100000)
  }

  return new_scope
endfunction

function enter_scope()
  let new_scope_id = this.new_unique_scope_id()
  this.scope_stack.push(new_scope_id)
  // this.symbols[new_scope_id] = OrderedDict()
  this.symbols.add_key(new_scope_id)
endfunction

function destructor_for_all_variables_in_scope(scope_id : int) -> String:
  // Return the destructor code for all variables in the provided scope
  // And, free(unregister) those variables as well.
  let des_code = "";
  if scope_id in this.symbols{
    // for variable in reversed(this.symbols[scope_id]):
    //     # Call the destructor for the variable
    //     # print(f"Destroying variable '{variable}' in scope {scope_id}")
    //     code = get_destructor_for_struct(variable)
    //     if code != None:
    //         # print(f"~() = {code}")
    //         des_code += code
    //     remove_struct_instance(variable)
    // del this.symbols[scope_id]
  }
  return des_code
endfunction

function exit_scope()
  if this.scope_stack.len() > 0{
    let exiting_scope_id = this.scope_stack.pop()
    let destructor_code = this.destructor_for_all_variables_in_scope(exiting_scope_id)
    if destructor_code != ""{
      // LinesCache.append(destructor_code)
    }
  }

  // No more scopes remaining.
  // Create one so current_scope() wont return list index out of range, because scope_stack is empty.
  if this.scope_stack.len() == 0{
    this.enter_scope()
  }
endfunction

function print_symbol_table()
  print("-------------------Symbol Table------------------")
  print("Unimplemented")
  print("-------------------------------------------------")
endfunction

function declare_variable(name : String, p_type : String)
  let current_scope = this.current_scope()

  // if name in this.symbols[current_scope]:
  //     this.print_symbol_table()
  //     RAISE_ERROR(f"Variable '{name}' already declared in this scope.")

  for scope in this.scope_stack{
  //     if name in this.symbols[scope]:
  //         this.print_symbol_table()
  //         RAISE_ERROR(
  //             f"Variable '{name}' already declared in previous scope {scope}."
  //         )
  }

  // this.symbols[current_scope][name] = Symbol(name, p_type)
endfunction

function find_variable(name : String)
  // for scope in this.scope_stack[::-1]{
  //     if name in this.symbols[scope]:
  //         return this.symbols[scope][name]
  // }
  // return None
endfunction

function destructor_code_for_all_remaining_variables() -> String:
  let destructor_code = "";
  while true{
    if this.scope_stack.len() > 0{
      let exiting_scope_id = this.scope_stack.pop()
      let des_code = this.destructor_for_all_variables_in_scope(exiting_scope_id)
      if des_code != ""{
        destructor_code += des_code
      }
    }else{
      break;
    }
  }
  return destructor_code
endfunction
    
function __del__()
  this.scope_stack.__del__()
endfunction
endnamespace

let symbol_table = SymbolTable{};


///*///

int main() {

  // clang-format off

  ///*/// main()

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