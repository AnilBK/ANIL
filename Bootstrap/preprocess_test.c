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
import ErrorHandler
import Optional

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

function get_templated_mangled_fn_name(p_struct_type: String, p_fn_name: String, p_templated_data_type: String) -> String:
  let s = String{p_struct_type};
  s += "_" + p_templated_data_type + p_fn_name
  return s
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

function get_name() -> String:
  return this.name
endfunction

function get_data_type() -> String:
  return this.data_type
endfunction

function __del__()
  this.name.__del__()
  this.data_type.__del__()
endfunction
endnamespace

struct NameSymbolPair{String name, Symbol symbol};

namespace NameSymbolPair

function __init__(p_name : String, p_symbol : Symbol)
  this.name.__init__(p_name)
  this.symbol.__init__(p_symbol.get_name(), p_symbol.get_data_type())
endfunction

function uncopied_name() -> &String:
  return this.name
endfunction

function get_name() -> String:
  // Duplicate the string and return it.
  let n1 = this.uncopied_name()
  let name = ""
  name = n1
  return name
endfunction

function get_symbol() -> Symbol:
  return this.symbol
endfunction

endnamespace

struct Scope{int scope_id, Vector<NameSymbolPair> symbols};

namespace Scope

function __init__(p_scope_id : int)
  this.scope_id = p_scope_id
  this.symbols.__init__(5)
endfunction

function contains_symbol(name : String) -> bool:
  for s in this.symbols{
    let n = s.get_name()
    if n == name{
      return true
    }
  }
  return false
endfunction

function declare_variable(name : String, p_type : String)
  if this.contains_symbol(name){
    let e = ErrorHandler{};    
    e.RAISE_ERROR("Variable already declared.")
  }

  let symbol = Symbol{name, p_type};
  let name_symbol_pair = NameSymbolPair{name, symbol};

  this.symbols.push(name_symbol_pair)
endfunction

function lookup_variable(name : String) -> Optional<Symbol>:
  let variable = Optional<Symbol>{};

  for s in this.symbols{
    let n = s.get_name()
    if n == name{
      let sym = s.get_symbol()
      let s_name = sym.get_name()
      let s_data_type = sym.get_data_type()

      let Sym1 = Symbol{s_name, s_data_type};
      variable.set_value(Sym1)
      return variable
    }
  }

  return variable
endfunction

function destructor_for_all_variables() -> String:
  let d = ""
  return d
endfunction

function __del__()
  this.symbols.__del__()
endfunction

endnamespace

let random = Random{};

struct ScopeScopeIDPair{int scope_id, Scope scope};

namespace ScopeScopeIDPair
function __init__(p_scope_id : int)
  this.scope_id = p_scope_id
  this.scope.__init__(p_scope_id)
endfunction

function get_scope_id() -> int:
  return this.scope_id
endfunction

function __del__()
  this.scope.__del__()
endfunction

endnamespace

struct SymbolTable{Vector<ScopeScopeIDPair> scopes, Vector<int> scope_stack};

namespace SymbolTable
function __init__()
  this.scopes.__init__(5)
  this.scope_stack.__init__(5)
endfunction

function current_scope() -> int:
  return this.scope_stack[-1]
endfunction

function get_scope_by_id(id : int) -> Scope:
  for s in this.scopes{
    if s.scope_id == id{
      return s.scope
    }
  }

  let e = ErrorHandler{};
  e.RAISE_ERROR("Didnt find scope of provided id.")
endfunction

function new_unique_scope_id() -> int:
  if this.scope_stack.len() == 0{
    return 0
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

  let scope_pair = ScopeScopeIDPair{new_scope_id};
  this.scopes.push(scope_pair)
endfunction

function remove_scope_by_id(scope_id : int)
  let id_to_remove = Vector<int>{1};
  let index : int = 0
  for scope in this.scopes{
    let id = scope.get_scope_id()
    if id == scope_id{
      id_to_remove.push(index)
    }
    index = index + 1
  }

  for idx in id_to_remove[::-1]{
    this.scopes.remove_at(idx)
  }
endfunction

function destructor_code_for_all_remaining_variables() -> String:
  let destructor_code = "";
  while this.scope_stack.len() > 0{
    let exiting_scope_id = this.scope_stack.pop()
    let scope = this.get_scope_by_id(exiting_scope_id)
    let des_code = scope.destructor_for_all_variables()
    if des_code != ""{
      destructor_code += des_code
    }
    this.remove_scope_by_id(exiting_scope_id)
  }
  return destructor_code
endfunction

function exit_scope()
  if this.scope_stack.len() > 0{
    let exiting_scope_id = this.scope_stack.pop()
    let scope = this.get_scope_by_id(exiting_scope_id)
    let destructor_code = scope.destructor_for_all_variables()
    if destructor_code != ""{
      // LinesCache.append(destructor_code)
    }
    this.remove_scope_by_id(exiting_scope_id)
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
  let current_scope_id = this.current_scope()
  let current_scope = this.get_scope_by_id(current_scope_id)

  if current_scope.contains_symbol(name){
    this.print_symbol_table()
    let e = ErrorHandler{};    
    e.RAISE_ERROR("Variable already declared.")
  }

  for scope_id in this.scope_stack{
    let scope = this.get_scope_by_id(scope_id)
    if scope.contains_symbol(name){
      this.print_symbol_table()
      let e = ErrorHandler{};    
      e.RAISE_ERROR("Variable already declared in previous scopes.")
    }
  }

  current_scope.declare_variable(name, p_type)
endfunction

function lookup_variable(name : String) -> Optional<Symbol>:
  let len = this.scope_stack.len()
  for i in range(0..=len-1,-1){
    let scope_id = this.scope_stack[i]
    let scope = this.get_scope_by_id(scope_id)
    let variable = scope.lookup_variable(name)
    if variable.has_value(){
      return variable
    }
  }

  let none = Optional<Symbol>{};
  return none
endfunction
    
function __del__()
  this.scopes.__del__()
  this.scope_stack.__del__()
endfunction
endnamespace

let symbol_table = SymbolTable{};

function get_current_scope() -> int:
  return symbol_table.current_scope()
endfunction

function increment_scope()
  symbol_table.enter_scope()
endfunction

function decrement_scope()
  symbol_table.exit_scope()
endfunction

function REGISTER_VARIABLE(p_var_name : String, p_var_data_type : String)
  symbol_table.declare_variable(p_var_name, p_var_data_type)
endfunction

function get_type_of_variable(p_var_name : String) -> Optional<String>:
  let return_type = Optional<String>{};

  let var = symbol_table.lookup_variable(p_var_name)
  if var.has_value(){
    let symbol = var.get_value()
    return_type.set_value(symbol.get_data_type())  
  }

  return return_type
endfunction

function is_variable_of_type(p_var_name : String, p_type : String) -> bool:
  let var_type = get_type_of_variable(p_var_name)
  if var_type.has_value(){
    let value = var_type.get_value()
    // TODO : value is freed before the boolean comparision.
    return value == p_type
  }
  return false
endfunction

function is_variable(p_var_name : String) -> bool:
  let var_type = get_type_of_variable(p_var_name)
  return var_type.has_value()
endfunction

function is_variable_char_type(p_var_name : String) -> bool:
  let type = "char"
  return is_variable_of_type(p_var_name, type)
endfunction

function is_variable_const_char_ptr(p_var_name : String) -> bool:
  let type = "c_str"
  return is_variable_of_type(p_var_name, type)
endfunction

function is_variable_str_type(p_var_name : String) -> bool:
  let type1 = "str"
  let type2 = "char*"

  if is_variable_of_type(p_var_name, type1){
    if is_variable_of_type(p_var_name, type2){
      return true
    }
  }

  return false
endfunction

function is_variable_boolean_type(p_var_name : String) -> bool:
  let type = "bool"
  return is_variable_of_type(p_var_name, type)
endfunction

function is_variable_int_type(p_var_name : String) -> bool:
  let type = "int"
  return is_variable_of_type(p_var_name, type)
endfunction

function is_variable_size_t_type(p_var_name : String) -> bool:
  let type = "size_t"
  return is_variable_of_type(p_var_name, type)
endfunction

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