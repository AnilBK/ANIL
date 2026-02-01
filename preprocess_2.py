import os
import re
import random
import argparse
import copy
from collections import OrderedDict

from enum import Enum
from typing import Callable, Dict, Optional

from contextlib import contextmanager

import lexer
import Parser
from ErrorHandler import ErrorHandler
from input_variables_gui_manager import InputVariablesGUI
from JSXLikeParser import UIAttribute, UIElement, UIElementTree 

from CompilerContext import CompilerContext
from CCodeGenerator import CCodeGenerator

# from ASTNodes import *

from abc import ABC, abstractmethod


class ASTNode(ABC):
    @abstractmethod
    def codegen(self):
        pass


class StatementNode(ASTNode):
    pass


class ExpressionNode(ASTNode):
    pass


class VariableDeclarationNode(StatementNode):
    def __init__(self, var_name, var_type, initializer: ExpressionNode):
        self.var_name = var_name
        self.var_type = var_type
        self.initializer = initializer

    def codegen(self):
        # Generate the C code: "int x = 10;"
        code = f"{self.var_type} {self.var_name}"
        if self.initializer:
            code += f" = {self.initializer.codegen()}"
        code += ";"
        return code


class LiteralNode(ExpressionNode):
    def __init__(self, value, value_type):
        self.value = value
        self.value_type = value_type

    def codegen(self) -> str:
        if self.value_type == "char":
            return f"'{self.value}'"
        if self.value_type == "str":
            return f'"{self.value}"'
        return (
            str(self.value).lower() if isinstance(self.value, bool) else str(self.value)
        )

class PrintNode(StatementNode):
    def __init__(self, raw_template_string: str):
        self.raw_template_string = raw_template_string

    def codegen(self) -> str:
        braces_open = False
        str_text = ""
        extracted_var_name_list = []
        extracted_var_name = ""
        
        for char in self.raw_template_string:
            if char == "{":
                braces_open = True
                extracted_var_name = ""
            elif char == "}":
                braces_open = False
                
                # Logic to determine C format specifier (%d, %s, etc.)
                format_specifier = "d" 
                return_type = get_type_of_variable(extracted_var_name)
                
                if return_type is not None:
                    format_specifier = get_format_specifier(return_type)
                
                str_text += f"%{format_specifier}"

                struct_type = data_type_with_struct_stripped(return_type or "")
                struct_def = get_struct_defination_of_type(struct_type)
                instanced_struct_info = get_instanced_struct(extracted_var_name)

                if struct_def:
                    if struct_def.has_member_fn("__str__"):
                        mangled_str_fn_name = get_mangled_fn_name(struct_type, "__str__")
                        # If it's a pointer (like 'this'), pass directly, else pass address.
                        prefix = "" if (instanced_struct_info and instanced_struct_info.is_pointer_type) else "&"
                        extracted_var_name_list.append(f"{mangled_str_fn_name}({prefix}{extracted_var_name})")
                    else:
                        RAISE_ERROR(f"Can't print object '{extracted_var_name}' of type '{return_type}' as it does not have a __str__ method.")
                else:
                    extracted_var_name_list.append(extracted_var_name)
                    
            elif braces_open:
                extracted_var_name += char
            else:
                str_text += char

        # Construct the final C printf string
        args_code = ""
        if len(extracted_var_name_list) > 0:
            args_code = "," + ",".join(extracted_var_name_list)
            
        return f'printf("{str_text}\\n"{args_code});\n'


# We don't typically pass filenames through command line, this is mostly for batch compile operations.
filename_parser = argparse.ArgumentParser()
filename_parser.add_argument("--filename", help="Name of source file to be compiled.")
filename_parser.add_argument("--no-clang-format", action="store_true", help="Disable code formatting with clang-format.")

args = filename_parser.parse_args()

# Basics.
source_file = "examples\\00_Hello_World.c"
# source_file = "examples\\00_Hello_World.anil"
# source_file = "examples\\01_variables.c"
# source_file = "examples\\02_List.c"
# source_file = "examples\\03_Dictionary.c"
# source_file = "examples\\04_Classes.c"
# source_file = "examples\\04_Classes.anil"
# source_file = "examples\\04_b_Classes.c"
# source_file = "examples\\05_Strings.c"
# source_file = "examples\\06_Vector.c"
# source_file = "examples\\07_Set.c"
# source_file = "examples\\08_Optional.c"
# source_file = "examples\\08_Optional.anil"
# source_file = "examples\\09_Functions.c"
# source_file = "examples\\10_FileIO.c"
# source_file = "examples\\10_FileIO.anil"
# source_file = "examples\\11_Threads_And_Channels.anil"

# Compile Time related.
# source_file = "examples\\Annotations.c"
# source_file = "examples\\Compile_Time_Constants.c"
# source_file = "examples\\Macro_With_Variadic_Arguments.c"
# source_file = "examples\\Reflection.c"

# Misc.
# source_file = "examples\\Enumerate.c"
# source_file = "examples\\decorators_inside_fn_body.c"
# source_file = "examples\\Unique_ptr_example.c"
# source_file = "examples\\FunctionPointer.c"

# Windows Specific.
# source_file = "examples\\UI\\TodoAppBasic.c"
# source_file = "examples\\UI\\TodoAppJSX.c"
# source_file = "examples\\UI\\TodoAppWebServer.c"
# source_file = "examples\\UI\\IDE\\IDE.anil"
# source_file = "examples\\Variables_GUI_Input_Win.c"
# source_file = "examples\\WebServer.c"

# Compiler Stress Tests.
# source_file = "examples\\TestSuites\\Expression_parse_tests.c"
# source_file = "examples\\TestSuites\\Return_value_tests.c"
# source_file = "examples\\TestSuites\\slicing.c"

# Graphics using raylib.
# source_file = "examples\\raylib\\raylib_example.c"
# source_file = "examples\\raylib\\snake.c"

# Compiler Bootstrap.
# source_file = "Bootstrap\\lexer_test.c"
# source_file = "Bootstrap\\Parser.c"
# source_file = "Bootstrap\\preprocess_test.c"
# source_file = "Bootstrap\\preprocess_test.anil"

if args.filename:
    source_file = args.filename

base_name, ext = os.path.splitext(os.path.basename(source_file))
output_file_name = os.path.join(os.path.dirname(source_file), base_name + "_generated" + ext)

Lines = []
with open(source_file, "r") as file:
    Lines = file.readlines()

is_anil_file = ext == ".anil"

if is_anil_file:
    # Modify contents of .anil file to like a .c file, because struct definations are emitted to "// STRUCT_DEFINATIONS //" line.
    # We cant just directly call the function which writes the global struct definations, as while importing c library
    # files we need to copy those global functions in those libraries serially as well.
    # If we emit the global struct definations directly, then we miss the order of the global c functions defined 
    # in those libraries.
    header = [
        "\n",
        "// STRUCT_DEFINATIONS // \n",
        "///*///",
        "\n",
    ]

    footer = [
        "\n",
        "// DESTRUCTOR_CODE //",
        "\n",
        "///*///",
        "\n",
    ]

    Lines = header + Lines + footer

    output_file_name = output_file_name.replace(".anil", "_anil.c")

imported_modules = []

for Line in Lines:
    Line = Line.strip()
    if Line.startswith("import "):
        # import String
        # import Vector
        # import UniquePtr
        _, module_name = Line.split()
        imported_modules.append(module_name)

if len(imported_modules) > 0:
    ImportedCodeLines = []

    script_dir = os.path.dirname(os.path.abspath(__file__))
    source_dir = os.path.dirname(os.path.abspath(source_file)) 

    for module_name in imported_modules:
        lib_dir = os.path.join(script_dir, "Lib")
        module_file_path = os.path.join(lib_dir, module_name + ".c")

        if not os.path.exists(module_file_path):
            module_file_path = os.path.join(source_dir, module_name + ".c")

        if os.path.exists(module_file_path):
            with open(module_file_path, "r") as module_file:
                ImportedCodeLines.extend(module_file.readlines())
        else:
            raise FileNotFoundError(f"Module {module_name}.c not found in script's Lib folder ({lib_dir}) or source file directory ({source_dir}).")
    
    Lines = ImportedCodeLines + Lines


ctx = CompilerContext()
code_generator = CCodeGenerator(compiler_context = ctx)

def emit(code):
    ctx.emit(code)

def _LinesCacheAddNewTracker():
    ctx.push_generated_lines_checkpoint()

def _CommitLinesCache():
    # No need to do anything with the generated lines,
    # just remove the recent tracker.
    ctx.commit_generated_lines_checkpoint()

def _RevertLinesCache():
    ctx.revert_generated_lines_checkpoint()


class RollbackTemporaryGeneratedCodes(Exception):
    pass

class VariablesCheckpoint:
    # TODO: Just track what was added during speculative parsing.
    def __init__(self):
        # Save global speculative counters and variables.
        self.temp_string_object_variable_count = temp_string_object_variable_count
        self.temp_arr_length_variable_count = temp_arr_length_variable_count
        self.temp_c_str_iterator_variable_count = temp_c_str_iterator_variable_count
        self.temp_arr_search_variable_count = temp_arr_search_variable_count
        self.temp_char_promoted_to_string_variable_count = temp_char_promoted_to_string_variable_count
        self.for_loop_depth = for_loop_depth

        # These maybe mutated, so we perform deepcopy.
        self.instanced_struct_names = copy.deepcopy(instanced_struct_names)
        self.symbol_table = copy.deepcopy(symbol_table)

    def restore(self):
        global temp_string_object_variable_count,temp_arr_length_variable_count,temp_c_str_iterator_variable_count,temp_arr_search_variable_count,temp_char_promoted_to_string_variable_count,for_loop_depth,instanced_struct_names,symbol_table

        # Restore all variables to their checkpointed values.
        temp_string_object_variable_count = self.temp_string_object_variable_count
        temp_arr_length_variable_count = self.temp_arr_length_variable_count
        temp_c_str_iterator_variable_count = self.temp_c_str_iterator_variable_count
        temp_arr_search_variable_count = self.temp_arr_search_variable_count
        temp_char_promoted_to_string_variable_count = self.temp_char_promoted_to_string_variable_count
        for_loop_depth = self.for_loop_depth

        instanced_struct_names = self.instanced_struct_names
        symbol_table = self.symbol_table

@contextmanager
def SpeculativeFunctionParse():
    # TODO: Maybe it should handle checkpointing as well.
    _LinesCacheAddNewTracker()
    variables_checkpoint = VariablesCheckpoint()

    try:
        yield
        _CommitLinesCache()
    except RollbackTemporaryGeneratedCodes:
        _RevertLinesCache()
        variables_checkpoint.restore()


struct_definations = OrderedDict()
instanced_struct_names = []

IncludeLines = [] # ["#include<stdio.h>", ...]

GlobalStructInitCode = ""

# Store all the generated function declarations. They will be written at the start of the file,
# so that the functions can be called, at any place in the file without any definition error.
# For e.g: It stores:
#           void f();
#           int g(int a);
#           struct A h(int b, ...);
GlobalGeneratedFunctionDeclarations = ""

# If classes don't provide their custom destructors(__del__()), then we recursively search if any of its members
# has destructors. If any of it's members has destructors, then we emit a custom destructor. The emmited
# destructor code is stored here.
GlobalGeneratedStructDestructors = ""

# Same as above, but for struct definations.
# For e.g: It stores:
#           struct Vector { ... };
#           struct String { ... };
GlobalGeneratedStructDefinations = ""

# Constexpr dictionary lookups which couldnt be resolved at compile time, say we provided a variable name
# to index constexpr dict, then we should resolve it at run time.
# The functions to resolve such looksups are stored here.
GlobalGeneratedConstexprLookupFnDefinations = ""
# The function names are stored here, just to check if we need to emit the function call.
constexpr_lookup_functions_generated = []


# Write __init__ code of structs defined in global scope.
# This code will be written at the start of main function marked by ///*/// main()
global_variables_initialization_code = []
main_fn_found = False

is_inside_form = False
gui_manager = InputVariablesGUI()
JSXlike_element_tree = UIElementTree()

is_inside_GUI_code = False

# When a fn is declared, we need to track all the scopes inside it,
# such that for return statements we can generate destructor code for all the required variables in those scopes.
tracking_scopes_for_current_fn = False
tracked_scopes_for_current_fn = []
# All the function parameters are registerd here, such that they may be freed when the function ends.
registered_function_parameters = []

# Store T and what T is instantiated with.
# For e.g: For Object of A<T> instantiated as A<int>, we store {"T": "int"}
# This is used to resolve templated types in function bodies and within the entire namespace.
templated_type_mappings = {}

def resolve_templated_type(p_templated_type: str) -> str:
    if p_templated_type in templated_type_mappings:
        return templated_type_mappings[p_templated_type]
    else:
        return p_templated_type


# UTILS BEGIN


# Insert a string at a given index in another string.
def insert_string(original_string, index, string_to_insert) -> str:
    return original_string[:index] + string_to_insert + original_string[index:]


def insert_intermediate_lines(index, p_array : list):
    global Lines
    Lines = Lines[:index] + p_array + Lines[index:]


def escape_quotes(s):
    # Add \ in front of any " in the string.
    # if we find \", then we don't add \ in front of ".
    result = ""
    i = 0
    while i < len(s):
        if s[i] == '"':
            if i == 0 or s[i-1] != '\\':
                result += '\\'
        result += s[i]
        i += 1
    return result


def get_format_specifier(p_type: str) -> str:
    db = {"char": "c", "int": "d", "float": "f", "size_t": "llu", "String": "s"}

    is_array_type = p_type[0] == "[" and p_type[-1] == "]"
    if is_array_type:
        p_type = p_type[1:-1]

    if p_type in db.keys():
        return db[p_type]
    else:
        # Custom classes can overload __str__ function.
        struct_type = data_type_with_struct_stripped(p_type)
        struct_def = get_struct_defination_of_type(struct_type)
        if struct_def and struct_def.has_member_fn("__str__"):
            return "s"

        return "d"


def get_mangled_templated_class_type(p_base_class_type: str, p_templated_type: str) -> str:
    '''Mangle an instance of A<T> as A_T.'''
    return p_base_class_type + "_" + p_templated_type


def get_mangled_fn_name(p_struct_type: str, p_fn_name: str) -> str:
    return p_struct_type + p_fn_name


def get_templated_mangled_fn_name(
    p_struct_type: str, p_fn_name: str, p_templated_data_type: str
) -> str:
    return p_struct_type + "_" + p_templated_data_type + p_fn_name


def get_overloaded_mangled_fn_name(p_struct_type: str, p_fn_name: str, p_parameters: list) -> str:
    # append(int) -> ListappendOVDint
    # append(char*) -> ListappendOVDstr
    function_name = get_mangled_fn_name(p_struct_type, p_fn_name)
    function_name += "OVD"
    for param in p_parameters:
        data_type = param
        if data_type == "char*":
            function_name += "str"
        else:
            #"struct String", we dont want those spaces in final function name.
            data_type_no_spaces = data_type.replace(" ", "")
            function_name += data_type_no_spaces
    return function_name
# UTILS END


class Symbol:
    def __init__(self, name, data_type):
        self.name = name
        self.data_type = data_type

class Scope:
    def __init__(self, scope_id):
        self.scope_id = scope_id
        self.symbols = OrderedDict()

    def declare_variable(self, name, p_type):
        if name in self.symbols:
            RAISE_ERROR(f"Variable '{name}' is already declared in this scope.")
        self.symbols[name] = Symbol(name, p_type)

    def lookup_variable(self, name):
        return self.symbols.get(name, None)
    
    def get_destructor_code_for_all_variables(self):
        destructor_code = ""
        for symbol in reversed(self.symbols):
            code = get_destructor_for_struct(symbol)
            if code is not None:
                destructor_code += code
        return destructor_code

    def remove_all_variables(self):
        for symbol in self.symbols:
            remove_struct_instance(symbol)
        self.symbols.clear()

    def get_destructor_for_all_variables_and_remove(self):
        destructor_code = self.get_destructor_code_for_all_variables()
        self.remove_all_variables()
        return destructor_code

class SymbolTable:
    def __init__(self):
        self.scopes = {}
        self.scope_stack = []

    def current_scope(self):
        return self.scope_stack[-1]

    def get_scope_by_id(self, id):
        return self.scopes[id]

    def new_unique_scope_id(self):
        if len(self.scope_stack) == 0:
            # We haven't created any scopes.
            # So, the first scope created is 0.
            return 0

        latest_scope = self.current_scope()
        new_scope = latest_scope + 1

        while new_scope in self.scope_stack:
            new_scope = random.randrange(100000)

        return new_scope

    def enter_scope(self):
        new_scope_id = self.new_unique_scope_id()
        self.scope_stack.append(new_scope_id)
        self.scopes[new_scope_id] = Scope(new_scope_id)

        if tracking_scopes_for_current_fn:
            if new_scope_id not in tracked_scopes_for_current_fn:
                tracked_scopes_for_current_fn.append(new_scope_id)
            # For for loops, we increment scope and the for loop generates ANIL code which increments scope.
            # Apparently new_scope_id is added again,
            # such that the destructor code is generated twice. So we make this check(Shouldn't be needed optimally).

    def exit_scope(self):
        if self.scope_stack:
            exiting_scope_id = self.scope_stack.pop()

            if exiting_scope_id in scopes_with_return_stmnt:
                # We have found return statement in this scope.
                # The return statement has already written the destructor code for all the variables in this scope.
                pass
            else:
                destructor_code = self.get_scope_by_id(exiting_scope_id).get_destructor_code_for_all_variables()
                if destructor_code != "":
                    emit(destructor_code)
            del self.scopes[exiting_scope_id]

        # No more scopes remaining.
        # Create one so current_scope() wont return list index out of range, because scope_stack is empty.
        if not self.scope_stack:
            self.enter_scope()

    def print_symbol_table(self):
        print("-------------------Symbol Table------------------")
        for scope_id in self.scope_stack:
            scope = self.get_scope_by_id(scope_id)
            for name, symbol in scope.symbols.items():
                print(f"{scope_id} {name} {symbol.data_type}")
        print("-------------------------------------------------")

    def declare_variable(self, name, p_type):
        current_scope = self.get_scope_by_id(self.current_scope())

        if name in current_scope.symbols:
            self.print_symbol_table()
            RAISE_ERROR(f"Variable '{name}' already declared in this scope.")

        for scope_id in self.scope_stack:
            if name in self.get_scope_by_id(scope_id).symbols:
                self.print_symbol_table()
                RAISE_ERROR(f"Variable '{name}' already declared in previous scope {scope_id}.")

        current_scope.declare_variable(name, p_type)

    def lookup_variable(self, name):
        for scope_id in reversed(self.scope_stack):
            variable = self.get_scope_by_id(scope_id).lookup_variable(name)
            if variable:
                return variable
        return None

    def destructor_code_for_all_remaining_variables(self):
        destructor_code = ""
        while self.scope_stack:
            exiting_scope_id = self.scope_stack.pop()
            des_code = self.get_scope_by_id(exiting_scope_id).get_destructor_for_all_variables_and_remove()
            if des_code != "":
                destructor_code += des_code
            del self.scopes[exiting_scope_id]
        return destructor_code


symbol_table = SymbolTable()
symbol_table.enter_scope()  # Create a new fresh scope.

# symbol_table.enter_scope()  # Enter Scope 1
# symbol_table.declare_variable("a", "str")
# symbol_table.declare_variable("b", "str")
# symbol_table.declare_variable("c", "str")

# symbol_table.enter_scope()  # Enter Scope 2
# symbol_table.declare_variable("d", "str")
# symbol_table.exit_scope()  # Exit Scope 2

# symbol_table.enter_scope()  # Enter Scope 3
# symbol_table.declare_variable("e", "str")
# symbol_table.exit_scope()  # Exit Scope 3

# symbol_table.exit_scope()  # Exit Scope 1


def get_current_scope():
    return symbol_table.current_scope()


def increment_scope():
    symbol_table.enter_scope()


def decrement_scope():
    symbol_table.exit_scope()


def REGISTER_VARIABLE(p_var_name: str, p_var_data_type: str) -> None:
    symbol_table.declare_variable(p_var_name, p_var_data_type)
    if is_inside_form:
        gui_manager.process_variable(p_var_name, p_var_data_type)


def get_type_of_variable(p_var_name):
    var = symbol_table.lookup_variable(p_var_name)
    if var == None:
        return None
    else:
        return var.data_type


def is_variable_of_type(p_var_name, p_type):
    var_type = get_type_of_variable(p_var_name)
    if var_type == None:
        return False

    return var_type == p_type


def is_variable(p_var_name) -> bool:
    return get_type_of_variable(p_var_name) != None


def is_variable_char_type(p_var_name):
    return is_variable_of_type(p_var_name, "char")


def is_variable_const_char_ptr(p_var_name):
    return is_variable_of_type(p_var_name, "c_str")


def is_variable_string_class(p_var_name):
    struct_info = get_instanced_struct(p_var_name)
    if struct_info != None:
        return struct_info.struct_type == "String"
    return False


def is_variable_str_type(p_var_name):
    return is_variable_of_type(p_var_name, "str") or is_variable_of_type(
        p_var_name, "char*"
    )


def is_variable_boolean_type(p_var_name):
    return is_variable_of_type(p_var_name, "bool")


def is_variable_int_type(p_var_name):
    return is_variable_of_type(p_var_name, "int")


def is_variable_size_t_type(p_var_name):
    return is_variable_of_type(p_var_name, "size_t")


def is_variable_array_type(p_var_name):
    var_type = get_type_of_variable(p_var_name)
    if var_type == None:
        return False

    is_array_type = var_type[0] == "[" and var_type[-1] == "]"
    return is_array_type


def is_data_type_struct_object(p_data_type):
    struct_info = get_struct_defination_of_type(p_data_type)
    return struct_info != None


def format_struct_data_type(p_data_type):
    if is_data_type_struct_object(p_data_type):
        return f"struct {p_data_type}"
    return p_data_type


def data_type_with_struct_stripped(p_data_type):
    '''"struct Vector" -> "Vector"'''
    if p_data_type.startswith("struct "):
        return p_data_type[len("struct "):]
    return p_data_type


class NestingLevel(Enum):
    FOR_LOOP = 0
    IF_STATEMENT = 1
    ELSE_STATEMENT = 2
    WHILE_STATEMENT = 3


nesting_levels = []

# From where did the function body write started.
# We have to insert function hooks later here.
c_function_body_start_pos = 0
currently_reading_fn_name = ""
currently_reading_fn_name_unmangled = ""
currently_reading_return_type = ""

currently_reading_fn_body = ""
currently_reading_fn_parent_struct = ""
currently_reading_parameters = []
should_write_fn_body = True

return_encountered_in_fn = False
scopes_with_return_stmnt = []

# User Defined function properties.
class_fn_defination = {
    "class_name": "",
    "function_name": "",
    "start_index": -1,
    "end_index": -1,
    "function_destination": "global"#"global"/"class",wether function is defined as global function, or member function of a struct.
}

#wether we are inside function scope or not.
is_inside_user_defined_function = False

HOOKS_hook_fn_name = ""
HOOKS_target_fn = ""


class MemberDataType:
    def __init__(self, p_data_type, p_member, p_is_generic) -> None:
        self.data_type = p_data_type
        self.member = p_member
        self.is_generic = p_is_generic


class MemberFunction:
    def __init__(
        self, p_fn_name: str, p_fn_arguments: list, p_return_type: str
    ) -> None:
        self.fn_name = p_fn_name
        self.fn_arguments = p_fn_arguments
        self.fn_body = ""
        self.return_type = p_return_type

        # This means the value(struct) which is assigned the return value of this function isn't freed by the destructor at the end of the created variable scope.
        self.is_return_type_ref_type = False

        self.is_overloaded_function = False
        self.overload_for_template_type = ""
        
        self.is_overloaded = False

        self.is_static_fn = False

    def is_static(self):
        return self.is_static_fn

    def is_constructor(self):
        return self.fn_name == "__init__"

    def is_destructor(self):
        return self.fn_name == "__del__"

    def print_info(self):
        print("--------------------------------------------------")
        print(f"Function Name : {self.fn_name}")
        print(f"Function Arguments : {self.fn_arguments}")
        print(f"Function Body : \n{self.fn_body}")
        print(f"Return Type: \n{self.return_type}")
        print("--------------------------------------------------")

# Functions which aren't defined inside structs.
GlobalFunctions = []

def is_global_function(p_fn_name: str) -> bool:
    return any(fn.fn_name == p_fn_name for fn in GlobalFunctions)

def get_global_function_by_name(p_fn_name: str):
    for fn in GlobalFunctions:
        if fn.fn_name == p_fn_name:
            return fn
    return None

def is_class_name(p_token: str) -> bool:
    return get_struct_defination_of_type(p_token) is not None

def get_class_static_function(class_name: str, p_fn_name: str):
    struct_def = get_struct_defination_of_type(class_name)
    if struct_def is None:
        return None

    for fn in struct_def.member_functions:
        if fn.fn_name == p_fn_name:
            if not fn.is_static():
                RAISE_ERROR(f"Function {p_fn_name} of class {class_name} is not static.")
            return fn
    return None

class Struct:
    def __init__(self, p_struct_name: str, p_members: list) -> None:
        self.name = p_struct_name
        self.members = p_members
        self.member_functions = []
        self.macro_definations = []
        self.is_class_templated = False
        self.templated_data_type = ""
        self.template_defination_variable = ""

        # For templated structs, we don't immediately parse functions.
        # Instead we just read the function bodies and store them in 'unparsed_functions'.
        # Later when we instantiate the template, we resolve the templated types in the function declaration,
        # emit the code, which is then parsed.
        self.unparsed_functions = []

        # When we instantiate a template, we copy 'unparsed_functions' from base class to ourself.
        # We then set this flag to True, to indicate that we have to emit the body of the function,
        # which is then parsed.
        # However, the base template struct will have this flag set to False, because we don't want to
        # generate function body for the base template struct as it doesn't have any templated types.
        # The base struct becomes like an abstract class.
        self.unparsed_functions_should_be_parsed = False

        self.unparsed_functions_emitted = False

    def is_templated(self) -> bool:
        return self.is_class_templated

    def has_constructor(self) -> bool:
        return any(fn.is_constructor() for fn in self.member_functions)

    def has_destructor(self) -> bool:
        return any(fn.is_destructor() for fn in self.member_functions)

    def fill_with_destructors_recursive(self):
        # Recursively figure out if any of its children has destructors.
        # If true, then we add destructor to their parent as well, if the parent doesn't have a destructor.
        for member in self.members:
            member_type = member.data_type

            member_struct_def = get_struct_defination_of_type(member_type)
            if member_struct_def is None:
                # Skip for non struct members.
                continue

            if member_struct_def.has_destructor():
                continue

            member_struct_def.fill_with_destructors_recursive()

        # All it's children have destructors filled if necessary.
        # Now, if we don't have a destructor, then we need to fill the destructor in ourself.
        if not self.has_destructor():
            atleast_one_child_has_destructor = False

            for member in self.members:
                member_type = member.data_type

                member_struct_def = get_struct_defination_of_type(member_type)
                if member_struct_def is None:
                    # Skip for non struct members.
                    continue

                if member_struct_def.has_destructor():
                    atleast_one_child_has_destructor = True
                    break

            if atleast_one_child_has_destructor:
                generate_destructor_for_struct(self.name)

    def has_member_fn(self, p_fn_name) -> bool:
        return any(fn.fn_name == p_fn_name for fn in self.member_functions)

    def has_macro(self, p_def_name) -> bool:
        return any(macro_def.name == p_def_name for macro_def in self.macro_definations)

    def get_return_type_of_fn(self, p_fn_name) -> str:
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                is_templated = self.is_templated() and (self.templated_data_type != "")

                if is_templated:
                    if fn.return_type == self.template_defination_variable:
                        return self.templated_data_type
                return fn.return_type

    def get_function_arguments(self, p_fn_name):
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                return fn.fn_arguments
        return None
        # RAISE_ERROR(f"Function {p_fn_name} not found.")

    #Overloaded function Utilities.
    def function_is_overloaded(self, p_fn_name):
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                return fn.is_overloaded
        return False


    def set_functions_overloaded(self, p_fn_name : str):
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                fn.is_overloaded = True
            

    def get_function_arguments_with_types(self, p_fn_name, provided_parameter_types:list):
        # To get arguments of overloaded function.
        possible_args = []
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                args = fn.fn_arguments
                args_list = [a.data_type for a in args]
                if args_list == provided_parameter_types:
                    return args
                possible_args.append(args_list)
        error_msg = f"Didn't find overloaded function({p_fn_name}) of provided types {provided_parameter_types}."
        error_msg += f"Possible argument types for the overloaded function are {possible_args}."
        RAISE_ERROR(f"{error_msg}")


    def get_return_type_of_overloaded_fn(self, p_fn_name, provided_parameter_types:list):
        # To get arguments of overloaded function.
        possible_args = []
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                args = fn.fn_arguments
                args_list = [a.data_type for a in args]
                if args_list == provided_parameter_types:
                    return fn.return_type
                possible_args.append(args_list)
        m_types = [a.data_type for a in provided_parameter_types]
        error_msg = f"Didn't find overloaded function({p_fn_name}) of provided types {m_types}."
        error_msg += f"Possible argument types for the overloaded functions are {possible_args}."
        RAISE_ERROR(f"{error_msg}")


    def is_return_type_of_fn_ref_type(self, p_fn_name, p_custom_overload_type = "") -> str:
        if p_custom_overload_type != "":
            for fn in self.member_functions:
                if fn.fn_name == p_fn_name:
                    if fn.overload_for_template_type == p_custom_overload_type:
                        return fn.is_return_type_ref_type

        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                return fn.is_return_type_ref_type

        RAISE_ERROR(f"Function {p_fn_name} not found.")

    def is_return_type_of_overloaded_fn_ref_type(self, p_fn_name, provided_parameter_types:list):
        possible_args = []
        for fn in self.member_functions:
            if fn.fn_name == p_fn_name:
                args = fn.fn_arguments
                args_list = [a.data_type for a in args]
                if args_list == provided_parameter_types:
                    return fn.is_return_type_ref_type
                possible_args.append(args_list)
        m_types = [a.data_type for a in provided_parameter_types]
        error_msg = f"Didn't find overloaded function({p_fn_name}) of provided types {m_types}."
        error_msg += f"Possible argument types for the overloaded functions are {possible_args}."
        RAISE_ERROR(f"{error_msg}")


    def get_type_of_member(self, p_member_name) -> Optional[str]:
        for struct_member in self.members:
            if struct_member.member == p_member_name:
                return struct_member.data_type
        return None


    def has_data_member(self, p_member_name) -> bool:
        return any(struct_member.member == p_member_name for struct_member in self.members)


    def print_member_fn_info(self):
        for fn in self.member_functions:
            fn.print_info()

    def get_struct_initialization_code(self) -> str:
        struct_code = f"struct {self.name} {{\n"
        for struct_member in self.members:
            type = struct_member.data_type
            mem = struct_member.member
            is_generic = struct_member.is_generic

            type_without_pointers = type.replace("*", "")
            # This get_struct_initialization_code() maybe called before itself has been registered
            # to global struct definations. In that case 'is_data_type_struct_object()' returns false.
            # So, we check if the type is same as this struct name, in that case it is a struct.
            if type_without_pointers == self.name or is_data_type_struct_object(type_without_pointers):
                struct_code += f"struct {type} {mem};\n"
            else:
                struct_code += f"{type} {mem};\n"
        struct_code += f"}};\n\n"
        return struct_code

    def ensure_has_function(self, p_fn_name: str, p_instance_name=""):
        if not self.has_member_fn(p_fn_name):
            print("The Struct has following functions : ")
            self.print_member_fn_info()
            if p_instance_name != "":
                RAISE_ERROR(f"{self.name}({p_instance_name}) doesn't have the function name : {p_fn_name}")
            else:
                RAISE_ERROR(f"{self.name} doesn't have the function name : {p_fn_name}")

    # Template Instantiation Related Code.     
    
    # Note : To be called by instantiate_template() only.
    def remove_unwanted_template_fn_overloads(self, p_template_type: str) -> None:
        indices_to_remove = []

        for index, fn in enumerate(self.member_functions):
            if fn.is_overloaded_function:
                use_this_overload = False

                if fn.overload_for_template_type == p_template_type:
                    # This current function is the suitable overload.
                    use_this_overload = True
                else:
                    # Check if any other function with the same name is a better overload.
                    has_suitable_overload = any(m_fn.fn_name == fn.fn_name and m_fn.overload_for_template_type == p_template_type for m_fn in self.member_functions)

                    if not has_suitable_overload:
                        # If no other suitable overload is found, check for the base overload.
                        if fn.overload_for_template_type == "#BASE#":
                            use_this_overload = True

                # If this function is not the suitable overload, mark it for removal.
                if not use_this_overload:
                    indices_to_remove.append(index)

        for index in reversed(indices_to_remove):
            del self.member_functions[index]

    def remove_unwanted_template_fn_overloads_and_return(self, p_template_type: str) -> None:
        #Same fn as above but returns the indices of the functions removed.

        indices_to_remove = []

        for index, fn in enumerate(self.member_functions):
            if fn.is_overloaded_function:
                use_this_overload = False

                if fn.overload_for_template_type == p_template_type:
                    # This current function is the suitable overload.
                    use_this_overload = True
                else:
                    # Check if any other function with the same name is a better overload.
                    has_suitable_overload = any(m_fn.fn_name == fn.fn_name and m_fn.overload_for_template_type == p_template_type for m_fn in self.member_functions)

                    if not has_suitable_overload:
                        # If no other suitable overload is found, check for the base overload.
                        if fn.overload_for_template_type == "#BASE#":
                            use_this_overload = True

                # If this function is not the suitable overload, mark it for removal.
                if not use_this_overload:
                    indices_to_remove.append(index)

        for index in reversed(indices_to_remove):
            del self.member_functions[index]

        return indices_to_remove

    def substitute_template_for_member_types(self, p_templated_type: str) -> None:
        for struct_member in self.members:
            type = struct_member.data_type
            is_generic = struct_member.is_generic

            # Replace the unresolved template types with actual instantiation type.
            if "<" in type:
                # type : 'struct DictObject<T>**'
                #                           ^ template_type
                #                ^^^^^^^^^^   base_type

                # Works for 1 level of templating like A<T>.

                ST_index = type.find("<")
                GT_index = type.find(">", ST_index)

                template_type = type[ST_index+ 1 : GT_index]
                type = type.replace(template_type, p_templated_type)

                # get everything left to <
                base_type = type[:ST_index]
                # 'struct DictObject'

                #strip "struct " from base_type
                base_type = data_type_with_struct_stripped(base_type)
                # 'DictObject'

                instantiate_template(base_type, p_templated_type)

                #struct Vector<T>
                #get everything left to <
                mangled_resolved_type_ptrless = type[:type.find("<")]
                mangled_resolved_type_ptrless += f"_{p_templated_type}"

                # Add all the required number of pointers.
                mangled_resolved_type_ptrless += "*" * type.count("*")

                type = mangled_resolved_type_ptrless
                # now type is 'struct DictObject_int**'

            if is_generic:
                if "*" in type:
                    actual_type, star = type.split("*")
                    type = p_templated_type + "*"
                else:
                    type = p_templated_type

            if type.startswith("Self"):
                type = type.replace("Self", f"struct {self.name}", 1)

            type = data_type_with_struct_stripped(type)

            struct_member.data_type = type



    def resolve_tags(self, p_templated_type: str) -> None:
        # if we want to use template type in fn body, we use following syntax.
        # @TEMPLATED_DATA_TYPE@
        # ^^^^^^^^^^^^^^^^^^^^^  These are tags.

        tag_replacements = {
            "TEMPLATED_DATA_TYPE": format_struct_data_type(p_templated_type),
            "PATCH_TEMPLATED_DATA_TYPE": p_templated_type,
            "SELF": f"struct {self.name}"
        }

        for idx,fn in enumerate(self.member_functions):
            fn_body = fn.fn_body

            # Find and replace tags between '@'
            i = 0
            modified = False
            while i < len(fn_body):
                start_idx = fn_body.find("@", i)
                if start_idx == -1:
                    break

                if fn_body.startswith("@TYPEOF(", start_idx):
                    #@TYPEOF(table) -> struct DictObject_int
                    #        ^^^^^^   var_name
                    #^                start_pos
                    #             ^   end_pos
                    start_pos = start_idx
                    end_pos = fn_body.find(")", start_idx)

                    if end_pos == -1:
                        RAISE_ERROR(f"Couldn't find closing ')' for @TYPEOF( tag in function body.")

                    var_name = fn_body[start_pos + len("@TYPEOF(") : end_pos]

                    member_type = self.get_type_of_member(var_name)
                    if member_type == None:
                        RAISE_ERROR(f"Struct doesnt have member named {var_name} to replace inside function body.")
                    else:
                        # Strip away pointers.
                        # (We could add another @tag@ to get full data type with pointers)
                        ptr_less_data_type = format_struct_data_type(member_type.replace("*", ""))

                        # Replace only in the region between start_pos and end_pos
                        fn_body = fn_body[:start_pos] + ptr_less_data_type + fn_body[end_pos + 1:]
                        i = start_pos + len(ptr_less_data_type)
                        modified = True
                        continue

                end_idx = fn_body.find("@", start_idx + 1)
                if end_idx == -1:
                    break

                # Extract the tag between the '@' symbols
                tag = fn_body[start_idx + 1:end_idx]
                
                # Check if the tag is in the dictionary, then replace it
                if tag in tag_replacements:
                    replacement_value = tag_replacements[tag]
                    # Replace only in the region between start_idx and end_idx
                    fn_body = fn_body[:start_idx] + replacement_value + fn_body[end_idx + 1:]
                    i = start_idx + len(replacement_value)
                    modified = True
                else:
                    i = end_idx + 1
            if modified:
                self.member_functions[idx].fn_body = fn_body



    def resolve_tags_in_unparsed_functions(self, p_templated_type: str) -> None:
        # if we want to use template type in fn body, we use following syntax.
        # @TEMPLATED_DATA_TYPE@
        # ^^^^^^^^^^^^^^^^^^^^^  These are tags.

        # TODO: We probably dont want to perform these replacements for normal functions, just c_functions.

        tag_replacements = {
            "TEMPLATED_DATA_TYPE": format_struct_data_type(p_templated_type),
            "PATCH_TEMPLATED_DATA_TYPE": p_templated_type,
            "SELF": f"struct {self.name}"
        }

        for idx,fn in enumerate(self.unparsed_functions):
            fn_body = fn

            # Find and replace tags between '@'
            i = 0
            modified = False
            while i < len(fn_body):
                start_idx = fn_body.find("@", i)
                if start_idx == -1:
                    break

                if fn_body.startswith("@TYPEOF(", start_idx):
                    #@TYPEOF(table) -> struct DictObject_int
                    #        ^^^^^^   var_name
                    #^                start_pos
                    #             ^   end_pos
                    start_pos = start_idx
                    end_pos = fn_body.find(")", start_idx)

                    if end_pos == -1:
                        RAISE_ERROR(f"Couldn't find closing ')' for @TYPEOF( tag in function body.")

                    var_name = fn_body[start_pos + len("@TYPEOF(") : end_pos]

                    member_type = self.get_type_of_member(var_name)
                    if member_type == None:
                        RAISE_ERROR(f"Struct doesnt have member named {var_name} to replace inside function body.")
                    else:
                        # Strip away pointers.
                        # (We could add another @tag@ to get full data type with pointers)
                        ptr_less_data_type = format_struct_data_type(member_type.replace("*", ""))

                        # Replace only in the region between start_pos and end_pos
                        fn_body = fn_body[:start_pos] + ptr_less_data_type + fn_body[end_pos + 1:]
                        i = start_pos + len(ptr_less_data_type)
                        modified = True
                        continue

                end_idx = fn_body.find("@", start_idx + 1)
                if end_idx == -1:
                    break

                # Extract the tag between the '@' symbols
                tag = fn_body[start_idx + 1:end_idx]
                
                # Check if the tag is in the dictionary, then replace it
                if tag in tag_replacements:
                    replacement_value = tag_replacements[tag]
                    # Replace only in the region between start_idx and end_idx
                    fn_body = fn_body[:start_idx] + replacement_value + fn_body[end_idx + 1:]
                    i = start_idx + len(replacement_value)
                    modified = True
                else:
                    i = end_idx + 1
            if modified:
                self.unparsed_functions[idx] = fn_body


    def resolve_templated_member_fns(self, p_templated_type: str) -> None:
        for idx,fn in enumerate(self.member_functions):
            # Patch all the templated types in the function arguments.
            for argid,argument in enumerate(fn.fn_arguments):
                if argument.data_type == self.template_defination_variable:
                    arg = format_struct_data_type(p_templated_type)
                    self.member_functions[idx].fn_arguments[argid].data_type = arg
            
            # Patch return type of the function it is templated.
            if fn.return_type == self.template_defination_variable:
                return_type = format_struct_data_type(p_templated_type)
                self.member_functions[idx].return_type = return_type
            elif f"<{self.template_defination_variable}>" in fn.return_type:
                # OrderedDictObject<T>, T = Symbol -> OrderedDictObject_Symbol
                return_type = fn.return_type.replace(f"<{self.template_defination_variable}>", "_" + p_templated_type)
                self.member_functions[idx].return_type = return_type


class StructInstance:

    def __init__(
        self,
        p_struct_type,
        p_struct_name,
        p_scope,
    ) -> None:
        self.struct_type = p_struct_type
        self.struct_name = p_struct_name

        # 'scope' denotes where this struct was created.
        self.scope = p_scope

        # 'should_be_freed' is a tag wether the destructor should be called when it reaches out of the scope.
        # Things like Function parameters shouldn't be freed at the end of the scope.
        self.should_be_freed = True
        
        # Variables like 'this' pointer inside class functions are pointer types.
        # This tag helps in code generation for function calls.
        # i.e pointer_types are passed directly to class function call i.e fn(variable_name,...), 
        # whereas non pointer variables are called as fn(&variable_name,...).
        self.is_pointer_type = False

    def get_struct_defination(self) -> Optional[Struct]:
        StructInfo = get_struct_defination_of_type(self.struct_type)
        return StructInfo

    def struct_type_has_constructor(self) -> bool:
        return self.get_struct_defination().has_constructor()

    def struct_type_has_destructor(self) -> bool:
        return self.get_struct_defination().has_destructor()
    
    def get_mangled_function_name(self, p_fn_name: str, parameters = None) -> str:
        StructInfo = self.get_struct_defination()

        if StructInfo.function_is_overloaded(p_fn_name):
            return get_overloaded_mangled_fn_name(self.struct_type, p_fn_name, parameters if parameters != None else [])
        else:
            return get_mangled_fn_name(self.struct_type, p_fn_name)

    def is_return_type_ref_type(self, p_fn_name, parameters = None):
        struct_info = self.get_struct_defination()
        if struct_info.function_is_overloaded(p_fn_name):
            return struct_info.is_return_type_of_overloaded_fn_ref_type(p_fn_name, parameters if parameters != None else [])
        else:
            return struct_info.is_return_type_of_fn_ref_type(p_fn_name, "")

    def get_return_type_of_fn(self, p_fn_name, parameters = None):
        return_type = None
        
        struct_info = self.get_struct_defination()
        if struct_info.function_is_overloaded(p_fn_name):
            return_type = struct_info.get_return_type_of_overloaded_fn(p_fn_name, parameters if parameters != None else [])
        else:
            return_type = struct_info.get_return_type_of_fn(p_fn_name)
        
        # For Vector<String>, the String here is of struct type.
        # TODO : templated_data_type should probably be 'struct String' instead of just 'String' to avoid all this comparision.
        return_type = format_struct_data_type(return_type)

        return return_type            

    def get_fn_arguments(self, p_fn_name, parameters = None):
        fn_args = None

        StructInfo = self.get_struct_defination()
        if StructInfo.function_is_overloaded(p_fn_name):
            fn_args = StructInfo.get_function_arguments_with_types(p_fn_name, parameters if parameters != None else [])
        else:
            fn_args = StructInfo.get_function_arguments(p_fn_name)

        if fn_args == None:
            RAISE_ERROR(f"Arguments for function {p_fn_name} not found.")

        return fn_args

    def get_destructor_fn_name(self) -> str:
        return self.get_mangled_function_name("__del__")


def get_struct_defination_of_type(p_struct_type: str) -> Optional[Struct]:
    return struct_definations.get(p_struct_type, None)
    

def add_fn_member_to_struct(p_struct_name: str, p_fn: MemberFunction):
    struct_defination = get_struct_defination_of_type(p_struct_name)
    if struct_defination is None:
        RAISE_ERROR(f"Struct type {p_struct_name} doesn't exist.")
    else:
        fn_name = p_fn.fn_name
        fn_already_exists = struct_defination.has_member_fn(fn_name)

        struct_defination.member_functions.append(p_fn)
        # 'is_overloaded_function' is for class overloads.
        # c_function<> push(value : T) & c_function<String> push(value : T)
        #            ^                              ^^^^^^
        # For the cases above, 'is_overloaded_function' is true.
        # The overloading we are setting below is overloading by parameter types.  
        if fn_already_exists and p_fn.is_overloaded_function == False:
            struct_defination.set_functions_overloaded(fn_name)


def add_fnbody_to_member_to_struct(p_struct_name: str, p_fn_name: str, p_fn_body: str):
    struct_defination = get_struct_defination_of_type(p_struct_name)
    if struct_defination is None:
        RAISE_ERROR(f"Struct type {p_struct_name} doesn't exist.")
    else:
        # If not reversed, this will always find the first function and set the fn body to that.
        # because we have overloaded functions, all those functions have same name.
        # This function should set function body to the function which has been recently registered to the struct defination.
        for fn in reversed(struct_defination.member_functions):
            if fn.fn_name == p_fn_name:
                fn.fn_body = p_fn_body
                break


""" Add destructor to a struct given it's member's destructors. We don't recursively check & add destructors to it's members. """
def generate_destructor_for_struct(p_struct_name: str):
    struct_defination = get_struct_defination_of_type(p_struct_name)

    if struct_defination is None:
        # Provided type is not a struct, so need to add a destructor.
        return

    if struct_defination.has_destructor():
        # Provided type already has a destructor, so no need to add a destructor.
        return

    fn_body = ""

    # Write member destructors in reverse order.
    for member in struct_defination.members[::-1]:
        member_type = member.data_type

        member_struct_def = get_struct_defination_of_type(member_type)
        if member_struct_def is None:
            continue

        if member_struct_def.has_destructor():
            destructor_mangled_fn_name = get_mangled_fn_name(member_type, "__del__")
            fn_body += f"{destructor_mangled_fn_name}(&this->{member.member});\n"

    params = []
    fn = MemberFunction("__del__", params, "void")
    fn.fn_body = fn_body

    add_fn_member_to_struct(p_struct_name, fn)

    mangled_destructor_fn_name = get_mangled_fn_name(p_struct_name, "__del__")

    global GlobalGeneratedStructDestructors
    GlobalGeneratedStructDestructors += f"void {mangled_destructor_fn_name}(struct {p_struct_name} *this){{ \n"
    GlobalGeneratedStructDestructors += fn_body
    GlobalGeneratedStructDestructors += "}\n\n"


def is_instanced_struct(p_struct_name: str):
    return any(struct.struct_name == p_struct_name for struct in instanced_struct_names)


def get_instanced_struct(p_struct_name) -> Optional[StructInstance]:
    for struct in reversed(instanced_struct_names):
        if struct.struct_name == p_struct_name:
            return struct
    return None


def get_struct_type_of_instanced_struct(p_struct_name):
    for struct in instanced_struct_names:
        if struct.struct_name == p_struct_name:
            return struct.struct_type


def get_destructor_for_struct(p_name):
    for struct in instanced_struct_names[::-1]:
        if struct.should_be_freed:
            # Maybe : Check for scope
            # Since variables can't be repeated across scopes,
            # We mayn't need to check for scopes.
            struct_name = struct.struct_name
            if struct_name == p_name:
                if not struct.struct_type_has_destructor():
                    struct.get_struct_defination().fill_with_destructors_recursive()

                if struct.struct_type_has_destructor():
                    destructor_fn_name = struct.get_destructor_fn_name()
                    des_code = f"{destructor_fn_name}(&{struct_name});\n"
                    return des_code
    return None


def remove_struct_instance(p_instance_name):
    global instanced_struct_names
    instanced_struct_names = [instance for instance in instanced_struct_names if instance.struct_name != p_instance_name]


class MacroDefination:
    def __init__(self, p_name, p_first_param, p_fn_body) -> None:
        self.name = p_name
        self.first_param = p_first_param
        self.fn_body = p_fn_body
        self.first_param_initializer_list = False


MacroDefinations = []


def is_macro_name(p_name: str) -> bool:
    return any(macro_def.name == p_name for macro_def in MacroDefinations)


def get_macro_def_by_name(p_name: str):
    for macro_def in MacroDefinations:
        if macro_def.name == p_name:
            return macro_def


def get_macro_def_by_name_of_class(p_macro_name: str, struct_type):
    StructInfo = get_struct_defination_of_type(struct_type)

    for macro_def in StructInfo.macro_definations:
        if macro_def.name == p_macro_name:
            return macro_def

    RAISE_ERROR(f'{struct_type} has no macro names "{p_macro_name}".')


is_inside_def = False
currently_reading_def_name = ""
currently_reading_def_macro_type = ""  # NORMAL_MACRO,CLASS_MACRO
currently_reading_def_paramter_initializer_list = False
currently_reading_def_target_class = ""
currently_reading_def_parameter = ""
currently_reading_def_body = ""

is_inside_new_code = False
is_inside_struct_c_function = False
is_inside_global_c_function = False

namespace_name = ""
is_inside_name_space = False

temp_arr_length_variable_count = 0
temp_c_str_iterator_variable_count = 0
temp_string_object_variable_count = 0
temp_arr_search_variable_count = 0
temp_char_promoted_to_string_variable_count = 0

# This is to index which loop variable we are using for 'for loops'.
# like i,j,k...z,a..i
# First scope will use i.
# If we create another for within already created i loop, we now use j index and so on.
for_loop_depth = 0

# Constant Expressions Related Stuffs.
class ConstexprDictionaryType:
    def __init__(self, p_dict_name: str, p_dictionary: dict) -> None:
        self.dict_name = p_dict_name
        self.dictionary = p_dictionary
        self.default_key_value = None
        self.dict_type = "number" # numbers or strings.

def generate_contexpr_dict_runtime_lookup_code(func_name, contexpr_dict):
    param_name = "p_string"
    if contexpr_dict.dict_type == "number":
        raise ValueError("Cant generate function for Constexpr Dictionary of type number.")
    if contexpr_dict.default_key_value is None:
        raise ValueError("Need a default value for Constexpr Dictionary.")

    fn_declaration = f"struct String {func_name}(struct String *{param_name})"

    lines = []
    lines.append(f"{fn_declaration}{{")

    first = True
    for key, value in contexpr_dict.dictionary.items():
        if first:
            lines.append(f' if (String__eq__({param_name}, "{key}")) {{')
            first = False
        else:
            lines.append(f' else if (String__eq__({param_name}, "{key}")) {{')
        
        lines.append(f'     struct String result;')
        lines.append(f'     String__init__from_charptr(&result, "{value}", {len(value)});')
        lines.append(f'     return result;')
        lines.append(f' }}')

    # Default return
    default_value = contexpr_dict.default_key_value
    lines.append(f'        struct String result;')
    lines.append(f'        String__init__from_charptr(&result, "{default_value}", {len(default_value)});')
    lines.append(f'        return result;')
    lines.append("}")
    fn_body = "\n".join(lines)

    return fn_declaration, fn_body


constexpr_dictionaries = []


def is_constexpr_dictionary(p_dict_name) -> bool:
    return any(m_dict.dict_name == p_dict_name for m_dict in constexpr_dictionaries)

def is_numeric_constexpr_dictionary(p_dict_name) -> bool:
    for m_dict in constexpr_dictionaries:
        if m_dict.dict_name == p_dict_name and m_dict.dict_type == "number":
            return True
    return False

def is_string_constexpr_dictionary(p_dict_name) -> bool:
    for m_dict in constexpr_dictionaries:
        if m_dict.dict_name == p_dict_name and m_dict.dict_type == "string":
            return True
    return False


class Annotation:
    # @route("/Home")
    #         ^^^^^^ annotation_argument_value
    #  ^^^^^ annotation_name
    # function Home():
    #          ^^^^ annotated_fn_name
    def __init__(self, p_annotation_name, p_annotation_argument_values, p_annotated_fn_name):
        self.annotation_name = p_annotation_name
        self.annotation_argument_values = p_annotation_argument_values
        self.annotated_fn_name = p_annotated_fn_name

annotations_list = []
temporary_annotations_list = []

def has_static_annotation():
    return any(a.annotation_name == "static" for a in temporary_annotations_list)

def consume_annotation(annotation_name):
    global temporary_annotations_list
    temporary_annotations_list = [
        annotation for annotation in temporary_annotations_list if annotation.annotation_name != annotation_name
    ]



##############################################################################################
class HookInfo:
    # @hook_begin("custom_integer_printer" "int" data)
    #              ^ p_hook_fn_name              ^ p_hook_var_name
    #                                       ^ p_hook_fn_arg_type
    # The hook fn 'p_hook_fn_name' uses 'p_hook_var_name' of type 'p_hook_fn_arg_type'.
    def __init__(self, p_hook_fn_name, p_hook_fn_arg_type, p_hook_var_name) -> None:
        self.hook_fn_name = p_hook_fn_name
        self.hook_fn_arg_type = p_hook_fn_arg_type
        self.hook_var_name = p_hook_var_name


def parse_hook_info(p_hook_body) -> HookInfo:
    # @hook_begin("custom_integer_printer" "int" data)
    hook_parser = Parser.Parser(p_hook_body)

    hook_parser.consume_token(lexer.Token.AT)
    hook_parser.consume_token(lexer.Token.HOOK_BEGIN)
    hook_parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

    hook_fn_name = hook_parser.extract_string_literal()
    hook_fn_arg_type = hook_parser.extract_string_literal()
    hook_var_name = hook_parser.get_token()

    hook_parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

    return HookInfo(hook_fn_name, hook_fn_arg_type, hook_var_name)


##############################################################################################

def parse_global_c_function():
    global temporary_annotations_list, GlobalGeneratedFunctionDeclarations, should_write_fn_body
    global GlobalStructInitCode, global_variables_initialization_code, is_inside_global_c_function

    should_write_fn_body = True

    function_declaration = parse_function_declaration()

    function_name = function_declaration["fn_name"]
    return_type = function_declaration["return_type"]
    is_return_type_ref_type = function_declaration["is_return_type_ref_type"]
    parameters = function_declaration["parameters"]
    parameters_combined_list = function_declaration["parameters_combined_list"]
    is_overloaded = function_declaration["is_overloaded"]
    is_overloaded_fn = function_declaration["is_overloaded_fn"]
    overload_for_type = function_declaration["overload_for_type"]

    # if this function is a struct member function, it needs to be mangled below.
    func_name = function_name

    defining_fn_for_custom_class = False
    
    if parser.has_tokens_remaining():
        # function my_first_ANIL_function() for String
        if parser.check_token(lexer.Token.FOR):
            parser.consume_token(lexer.Token.FOR)

            target_class = parser.get_token()

            if function_name == "WinMain":
                RAISE_ERROR(f"WinMain is a global entry point. It can't be a member function of \"{namespace_name}\".")

            defining_fn_for_custom_class = True
            class_fn_defination["class_name"] = target_class
            class_fn_defination["function_name"] = function_name
            class_fn_defination["start_index"] = ctx.get_generated_lines_count()
        else:
            RAISE_ERROR('Expected for after function declaration like "function A() for namespace_name".')

    struct_def = None
    if defining_fn_for_custom_class:
        struct_name = class_fn_defination["class_name"]
        struct_def = get_struct_defination_of_type(struct_name)

    # TODO: This need not be performed as the template types should be resolved at template instantiation time.
    # This could be performed by updating 'unparsed_function' during template instantiation.
    def resolve_template_data_type(p_type):
        if struct_def != None:
            if p_type == struct_def.template_defination_variable:
                if struct_def.templated_data_type:
                    return struct_def.templated_data_type
        return p_type

    if defining_fn_for_custom_class:
        if is_overloaded:
            # Get datatype from MemberDataType,
            # as get_overloaded_mangled_fn_name requires list of strings(data_type).
            data_types_str_list = [p.data_type for p in parameters]
            func_name = get_overloaded_mangled_fn_name(class_fn_defination["class_name"], function_name, data_types_str_list)
        else:
            func_name = get_mangled_fn_name(class_fn_defination["class_name"], function_name)
    
        if temporary_annotations_list:
            RAISE_ERROR("Annotations are not supported for class functions.")
    else:
        # Annotations are implemented for global functions only as of now.
        if temporary_annotations_list:
            # PATCH_ANNOTATION
            # Patch stored (stacked) temporary annotations, which didn't have fn_name.
            for i in range(len(temporary_annotations_list)):
                temporary_annotations_list[i].annotated_fn_name = function_name
            annotations_list.extend(temporary_annotations_list)
            temporary_annotations_list = []

    if defining_fn_for_custom_class:
        if struct_def != None:
            if struct_def.is_templated():
                should_write_fn_body = False

    code = ""

    return_type = format_struct_data_type(resolve_template_data_type(return_type))

    has_parameters = len(parameters) > 0

    code = f"{return_type} {func_name}("
    if function_name == "WinMain":
        # We dont need to use func_name, as we have validated that function_name is a global function,
        # and not a mangled name.
        code = f"{return_type} WINAPI {function_name}("

    if defining_fn_for_custom_class:
        struct_name = class_fn_defination["class_name"]
        code += f"struct {struct_name} *this"
        if has_parameters:
            code += ","
            
    if has_parameters:
        params = [f"{format_struct_data_type(p.data_type)} {p.member}" for p in parameters]
        parameters_str = ",".join(params)
        code += f"{parameters_str}"

    GlobalGeneratedFunctionDeclarations += code + ");\n"
    
    code += f") {{\n"

    emit(code)

    if is_anil_file and function_name in ("main", "WinMain"):
        # Normal .c file has identifiers which indicates where the global variables constructors is placed in main.
        # .anil files doesn't have that, so, the first line for main function is the global variables constructors
        # initialization code.
        if len(global_variables_initialization_code) > 0:
            emit("//Global Variables Initialization.\n")
            ctx.extend_generated_lines(global_variables_initialization_code)
            emit("\n")
            global_variables_initialization_code = []
    
    fn = MemberFunction(function_name, parameters, return_type)
    fn.is_overloaded_function = is_overloaded_fn
    fn.overload_for_template_type = overload_for_type
    fn.is_return_type_ref_type = is_return_type_ref_type
    
    if defining_fn_for_custom_class:
        if should_write_fn_body:
            GlobalStructInitCode += code
        add_fn_member_to_struct(class_fn_defination["class_name"], fn)
        class_fn_defination["function_destination"] = "class"
    else:
        GlobalFunctions.append(fn)
        class_fn_defination["function_destination"] = "global"
        class_fn_defination["function_name"] = function_name

    is_inside_global_c_function = True


##############################################################################################


Error_Handler = ErrorHandler()
Error_Handler.register_source_file(source_file)

def RAISE_ERROR(error_msg):
    Error_Handler.raise_error(error_msg)


def create_const_charptr_iterator(array_name, current_array_value_variable):
    global temp_c_str_iterator_variable_count

    iterator_var_name = f"{array_name}_iterator_{temp_c_str_iterator_variable_count}"

    emit(
        f"char *{iterator_var_name} = {array_name};"
        f"while (*{iterator_var_name} != '\\0') {{"
        f"char {current_array_value_variable} = *{iterator_var_name};"
        f"{iterator_var_name}++;"
    )

    REGISTER_VARIABLE(current_array_value_variable, "char")

    temp_c_str_iterator_variable_count += 1


def create_array_enumerator(
    array_name, ranged_index_item_variable, current_array_value_variable
):
    emit(
        f"Iterator {array_name}_iter = create_iterator_from_array({array_name}, {array_name}_array_size); \n"
        f"Enumerator {array_name}_enumerator;\n"
        f"{array_name}_enumerator.index = -1;\n\n"
        f"while (enumerate(&{array_name}_iter, &{array_name}_enumerator)) {{\n"
        f"int {ranged_index_item_variable} = {array_name}_enumerator.index;\n"
        f"int {current_array_value_variable} = {array_name}_enumerator.val;\n"
    )


def create_normal_array_iterator(array_name, current_array_value_variable):
    # The variable type is in format '[int]'.
    array_type = get_type_of_variable(array_name)
    if array_type == None:
        RAISE_ERROR(f"{array_type} isn't a registered array type.")

    array_type = array_type[1:-1]

    emit(
        f"for (unsigned int i = 0; i < {array_name}_array_size; i++){{\n"
        f"{array_type} {current_array_value_variable} = {array_name}[i];\n"
    )

    REGISTER_VARIABLE(current_array_value_variable, array_type)

def promote_char_to_string(var_to_check):                             
    global temp_char_promoted_to_string_variable_count
    
    promoted_char_var_name = f"{var_to_check}_promoted_{temp_char_promoted_to_string_variable_count}"
    temp_char_promoted_to_string_variable_count += 1

    emit(f"char {promoted_char_var_name}[2] = {{ {var_to_check}, '\\0'}};\n")
    REGISTER_VARIABLE(f"{promoted_char_var_name}", "str")

    return promoted_char_var_name


def handle_array_in_operator(var_to_check, var_to_check_against):
    global temp_arr_search_variable_count

    search_variable = f"{var_to_check_against}__contains__{var_to_check}_{temp_arr_search_variable_count}"
    temp_arr_search_variable_count += 1

    emit(
        f"bool {search_variable} = false;\n"
        f"for (unsigned int i = 0; i < {var_to_check_against}_array_size; i++){{\n"
        f"  if ({var_to_check_against}[i] == {var_to_check}){{\n"
        f"      {search_variable} = true;\n"
        f"      break;\n"
        f"  }}\n"
        f" }}\n"
    )

    REGISTER_VARIABLE(search_variable, f"bool")
    return search_variable


class ParameterType(Enum):
    UNDEFINED = -1
    RAW_STRING = 0
    CHAR_TYPE = 1
    STR_TYPE = 2
    STRING_CLASS = 3
    NUMBER = 4
    VARIABLE = 5
    BOOLEAN_CONSTANT = 6 # True or False
    FUNCTION_POINTER = 7
    STRING_EXPRESSION = 8


def _parameters_to_types_str_list(parameters : list) -> list:
    strs = []
    for param in parameters:
        if param.param_type == ParameterType.NUMBER or param.param_type == "int":
            strs.append("int")
        elif param.param_type == ParameterType.CHAR_TYPE:
            strs.append("char")
        elif param.param_type == ParameterType.RAW_STRING or param.param_type == ParameterType.STR_TYPE:
            strs.append("char*")
        elif param.param_type == ParameterType.STRING_CLASS or param.param_type == ParameterType.STRING_EXPRESSION or param.param_type == "struct String" or param.param_type == "String":
            strs.append("struct String")
        elif param.param_type == ParameterType.FUNCTION_POINTER:
            strs.append("fn_ptr")
        elif param.param_type == ParameterType.VARIABLE:
            strs.append(get_type_of_variable(param.param))
        elif is_data_type_struct_object(data_type_with_struct_stripped(param.param_type)):
            strs.append(data_type_with_struct_stripped(param.param_type))
        else:
            RAISE_ERROR(f"Unimplemented for {param.param_type}.")
    return strs


class SimpleAst:
    def __init__(self):
        self.params = []

    def add_param(self, param_name, param_type):
        param = (param_name, param_type)
        self.params.append(param)

    def update_param(self, idx, new_param_name, new_param_type):
        param = (new_param_name, new_param_type)
        self.params[idx] = param

    def remove_param(self, index):
        del self.params[index]


class Parameter:
    def __init__(self, p_param, p_param_type: ParameterType) -> None:
        self.param = p_param
        self.param_type = p_param_type
        self.struct_instance = None # For a.b, we may need instance info of b later, for e.g in boolean_expression()
        
        self.fn_call_parse_info = None
        self.ast = SimpleAst()


def handle_raw_string_equality(var_to_check_against, var_to_check, l_type, struct_info, is_struct, negation):
    if is_struct:
        fn_name = struct_info.get_mangled_function_name("__eq__")
        code = f'{fn_name}(&{var_to_check_against}, "{var_to_check}")'
    else:
        if l_type == ParameterType.CHAR_TYPE or l_type == "char":
            code = f"{var_to_check_against} == '{var_to_check}'"
        else:
            code = f'{var_to_check_against} == "{var_to_check}"'

    return f"!({code})" if negation else code


def handle_struct_equality(var_to_check_against, var_to_check, r_type, struct_info, negation):
    code = ""

    # Create a function expression and merge the tokens to the current parser.
    # Parse the function expression using the recently merged tokens.
    ANIL_code = f"{var_to_check_against}.__eq__({var_to_check})".replace("->",".").replace("&","")
    fn_parser = Parser.Parser(ANIL_code)
    parser.tokens = fn_parser.tokens + parser.tokens
    fn_call_parse_info = function_call_expression()
    if fn_call_parse_info == None:
        RAISE_ERROR(f"For \"{ANIL_code}\", Struct equality fn call parsing failed.")
    else:
        code = fn_call_parse_info.get_fn_str()

    if negation:
        code = f"!{code}"

    if struct_info.struct_type == "String" and r_type == ParameterType.STRING_CLASS:
        return code

    return {"code": code, "return_type": return_type}


def handle_char_equality(var_to_check_against, var_to_check, l_type, negation):
    code = f"{var_to_check_against} == '{var_to_check}'"
    return f"!({code})" if negation else code


class ParsedFunctionCallType(Enum):
    # To denote x = A.fn();
    STRUCT_FUNCTION_CALL = 0
    # To denote x = fn();
    GLOBAL_FUNCTION_CALL = 1
    # To denote x = A.B;
    MEMBER_ACCESS_CALL =  2
    # To denote x = ClassName::fn(); where fn is static function.
    CLASS_STATIC_FUNCTION_CALL = 3


class ParseErrorException(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)


class SpeculativeTokenType(Enum):
    NONE = 0
    NUMERIC_CONSTANT = 1 # 42
    NUMERIC_VARIABLE = 2 # a = 45
    STRING_LITERAL = 3 # a = "Hello World"
    STR_TYPE = 4
    STRING_CLASS = 5 # a = String{"Hello World"}
    CHAR_TYPE = 6
    STRING_EXPRESSION = 7 # "StringStrip(&s1, 1, 5)"


# Maybe Use ParameterType instead of SpeculativeTokenType.
def SpeculativeTokenTypeToParameterType(tk_type):
    mapping = {
        SpeculativeTokenType.NONE: ParameterType.UNDEFINED,
        SpeculativeTokenType.NUMERIC_CONSTANT: ParameterType.NUMBER,
        SpeculativeTokenType.NUMERIC_VARIABLE: ParameterType.VARIABLE,
        SpeculativeTokenType.STRING_LITERAL: ParameterType.RAW_STRING,
        SpeculativeTokenType.STR_TYPE: ParameterType.STR_TYPE,
        SpeculativeTokenType.STRING_CLASS: ParameterType.STRING_CLASS,
        SpeculativeTokenType.CHAR_TYPE: ParameterType.CHAR_TYPE,
        SpeculativeTokenType.STRING_EXPRESSION: ParameterType.STRING_EXPRESSION,
    }

    if tk_type in mapping:
        return mapping[tk_type]

    RAISE_ERROR(f"{tk_type} conversion error.")


class SpeculativeParseInfo:
    def __init__(self):
        self.speculative_token_type = SpeculativeTokenType.NONE
        self.speculative_token_value = ""
        self.speculative_exact_type = ""


class SpeculativeExpressionType(Enum):
    NONE = 0
    NUMERIC_EXPRESSION = 1 # 1 + 2 + 3
    STRING_EXPRESSION = 2 # a = 45
    FUNCTION_CALL_EXPRESSION = 3 # a.f()


class SpeculativeExpressionInfo:
    def __init__(self):
        self.speculative_expression_type = SpeculativeExpressionType.NONE
        self.speculative_expression_value = ""
        self.function_call_metadata = {} # For Function Call expression to return stuffs.

        self.ast = SimpleAst() # Individual members of a string expression "Hello" + "World" are stored in this ast.

    def get_fn_str(self) -> str:
        # Returns the parsed expression in code form.
        # For e.g: "Stringlen(&TestString)"
        parse_result = self.function_call_metadata
        parsed_fn_call_type = parse_result["function_call_type"]
        # return_type = parse_result["return_type"]
        member_access_string = parse_result["member_access_string"]

        code = ""

        if parsed_fn_call_type == ParsedFunctionCallType.MEMBER_ACCESS_CALL:
            code = member_access_string
        else:
            fn_name = parse_result["fn_name"]
            has_parameters = parse_result["has_parameters"]

            if has_parameters:
                parameters_str = parse_result["parameters_str"]
                # For global fn, member_access_string is empty.
                if member_access_string != "":
                    code = f"{fn_name}({member_access_string}, {parameters_str})"
                else:
                    code = f"{fn_name}({parameters_str})"
            else:
                # Function hooks take no parameters.
                global HOOKS_hook_fn_name
                if HOOKS_hook_fn_name != "":
                    global hook_fn_name
                    global HOOKS_target_fn
                    code = f"{fn_name}_hooked_{hook_fn_name}({member_access_string},{HOOKS_target_fn})"
                    HOOKS_hook_fn_name = ""
                    HOOKS_target_fn = ""
                else:
                    code = f"{fn_name}({member_access_string})"
        return code


def _quote_string_params(actual_fn_args, provided_parameters):
    parameters_quoted = []
    for arg, parameter in zip(actual_fn_args, provided_parameters):
        param = parameter.param
        param_type = parameter.param_type

        if param_type == ParameterType.RAW_STRING:
            if (arg == "char*") or (arg == "str") or (arg == "c_str") or (arg == "struct String") or (arg == "String"):
                param = f'"{param}"'
            else:
                param = f"'{param}'"
            parameters_quoted.append(Parameter(param, param_type))
        else:
            parameters_quoted.append(parameter)
    return parameters_quoted    


class BooleanExpressionType:
    def __init__(self):
        self.return_type = ""
        self.return_value = ""

        # For 'return a == b'
        # The structs involved are 'a' and 'b'
        self.structs_involved = []

        self.returns_single_value = False
        #    ^^^^^^^^^^^^^^^^^^^^ if we properly fetched all 'structs_involved' then this can be removed.



index = 0

while index < len(Lines):
    # Just strip away spaces and not other kinds of whitespaces, as that messes up final code generation.
    Line = Lines[index].strip(" ")
    index += 1

    if Line.startswith("// clang-format off"):
        continue
    elif Line.startswith("// clang-format on"):
        continue
    elif Line.startswith("import "):
        continue
    elif Line.startswith("///*///"):
        is_inside_new_code = not is_inside_new_code
        if "main()" in Line:
            if main_fn_found:
                RAISE_ERROR("Main function already declared. Can't declare another main function.")
            else:
                main_fn_found = True
                if len(global_variables_initialization_code) > 0:
                    code_generator.emit_comment("//Global Variables Initialization.")
                    for g_code in global_variables_initialization_code:
                        code_generator.emit(g_code)
                    global_variables_initialization_code = []
                continue
        else:
            continue
    elif Line.startswith("<form>"):
        if is_inside_form:
            RAISE_ERROR("Form within a form, not allowed.")
        else:
            is_inside_form = True
        continue
    elif Line.startswith("</form>"):
        if not is_inside_form:
            RAISE_ERROR("Not inside a form, can't close it.")
        else:
            is_inside_form = False
            gui_code = gui_manager.get_window_code()
            for code in gui_code:
                emit(code)
        continue
    elif Line.startswith("<UI>"):
        if is_inside_GUI_code:
            RAISE_ERROR("GUI within a GUI, not allowed.")
        else:
            is_inside_GUI_code = True
        continue
    elif Line.startswith("</UI>"):
        if not is_inside_GUI_code:
            RAISE_ERROR("Not inside a GUI, can't close it.")
        else:
            is_inside_GUI_code = False
            ANIL_GUI_code = JSXlike_element_tree.generate_code()
            insert_intermediate_lines(index, ANIL_GUI_code)
        continue
    elif Line.startswith("// DESTRUCTOR_CODE //"):
        # Destructors NOTE: If structs aren't instantiated, then their destructors mayn't be emitted.
        # TODO: This could be fixed by just generating destructors of all structs whose
        # destructors haven't been emitted yet at the end of the code during final destructors generation.
        # Not a necessity for now.

        # Destroy all variables, then emit the code to be parsed later.
        # Now the code to be parsed later has fresh symbol table to work with.
        if not is_anil_file:
            emit(symbol_table.destructor_code_for_all_remaining_variables())

        templated_fn_codes = []

        for defined_struct in list(struct_definations.values()):
            if defined_struct.unparsed_functions_should_be_parsed:
                template_code = [f"namespace {defined_struct.name}\n"]
                template_code.extend(defined_struct.unparsed_functions)
                template_code.append("endnamespace\n")

                templated_fn_codes.extend(template_code)

                # See: ADD_TEMPORARY_FUNCTIONS_FOR_TEMPLATE_INSTANTIATION.
                # The temporary functions we added in instantiate_template() should be cleared,
                # defined_struct.member_functions.clear()
                # But we can't clear it here because
                # Suppose, we emit A<T>, B<Y> in this loop.
                # A<T> may use B<Y> inside it's function body.
                # If we cleared B<Y>.member_functions() here, A<T> will have unresolved function calls,
                # because later down when we are parsing namespace A<T>, we don't have B<Y> member functions(
                # as it would be cleared).
                # So, we clear the member functions when we open the corresponding namespace.
                # Inside namespace we clear the temporary member functions of the struct,
                # and start creating the actual member functions.
                defined_struct.unparsed_functions.clear()
                defined_struct.unparsed_functions_should_be_parsed = False
                defined_struct.unparsed_functions_emitted = True
        
        if templated_fn_codes:
            templated_fn_codes.append("// DESTRUCTOR_CODE //")
            # If we generate templates within this recently added fn body,
            # this '// DESTRUCTOR_CODE //' will handle it the next pass, after this namespace has completed parsing,
            # and the parser has again reached '// DESTRUCTOR_CODE //'.

            insert_intermediate_lines(index, templated_fn_codes)

    if not is_inside_new_code:
        # Normal C code, so just write that.
        emit(Line)
        continue

    if is_inside_global_c_function and not "endc_function" in Line:
        currently_reading_fn_body += Line
        continue
    elif is_inside_struct_c_function and not "endc_function" in Line:
        currently_reading_fn_body += Line
        if should_write_fn_body:
            GlobalStructInitCode += Line
        continue
    elif is_inside_def and not "enddef" in Line:
        currently_reading_def_body += Line
        # Newlines are stripped while parsing 'Line'.
        # We add these newlines, so that 'currently_reading_def_body' can be splitted using newlines later.
        # That distinguishes unique lines.
        currently_reading_def_body += "\n"
        continue

    Error_Handler.register_current_line_code(Line)
    
    parser = Parser.Parser(Line)

    def check_token(token: lexer.Token):
        return parser.check_token(token)

    def parse_slice(parser):
        start_index, step_size, end_index = None, None, None
        
        if parser.has_tokens_remaining() and parser.current_token() == lexer.Token.LEFT_SQUARE_BRACKET:
            parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)

            if not parser.check_token(lexer.Token.COLON):
                # [start:] case
                #  ^^^^^   not COLON.
                start_index = get_integer_expression("Expected integer expression for start_index.")
            
            # [:end] and [start:end] cases
            if parser.check_token(lexer.Token.COLON):
                # [:...] case or [start:..] case
                #  ^                   ^ 
                parser.consume_token(lexer.Token.COLON)
                if not parser.check_token(lexer.Token.COLON) and not parser.check_token(lexer.Token.RIGHT_SQUARE_BRACKET):
                    # [:end] case or [start:end] case
                    #   ^^^^                ^^^^
                    end_index = get_integer_expression("Expected integer expression for end_index.")
            
            if parser.check_token(lexer.Token.COLON):
                # [..:..:..]
                #       ^   case
                parser.consume_token(lexer.Token.COLON)
                if not parser.check_token(lexer.Token.RIGHT_SQUARE_BRACKET):
                    # [::step] case or [start:end:step] case
                    #    ^^^^                     ^^^^
                    step_size = get_integer_expression("Expected integer expression for step_size.")
            
            parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)
        
        return start_index, end_index, step_size

    def create_array_iterator_from_struct(array_name, current_array_value_variable):
        global temp_arr_length_variable_count
        global for_loop_depth

        #for x in list[::-1]
        #             ^^^^^^
        start_index, end_index, step_size = parse_slice(parser)

        if start_index == None:
            start_index = "0"

        loop_indices = "ijklmnopqrstuvwxyzabcdefgh"
        loop_counter_index = loop_indices[for_loop_depth % 26]

        temporary_len_var_name = f"tmp_len_{temp_arr_length_variable_count}"

        # Emit ANIL code to perform looping.
        # This line will be parsed by the compiler in next line.
        
        code = []
        
        if step_size and step_size[0] == "-":
            # Starts with Negative index.
            l1 = ""
            l1a = ""
            if end_index:
                l1 = f"let {temporary_len_var_name} : int = {end_index}\n"
            else:
                l1 = f"let {temporary_len_var_name} = {array_name}.len()\n"
                l1a = f"{temporary_len_var_name} -= 1;\n"

            l2 = f"for {loop_counter_index} in range({temporary_len_var_name}..={start_index},{step_size}){{\n"
            l3 = f"let {current_array_value_variable} = {array_name}[{loop_counter_index}]\n"
            code = [l1, l1a, l2, l3]
        else:
            l1 = ""
            if end_index:
                l1 = f"let {temporary_len_var_name} : int = {end_index}\n"
            else:
                l1 = f"let {temporary_len_var_name} = {array_name}.len()\n"
            l2 = ""
            if step_size:
                l2 = f"for {loop_counter_index} in range({start_index}..{temporary_len_var_name}, {step_size}){{\n"
            else:
                l2 = f"for {loop_counter_index} in range({start_index}..{temporary_len_var_name}){{\n"
            l3 = f"let {current_array_value_variable} = {array_name}[{loop_counter_index}]\n"
            code = [l1, l2, l3]

        insert_intermediate_lines(index, code)

        temp_arr_length_variable_count += 1

    def create_range_iterator(current_array_value_variable):
        # for i in range(1..10){    -> 0 >= i < 10
        # for i in range(1..10,2){  -> 0 >= i < 10
        # for i in range(1..=10,2){ -> 0 >= i <= 10
        #                       ^     step
        #                    ^        stop
        #                ^            start
        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

        includes_stop = False
        step = '1'

        start = get_integer_expression("Expected integer expression for start value.")

        parser.consume_token(lexer.Token.DOT)
        parser.consume_token(lexer.Token.DOT)

        if parser.check_token(lexer.Token.EQUALS):
            # for i in range(1..=10,2){ -> 0 >= i <= 10
            #                   ^                
            parser.next_token()
            includes_stop = True
        
        stop = get_integer_expression("Expected integer expression for stop value.")

        # for i in range(1..10,2){  -> 0 >= i < 10
        #                     ^
        if parser.check_token(lexer.Token.COMMA):
            parser.next_token()

            step = get_integer_expression("Expected integer expression for step value.")

        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        stopping_condition = "<"
        if includes_stop:
            if step[0] == "-":
                if stop == "0":
                    stopping_condition = "!"
                else:
                    stopping_condition = ">"
            stopping_condition += "="

        increment_operation = "++"
        if step != '1':
            increment_operation = f"+={step}"

        if stop == "0":
            emit(
                f"for (size_t {current_array_value_variable} = {start}; {current_array_value_variable} {stopping_condition} (size_t) - 1; {current_array_value_variable}{increment_operation}){{\n"
            )
        elif step[0] == "-":
            emit(
                f"for (size_t {current_array_value_variable} = {stop}; {current_array_value_variable} {stopping_condition} {start}; {current_array_value_variable}{increment_operation}){{\n"
            )
        else:
            emit(
                f"for (size_t {current_array_value_variable} = {start}; {current_array_value_variable} {stopping_condition} {stop}; {current_array_value_variable}{increment_operation}){{\n"
            )
        REGISTER_VARIABLE(current_array_value_variable, "size_t")

    def parse_function_pointer():
        # Fn(int, int) -> void
        #   ^
        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

        arg_type_list = []
        while True:
            if parser.check_token(lexer.Token.RIGHT_ROUND_BRACKET):
                break
            arg_type = parse_data_type()
            arg_type_list.append(arg_type)
            if parser.check_token(lexer.Token.COMMA):
                parser.consume_token(lexer.Token.COMMA)
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        # Fn(int, int) -> void
        #              ^
        parser.consume_token(lexer.Token.MINUS)
        parser.consume_token(lexer.Token.GREATER_THAN)

        # TODO: Maybe use parse_data_type() here as well.
        # But, does that handle 'void' types ??
        return_type = parser.get_token()

        def to_string():
            # Lookup "Function Pointer Implementation" Section. 
            # (*callback)(UIElement*, void*)
            #   ^^^^^^^^
            #                        VVVVVVV Replace FNNAME later.
            return f"{return_type} (*#FNNAME#)({', '.join(arg_type_list)})"
        
        return to_string()

    def parse_data_type(inner = False, incomplete_types = None):
        # inner is used as a tag for name mangling during recursive data parsing.

        # For templated struct like struct<T> A{X<T> B};
        #                                  ^ is stored in 'incomplete_types'.
        # That means while parsing if we encounter 'T' like for X<T>, 
        # we dont mangle X<T> and leave as it is to hint that it should be mangled,
        # during template instantiation and not parsing phase.

        # String,Vector<String>
        # This all should be parsed.
        # String
        # ^^^^^^
        data_type = parser.get_token()

        if data_type == "Fn":
            # Fn(int, int) -> void
            return parse_function_pointer()
        
        # struct Vector
        # ^^^^^^
        if data_type == lexer.Token.STRUCT:
            data_type = parser.get_token() 

        if data_type == "str":
            # str is an alias for char*, even though in most places we have checks for both "char*" and "str".
            data_type = "char*"

        data_type_str = data_type

        struct_defination = get_struct_defination_of_type(data_type)
        is_struct_type = struct_defination != None

        if not inner:
            if is_struct_type:
                data_type_str = f"struct {data_type_str}"

        # Vector<String>
        #       ^
        if parser.has_tokens_remaining() and parser.check_token(
            lexer.Token.SMALLER_THAN
        ):
            # This has_tokens_remaining() could probably integrated inside check_token() maybe.
            parser.consume_token(lexer.Token.SMALLER_THAN)
            #      Vector<Vector<String>>
            #             ^^^^^^^^^^^^^^
            # Hope, this case is recursively parsed as well.

            # TODO:Since, we don't immediately instantiate templates structs.They aren't registered here.
            # So, these checks fail, but these checks are important .
            """
            # Only a templated struct can be used to create a data type.
            if not is_struct_type:
                RAISE_ERROR(f"Struct type {data_type} doesn't exist.")
                
            if not struct_defination.is_templated():
                RAISE_ERROR(
                    f"Struct type {data_type} isn't a template class and can't be used to instantiate new data types."
                )
            """
            is_templated = True

            inner_data_type = parse_data_type(inner=True, incomplete_types=incomplete_types)

            parser.consume_token(lexer.Token.GREATER_THAN)

            if (incomplete_types != None) and (inner_data_type in incomplete_types):
                # We dont mangle templated data types.
                # Keep them in A<B> format to later parse again with appropriate types.
                data_type_str += f"<{inner_data_type}>"
                return data_type_str
            else:
                data_type_str = get_mangled_templated_class_type(data_type_str, inner_data_type)
                instantiate_template(data_type, inner_data_type)
        else:
            if incomplete_types != None:
                if data_type in incomplete_types:
                    return data_type

        return data_type_str

    def parse_array(type_name, array_name):
        parser.consume_token(lexer.Token.EQUALS)
        parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)

        array_values = []

        while parser.current_token() != lexer.Token.RIGHT_SQUARE_BRACKET:
            if type_name == "int":
                arr_value = get_integer_expression(f"Expected integer expression for {array_name}.")
            else:
                arr_value = parser.get_token()
            array_values.append(arr_value)
            # print(f"Array Value : {arr_value}")
            if parser.check_token(lexer.Token.COMMA):
                parser.next_token()

        parser.next_token()
        parser.consume_token(lexer.Token.SEMICOLON)

        code_generator.emit_array_declaration(array_type=type_name, array_name=array_name, elements=array_values)

        # register variable as '[int]' to indicate the array of type int.
        REGISTER_VARIABLE(array_name, f"[{type_name}]")

    def instantiate_template(struct_type, templated_data_type):
        # Recreate Generic Structs on instantiation.
        m_struct_name = get_mangled_templated_class_type(struct_type, templated_data_type)
        m_struct_info = get_struct_defination_of_type(m_struct_name)
        if m_struct_info != None:
            # This struct has already been defined.
            # So, we don't need to instantiate again.
            return

        StructInfo = get_struct_defination_of_type(struct_type)
        if StructInfo == None:
            RAISE_ERROR(f'While instantiating "{struct_type}<{templated_data_type}>", Struct type "{struct_type}" was undefined.')

        instantiated_struct_info = copy.deepcopy(StructInfo)
        instantiated_struct_info.name = m_struct_name

        instantiated_struct_info.substitute_template_for_member_types(templated_data_type)
        struct_code = instantiated_struct_info.get_struct_initialization_code()

        global GlobalGeneratedStructDefinations
        GlobalGeneratedStructDefinations += struct_code
        
        instantiated_struct_info.remove_unwanted_template_fn_overloads(templated_data_type)
        instantiated_struct_info.resolve_templated_member_fns(templated_data_type)
        instantiated_struct_info.resolve_tags(templated_data_type)

        # Our parent class was templated, but we aren't.
        # We are independent.
        # These are the same assignments performed by the Struct constructor.
        instantiated_struct_info.is_class_templated = False
        instantiated_struct_info.templated_data_type = templated_data_type
        instantiated_struct_info.template_defination_variable = StructInfo.template_defination_variable

        instantiated_struct_info.unparsed_functions = copy.deepcopy(StructInfo.unparsed_functions)
        instantiated_struct_info.unparsed_functions_should_be_parsed = True
        instantiated_struct_info.unparsed_functions_emitted = False

        unparsed = instantiated_struct_info.unparsed_functions  # Cache the list for easier access
        
        ###################################################################################################
        # Find all the 'unparsed functions' and store their ranges(tuple of (starting_index, ending_index)).
        unparsed_function_ranges = []

        i = 0
        length = len(unparsed)
        while i < length:
            line = unparsed[i].strip()
            
            if line.startswith(("function", "c_function")):
                start_index = i
                
                # Advance i within the inner loop to search for the corresponding end function
                while i < length and not unparsed[i].strip().startswith(("endfunction", "endc_function")):
                    i += 1
                    
                # If we found an ending marker, record the range
                if i < length:
                    unparsed_function_ranges.append((start_index, i + 1))
                else:
                    RAISE_ERROR(f"Expected 'endfunction' or 'endc_function' for function.")
            
            i += 1

        ###################################################################################################

        # Store all function declarations only.
        function_declarations = []

        # Get all the unparsed function declarations and replace the templated data type in the function declaration.
        for start_index, _ in unparsed_function_ranges:
            # Replace <T> in unparsed code.
            # Hack : We cant properly parse templates in return type like Vector<T> as it gets recursive
            # and we lose the meaning of T.
            instantiated_struct_info.unparsed_functions[start_index] = instantiated_struct_info.unparsed_functions[start_index].replace(f"<{instantiated_struct_info.template_defination_variable}>",f"<{templated_data_type}>")
            function_declarations.append(instantiated_struct_info.unparsed_functions[start_index])

        global struct_definations
        struct_definations[instantiated_struct_info.name] = instantiated_struct_info

        ###################################################################################################

        global parser
        parser.save_checkpoint()

        namespace_name = m_struct_name

        def resolve_template_data_type(p_type):
            if p_type == StructInfo.template_defination_variable:
                if templated_data_type:
                    return templated_data_type
            return p_type

        # ADD_TEMPORARY_FUNCTIONS_FOR_TEMPLATE_INSTANTIATION : 
        # Create temporary functions of unparsed functions such that struct instances can call them.
        # These functions don't have any body, but they are placeholders for the actual function,
        # but just contain return types and parameters.
        for function_declaration_line in function_declarations:
            # Load parser with new tokens related to fn declaration.
            parser_tmp = Parser.Parser(function_declaration_line)
            parser.tokens = parser_tmp.tokens

            if not parser.has_tokens_remaining():
                continue

            tk = parser.current_token()

            if tk == lexer.Token.FUNCTION:
                parser.consume_token(lexer.Token.FUNCTION)
            elif tk == lexer.Token.CFUNCTION:
                parser.consume_token(lexer.Token.CFUNCTION)
            else:
                RAISE_ERROR("Expected 'function' or 'c_function' keyword.")

            function_declaration = parse_function_declaration()

            function_name = function_declaration["fn_name"]
            return_type = function_declaration["return_type"]
            is_return_type_ref_type = function_declaration["is_return_type_ref_type"]
            parameters = function_declaration["parameters"]
            parameters_combined_list = function_declaration["parameters_combined_list"]
            is_overloaded = function_declaration["is_overloaded"]
            is_overloaded_fn = function_declaration["is_overloaded_fn"]
            overload_for_type = function_declaration["overload_for_type"]

            target_class_name = namespace_name

            if parser.has_tokens_remaining():
                # function my_first_ANIL_function() for String
                if parser.check_token(lexer.Token.FOR):
                    parser.consume_token(lexer.Token.FOR)

                    target_class_name = parser.get_token()

            for i in range(len(parameters)):
                parameters[i].data_type = resolve_template_data_type(parameters[i].data_type)
            return_type = resolve_template_data_type(return_type)

            fn = MemberFunction(function_name, parameters, return_type)
            fn.is_overloaded_function = is_overloaded_fn
            fn.overload_for_template_type = overload_for_type
            fn.is_return_type_ref_type = is_return_type_ref_type
            add_fn_member_to_struct(target_class_name, fn)

        updated_struct_info = get_struct_defination_of_type(m_struct_name)
        indices = updated_struct_info.remove_unwanted_template_fn_overloads_and_return(templated_data_type)

        # Remove unused overloads.
        for ind in reversed(indices):
            start, end = unparsed_function_ranges[ind]
            del updated_struct_info.unparsed_functions[start:end]

        updated_struct_info.resolve_tags_in_unparsed_functions(templated_data_type)

        parser.rollback_checkpoint()

    def parse_create_struct(struct_type, struct_name):
        StructInfo = get_struct_defination_of_type(struct_type)
        if StructInfo is None:
            RAISE_ERROR(f'Struct type "{struct_type}" undefined.')

        m_struct_type = struct_type

        templated_data_type = ""

        if parser.check_token(lexer.Token.SMALLER_THAN):
            parser.next_token()
            templated_data_type = resolve_templated_type(parser.get_token())
            parser.consume_token(lexer.Token.GREATER_THAN)
            m_struct_type = get_mangled_templated_class_type(struct_type, templated_data_type)

        if templated_data_type != "":
            class_already_instantiated = is_data_type_struct_object(m_struct_type)

            # Immediately instantiate templated class.
            if not class_already_instantiated:
                instantiate_template(struct_type, templated_data_type)        
        parser.consume_token(lexer.Token.LEFT_CURLY)

        values_list = []

        while parser.current_token() != lexer.Token.RIGHT_CURLY:
            # Obtained String.
            string = ""
            parsed_string = False

            if parser.check_token(lexer.Token.QUOTE):
                string = parser.extract_string_literal()
                parsed_string = True

            if parsed_string:
                arr_value = '"' + string + '"'
            else:
                arr_value = parser.get_token()
            values_list.append(arr_value)
            # print(f"Array Value : {arr_value}")
            if parser.check_token(lexer.Token.COMMA):
                parser.next_token()

        parser.next_token()
        parser.match_token(lexer.Token.SEMICOLON)

        instanced_struct_info = StructInstance(m_struct_type, struct_name, get_current_scope())

        global instanced_struct_names
        instanced_struct_names.append(instanced_struct_info)

        REGISTER_VARIABLE(struct_name, m_struct_type)

        code = f"struct {m_struct_type} {struct_name};\n"

        has_constuctor = instanced_struct_info.struct_type_has_constructor()
        if has_constuctor:
            values_str = ""
            if len(values_list) > 0:
                values_str = ",".join(values_list)

            emit(code)
            constructor_ANIL_code = f"{struct_name}.__init__({values_str})\n"

            index_to_insert_at = index
            Lines.insert(index_to_insert_at, constructor_ANIL_code)
            return
        else:
            struct_var_values_pairs = list(zip(StructInfo.members, values_list))
            for struct_member, values in struct_var_values_pairs:
                # type = struct_member[0]
                mem = struct_member.member

                code += f"{struct_name}.{mem} = {values};\n"

        emit(code)

    def _read_a_parameter():
        # Parse a number or string..
        # In any case, just a single symbol.
        parameter = None
        parameter_type = ParameterType.UNDEFINED

        parser.save_checkpoint()
        tokens_tmp = copy.deepcopy(parser.tokens)

        string_expr = speculative_parse_string_expression()
        if string_expr == None:
            parser.rollback_checkpoint()
            parser.tokens = tokens_tmp
        else:
            parameter = string_expr.speculative_expression_value
            parameter_type = ParameterType.STRING_CLASS # Maybe STRING_EXPRESSION.

            ast = string_expr.ast

            if len(ast.params) == 1:
                ast_param_name, ast_type = ast.params[0]
                parameter_type = ast_type

            p = Parameter(parameter, parameter_type)
            for ast_param_name, ast_type in ast.params:
                p.ast.add_param(ast_param_name, ast_type)
            return p   

        tk = parser.current_token()

        parser.save_checkpoint()

        with SpeculativeFunctionParse():
            fn_call_parse_info = function_call_expression()
            if fn_call_parse_info == None:
                parser.rollback_checkpoint()
                parser.clear_checkpoint()
                raise RollbackTemporaryGeneratedCodes() 
            else:
                parse_result = fn_call_parse_info.function_call_metadata
                return_type = parse_result["return_type"]
                value = fn_call_parse_info.get_fn_str()

                parameter = value
                parameter_type = return_type
                fn_call_info = fn_call_parse_info
                if return_type == "char*":
                    parameter_type = ParameterType.STR_TYPE

                is_arithmatic_operation = False
                if parser.has_tokens_remaining():
                    symbols = {
                        lexer.Token.PLUS : "+",
                        lexer.Token.MINUS : "-",
                        lexer.Token.ASTERISK : "*",
                        lexer.Token.FRONT_SLASH : "/"
                    }

                    current_tk = parser.current_token()
                    if current_tk in symbols:
                        # This is an arithmatic expression. So, revert it and handle it below.
                        parser.rollback_checkpoint()
                        # parser.clear_checkpoint()
                        is_arithmatic_operation = True

                if not is_arithmatic_operation:
                    m_parameter = Parameter(parameter, parameter_type)

                    if "struct_instance" in parse_result:
                        m_parameter.struct_instance = parse_result["struct_instance"]

                    m_parameter.fn_call_parse_info = fn_call_info

                    return m_parameter
        
        parser.save_checkpoint()
        # string_expression = speculative_parse_string_expression()
        # if string_expression != None:
        #     parameter = string_expression.speculative_expression_value
        #     parameter_type = ParameterType.NUMBER
        #     return Parameter(parameter, parameter_type)
        # else:
        #     parser.rollback_checkpoint()
        #     parser.clear_checkpoint()
        
        # parser.save_checkpoint()

        integer_expression = speculative_parse_integer_expression()
        if integer_expression != None:
            parameter = integer_expression.speculative_expression_value
            parameter_type = ParameterType.NUMBER
            return Parameter(parameter, parameter_type)
        else:
            parser.rollback_checkpoint()
            parser.clear_checkpoint()
        
        if is_variable(tk):
            parameter = tk
            if is_variable_str_type(tk):
                parameter_type = ParameterType.STR_TYPE
            elif is_variable_string_class(tk):
                parameter_type = ParameterType.STRING_CLASS
            elif is_variable_char_type(tk):
                parameter_type = ParameterType.CHAR_TYPE
            else:
                parameter_type = ParameterType.VARIABLE
            parser.next_token()
        else:
            if tk == "true":
                parameter = tk
                parameter_type = ParameterType.BOOLEAN_CONSTANT
                parser.next_token()
            elif tk == "false":
                parameter = tk
                parameter_type = ParameterType.BOOLEAN_CONSTANT
                parser.next_token()
            elif is_global_function(tk):
                parameter = tk
                parameter_type = ParameterType.FUNCTION_POINTER
                parser.next_token()
            else:
                pass
                # Debug
                # parser.rollback_checkpoint()
                # parser.clear_checkpoint()

        return Parameter(parameter, parameter_type)

    def _read_parameters_within_round_brackets():
        # Parse n number of parameters within two round brackets.
        # (param1)
        # (param1,param2...paramN)
        parameters = []

        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
        while parser.has_tokens_remaining():
            curr_param = parser.current_token()
            if curr_param == lexer.Token.RIGHT_ROUND_BRACKET:
                break

            parameters.append(_read_a_parameter())

            if not parser.has_tokens_remaining():
                RAISE_ERROR("Expected ',' or ')' while parsing parameters for function call.")

            curr_param = parser.current_token()
            if curr_param != lexer.Token.RIGHT_ROUND_BRACKET:
                # There are few other tokens rather than ).
                # That should be a comma to indicate next parameter.
                parser.consume_token(lexer.Token.COMMA)
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        return parameters
    
    def Speculative_parse_struct_member_access(tk, base_struct_info, child_struct_info, member_access_string, pointer_access):
        struct_instance = None

        # let expr = A.B.C.Function()
        #            | ^ ^ child struct
        #            ^ base struct

        # let expr = A.Function()
        #             ^
        # In this case, 'A' is instanced struct, so we can get its defination.
        # We haven't set 'base_struct_info' so, we can make this check to know
        # that this is the base struct.
        if base_struct_info is None:
            struct_instance = get_instanced_struct(tk)
            if struct_instance is None:
                raise ParseErrorException(f"{tk} isn't an instanced struct.")
            base_struct_info = struct_instance.get_struct_defination()
            child_struct_info = base_struct_info

            if struct_instance.is_pointer_type:
                member_access_string = f"{tk}"
                pointer_access = "->"
            else:
                member_access_string = f"&{tk}"
                pointer_access = "."
        else:
            type_of_tk = child_struct_info.get_type_of_member(tk)
            if type_of_tk is None:
                RAISE_ERROR(f"Struct doesn't have member {tk}.")

            val = random.randrange(100000)

            type_of_tk = type_of_tk.replace("*","")

            struct_instance = StructInstance(type_of_tk, f"tmp_struct_name_{str(val)}", get_current_scope())

            child_struct_info = struct_instance.get_struct_defination()

            member_access_string += f"{pointer_access}{tk}"
            pointer_access = "."
        return base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance

    def generate_add_string_expression_code_For_simple_AST(target_variable_name, ast):
		# let a = b + c + d
		#			^^^^^^^
		# Convert this to:
		# a.__add__(c)
		# a.__add__(d)
		# Parse the string expression to extract components
        
        if not ast.params:
            return []
        
        add_method = "__add__"
        allocate_memory_method = "_allocate_more"
        
        generated_code_lines = []
        memory_allocation_lines = []
        length_expressions = []

        global temp_string_object_variable_count
        global instanced_struct_names

        for i, (string_value, string_type) in enumerate(ast.params):
            if string_type == ParameterType.RAW_STRING:
                # "string"
                generated_code_lines.append(f"{target_variable_name}.{add_method}(\"{string_value}\")")
                length_expressions.append(str(len(string_value)))
            elif string_type == ParameterType.STRING_EXPRESSION:
                temp_string_var_name = f"tmp_string_{temp_string_object_variable_count}"
                temp_string_object_variable_count += 1
                instance = StructInstance("String", f"{temp_string_var_name}", get_current_scope())
                # instance.is_pointer_type = True
                # instance.should_be_freed = False
                instanced_struct_names.append(instance)
                code_generator.emit_variable_declaration(
                    variable_type= "struct String",
                    variable_name= temp_string_var_name,
                    initialization_value= string_value
                )
                REGISTER_VARIABLE(f"{temp_string_var_name}", f"String")

                ast.update_param(i, temp_string_var_name, ParameterType.STRING_CLASS)

                # Write the new temporary instead.
                generated_code_lines.append(f"{target_variable_name}.{add_method}({temp_string_var_name})")
                length_expressions.append(f"{temp_string_var_name}.len()")
            else:
				# String objects.
                generated_code_lines.append(f"{target_variable_name}.{add_method}({string_value})")
                length_expressions.append(f"{string_value}.len()")

		# Compute total extra memory required if more than one component,
        # i.e if only one String object is added, than that add will perform reallocation itself.
        if len(length_expressions) > 1:
            total_length_expression = " + ".join(length_expressions)

            memory_allocation_lines = [
				f"{target_variable_name}.{allocate_memory_method}({total_length_expression})"
			]

		# Combine allocation and string addition code
        all_generated_lines = []
        all_generated_lines.extend(memory_allocation_lines)
        all_generated_lines.extend(generated_code_lines)
        return all_generated_lines

    def parse_access_struct_member(var_name):
        # let str = str2[0]
        #     ^^^   ^^^^ ^
        #     |     |    |
        #     |     |    .________ parameters
        #     |     ._____________ target
        #     .___________________ varname

        tk_copy_tmp = copy.deepcopy(parser.tokens)

        term = parse_term()
        term_parameters = term["parameter"]

        global instanced_struct_names
        global temp_string_object_variable_count    

        term_type = term['type']

        fn_call_parse_info = term_parameters.fn_call_parse_info
        if fn_call_parse_info != None:
            parse_result = fn_call_parse_info.function_call_metadata
            fn_name = parse_result["fn_name"]
            return_type = parse_result["return_type"]
            is_return_type_ref_type = parse_result["is_return_type_ref_type"]
            has_parameters = parse_result["has_parameters"]
            parsed_fn_call_type = parse_result["function_call_type"]
            member_access_string = parse_result["member_access_string"]

            if return_type.startswith("struct ") or is_data_type_struct_object(return_type):
                raw_return_type = return_type
                if return_type.startswith("struct "):
                    #              "struct Vector__String"
                    # return type   ^^^^^^^^^^^^^^^^^^^^^
                    # raw_return_type      ^^^^^^^^^^^^^^
                    raw_return_type = return_type[len("struct "):]

                instance = StructInstance(raw_return_type, var_name, get_current_scope())

                if is_return_type_ref_type or parsed_fn_call_type == ParsedFunctionCallType.MEMBER_ACCESS_CALL:
                    instance.should_be_freed = False
                
                instanced_struct_names.append(instance)

            REGISTER_VARIABLE(var_name, return_type)

            assignment_code = ""
            if parsed_fn_call_type == ParsedFunctionCallType.MEMBER_ACCESS_CALL:
                assignment_code = f"{format_struct_data_type(return_type)} {var_name} = {member_access_string};"
            else:
                assignment_code = f"{return_type} {var_name} = {fn_name}("

                if parsed_fn_call_type == ParsedFunctionCallType.STRUCT_FUNCTION_CALL:
                    assignment_code += f"{member_access_string}"
                    if has_parameters:
                        assignment_code += ","

                if has_parameters:
                    parameters_str = parse_result["parameters_str"]
                    assignment_code += f"{parameters_str}"
                assignment_code += ");"

            emit(f"{assignment_code}\n")
            return
        elif term_type == ParameterType.STRING_CLASS:
            # Creation of a string variable from another string variable.
            # let str1 = str2
            src_var_name = term["value"]

            ANIL_code = [f"let {var_name} = String{{{src_var_name}}};\n"]

            parameters = term["parameter"]

            ast = parameters.ast
            if ast.params:
                # Initialize the string using the first string in the AST.
                first_string_param_name, first_string_param_type = ast.params[0]
                ANIL_code = [f"let {var_name} = String{{{first_string_param_name}}};\n"]

                str_temporarires = []
                new_ast = []
                for i, (ast_p_name, ast_p_type) in enumerate(parameters.ast.params):
                    if ast_p_type == ParameterType.STRING_EXPRESSION:
                        temp_string_var_name = f"tmp_string_{temp_string_object_variable_count}"
                        temp_string_object_variable_count += 1
                        instance = StructInstance("String", f"{temp_string_var_name}", get_current_scope())
                        # instance.is_pointer_type = True
                        # instance.should_be_freed = False
                        instanced_struct_names.append(instance)

                        code_generator.emit_variable_declaration(
                            variable_type= "struct String",
                            variable_name= temp_string_var_name,
                            initialization_value= ast_p_name
                        )
                        REGISTER_VARIABLE(f"{temp_string_var_name}", f"String")
                        str_temporarires.append(temp_string_var_name)
                        new_ast.append((temp_string_var_name ,ParameterType.STRING_CLASS))
                    elif ast_p_type == ParameterType.RAW_STRING:
                        new_ast.append((ast_p_name, ParameterType.RAW_STRING))
                    else:
                        str_temporarires.append(ast_p_name)
                        new_ast.append((ast_p_name ,ParameterType.STRING_CLASS))
                joined = "+".join(str_temporarires)

                first_string_param_name, _type = new_ast[0]
                ANIL_code = [f"let {var_name} = String{{{first_string_param_name}}};\n"]
                del new_ast[0]

                new_simple_ast = SimpleAst()
                for name, _type in new_ast:
                    new_simple_ast.add_param(name, _type)

                # We used the first string to initialize already existing string,
                # so, remaining strings generate add expressions.
                parameters.ast.remove_param(0)

                ANIL_code.extend(generate_add_string_expression_code_For_simple_AST(var_name, new_simple_ast))

            insert_intermediate_lines(index, ANIL_code)
            return            

        parser.tokens = tk_copy_tmp

        fn_call_parse_info = None
        with SpeculativeFunctionParse():
            fn_call_parse_info = function_call_expression()
            if fn_call_parse_info == None:
                raise RollbackTemporaryGeneratedCodes()
            
        if fn_call_parse_info == None:
            # If it wasn't a function call assignment,
            # then, try to parse a variable assignment.
            term = parse_term()
            term_type = term['type']

            if term_type == ParameterType.STRING_CLASS:
                # Creation of a string variable from another string variable.
                # let str1 = str2
                src_var_name = term["value"]

                ANIL_code = [f"let {var_name} = String{{{src_var_name}}};\n"]

                parameters = term["parameter"]

                ast = parameters.ast
                if ast.params:
                    # Initialize the string using the first string in the AST.
                    first_string_param_name, first_string_param_type = ast.params[0]
                    ANIL_code = [f"let {var_name} = String{{{first_string_param_name}}};\n"]

                    # We used the first string to initialize already existing string,
                    # so, remaining strings generate add expressions.
                    parameters.ast.remove_param(0)

                    ANIL_code.extend(generate_add_string_expression_code_For_simple_AST(var_name, parameters.ast))

                insert_intermediate_lines(index, ANIL_code)
                return
            elif term_type != ParameterType.UNDEFINED:
                RAISE_ERROR(f"Variable assignment unimplemented for {term}.")

            RAISE_ERROR("Error in parsing variable assignment.")

        parse_result = fn_call_parse_info.function_call_metadata
        fn_name = parse_result["fn_name"]
        return_type = parse_result["return_type"]
        is_return_type_ref_type = parse_result["is_return_type_ref_type"]
        has_parameters = parse_result["has_parameters"]
        parsed_fn_call_type = parse_result["function_call_type"]
        member_access_string = parse_result["member_access_string"]

        if return_type.startswith("struct ") or is_data_type_struct_object(return_type):
            raw_return_type = return_type
            if return_type.startswith("struct "):
                #              "struct Vector__String"
                # return type   ^^^^^^^^^^^^^^^^^^^^^
                # raw_return_type      ^^^^^^^^^^^^^^
                raw_return_type = return_type[len("struct "):]

            instance = StructInstance(raw_return_type, var_name, get_current_scope())

            if is_return_type_ref_type or parsed_fn_call_type == ParsedFunctionCallType.MEMBER_ACCESS_CALL:
                instance.should_be_freed = False
            
            instanced_struct_names.append(instance)

        REGISTER_VARIABLE(var_name, return_type)

        assignment_code = ""
        if parsed_fn_call_type == ParsedFunctionCallType.MEMBER_ACCESS_CALL:
            assignment_code = f"{format_struct_data_type(return_type)} {var_name} = {member_access_string};"
        else:
            assignment_code = f"{return_type} {var_name} = {fn_name}("

            if parsed_fn_call_type == ParsedFunctionCallType.STRUCT_FUNCTION_CALL:
                assignment_code += f"{member_access_string}"
                if has_parameters:
                    assignment_code += ","

            if has_parameters:
                parameters_str = parse_result["parameters_str"]
                assignment_code += f"{parameters_str}"
            assignment_code += ");"

        emit(f"{assignment_code}\n")

    def parse_macro(parsed_member, p_macro_type, p_macro_fn_name=""):
        # p_macro_type -> NORMAL_MACRO | CLASS_MACRO

        # For normal macros:
        # add_token_raw "="
        # ^^^^^^^^^^^^^ parsed_member

        # For class macros:
        # values.pushn 40 50 60
        #        ^^^^^ p_macro_fn_name
        # ^^^^^^ parsed_member

        macro_def = None

        if p_macro_type == "NORMAL_MACRO":
            macro_def = get_macro_def_by_name(parsed_member)
        elif p_macro_type == "CLASS_MACRO":
            struct_type = get_struct_type_of_instanced_struct(parsed_member)
            macro_def = get_macro_def_by_name_of_class(p_macro_fn_name, struct_type)

        macro_has_parameter = macro_def.first_param != ""

        # add_token_raw "="
        parser.next_token()

        # Get the remaining tokens
        remaining_tokens = ""
        if p_macro_type == "NORMAL_MACRO":
            remaining_tokens = Line.split(parsed_member)[1]
        elif p_macro_type == "CLASS_MACRO":
            remaining_tokens = Line.split(p_macro_fn_name)[1]

        remaining_tokens = remaining_tokens.strip()

        # Remove any spaces in between the supplied arguments.
        # FIXME : If we have used the Parser to parse, we didn't need to do such workarounds.
        params_uncleaned = remaining_tokens
        params_uncleaned = params_uncleaned.split(" ")

        # Keep only the parameters, which are not (just spaces or 'n' number of spaces which may have appeared during splitting process).
        params = [param for param in params_uncleaned if param.strip() != ""]
        parameters_provided = len(params) > 0

        if macro_has_parameter:
            if not parameters_provided:
                RAISE_ERROR(
                    f'"{parsed_member}" macro expects an argument but provided None.'
                )
        else:
            if parameters_provided:
                RAISE_ERROR(
                    f'"{parsed_member}" macro doesn\'t expect an argument but provided {len(params)} parameters.'
                )

        # Write the macro name by replacing the fn parameter.
        code = ""

        code = macro_def.fn_body

        if p_macro_type == "CLASS_MACRO":
            # def pushn:
            #   self.push ..
            # a.pushn .. -> a.push ..
            code = code.replace("self", parsed_member)

        # X.size for init_list
        if macro_def.first_param_initializer_list:
            # def f X...
            # X.size -> this is replaced by the number of arguments passed to the macro call.
            # end def
            size_str = f"{macro_def.first_param}.size"
            if size_str in code:
                # print(params)
                length = str(len(params))
                code = code.replace(size_str, length)

        if macro_has_parameter:
            code = code.replace(macro_def.first_param, remaining_tokens)
        code_list = code.split("\n")

        new_code = []
        for line in code_list:
            if line == "":
                continue

            constexpr_parser = Parser.Parser(line)
            if not constexpr_parser.has_tokens_remaining():
                # The line == "" check above should handle this case,but whynot.
                continue

            if constexpr_parser.current_token() == "forall":
                constexpr_parser.next_token()

                # forall x : values.push_unchecked x
                #        ^
                #         loop_variable
                loop_variable = constexpr_parser.get_token()

                # forall annotation_argument_value, annotated_fn_name in annotated_functions_by_name(route)
                #        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                #                                                are stored in variables_list.
                variables_list = [loop_variable]

                while True:
                    if constexpr_parser.check_token(lexer.Token.COMMA):
                        constexpr_parser.consume_token(lexer.Token.COMMA)
                        var = constexpr_parser.get_token()
                        variables_list.append(var)
                    else:
                        break

                # forall x : values.push_unchecked x
                #          ^ in this case(forall x : ...), 
                # x(params) has all the parameters passed to the macro. 
                m_params = params

                code_to_write = ""

                # forall x : values.push_unchecked x
                #          ^
                # forall x in members_of(Vector)
                #          ^
                if constexpr_parser.check_token(lexer.Token.COLON):
                    constexpr_parser.consume_token(lexer.Token.COLON)

                    # forall x : values.push_unchecked x
                    #          ^ ------------------------ code_to_write
                    code_to_write = line[line.find(":") + 1:]
                elif constexpr_parser.check_token(lexer.Token.IN):
                    # forall x in members_of(Vector):
                    #          ^
                    constexpr_parser.consume_token(lexer.Token.IN)

                    # forall x in members_of(Vector):
                    #             ^
                    function_name = constexpr_parser.get_token()

                    # forall x in members_of(Vector):
                    #                       ^
                    constexpr_parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
                    class_name = constexpr_parser.get_token()
                    constexpr_parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)
                    
                    should_quote = True
                    # forall x in instances_of_class(List) UNQUOTE: x.print()
                    #                                      ^
                    has_quote_token = constexpr_parser.current_token() == "QUOTE"
                    has_unquote_token = constexpr_parser.current_token() == "UNQUOTE"
                    if has_quote_token or has_unquote_token:
                        constexpr_parser.next_token()
                        if has_unquote_token:
                            should_quote = False
                                        
                    def stringify(p_str):
                        if should_quote:
                            return '"' + p_str + '"'
                        else:
                            return p_str

                    constexpr_parser.consume_token(lexer.Token.COLON)

                    # forall x: values.push_unchecked x
                    #           ^^^^^^^^^^^^^^^^^^^^^^^
                    #           code_to_write
                    code_to_write = line[line.find(":") + 1:]

                    struct_def = None
                    
                    if function_name == "annotated_functions_by_name":
                        # This constexpr function doesn't access struct information.
                        pass
                    else:
                        struct_def = get_struct_defination_of_type(class_name)
                        if struct_def == None:
                            RAISE_ERROR(f"{class_name} class isn't registered.")
                    
                    def _get_members():
                        return [stringify(member.member) for member in struct_def.members]

                    def _get_member_functions():
                        # FIXME: Now templated classes dont store functions definations in member_functions.
                        # Instead they are stored in 'unparsed_functions' as raw text.
                        # For templated classes extract function names using that.
                        return [stringify(fn.fn_name) for fn in struct_def.member_functions]

                    def _get_instances():
                        global instanced_struct_names
                        return [stringify(struct.struct_name) 
                                for struct in instanced_struct_names 
                                if struct.struct_type == class_name]
                    
                    def _get_annotated_functions_by_name():
                        global annotations_list

                        item_list = []
                        for annotated_item in annotations_list:
                            if annotated_item.annotation_name == class_name:
                                fn_name = stringify(annotated_item.annotated_fn_name)
                                args = [stringify(args) for args in annotated_item.annotation_argument_values] 
                                # The args_list is [fn_name, arg1, arg2, ... argn]
                                args_list = [fn_name]
                                args_list.extend(args)
                                item_list.append(args_list)
                        return item_list

                    constexpr_map: Dict[str, Callable] = {
                        "members_of": _get_members,
                        "member_functions_of": _get_member_functions,
                        "instances_of_class": _get_instances,
                        "annotated_functions_by_name": _get_annotated_functions_by_name,
                    }
            
                    if function_name in constexpr_map:
                        fn_names = constexpr_map[function_name]()
                        m_params = fn_names
                    else:
                        error_msg = f"\"{function_name}\" is not a constexpr function. \n"
                        error_msg += "Only the following functions are constexpr functions:\n"
                        for names in constexpr_map.keys():
                            error_msg += f"{names}\n"
                        RAISE_ERROR(f"{error_msg}")

                if len(variables_list) == 1:
                    for param in m_params:
                        # values.push_unchecked x
                        #                       ^ loop_variable
                        # ^^^^^^^^^^^^^^^^^^^^^^^ code_to_write
                        # Our m_params could be 10, 20, 30(which are passed while calling the macro).
                        # For all the param in m_params, rewrite 'code_to_write' with the 'param'.
                        # so, the generated code will be like,
                        # values.push_unchecked(10)
                        # values.push_unchecked(20)
                        # values.push_unchecked(30)
                        gen_code = code_to_write.replace(loop_variable, param) + "\n"
                        new_code.append(gen_code)
                    new_code.append("\n")
                else:
                    for param in m_params:
                        if len(variables_list) != len(param):
                            RAISE_ERROR(f"Macro returns {len(param)} values but {len(variables_list)} variables are unpacked.")
                        gen_code = code_to_write
                        for count, var in enumerate(variables_list):
                            gen_code = gen_code.replace(var, param[count]) + "\n"
                        new_code.append(gen_code)
                    new_code.append("\n")
            else:
                new_code.append(line)

        insert_intermediate_lines(index, new_code)

    def parse_constexpr_dictionary(p_dict_name):
        parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
        key = parser.extract_string_literal()
        parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

        for dict in constexpr_dictionaries:
            if dict.dict_name == p_dict_name:
                try:
                    if dict.dict_type == "number":
                        return dict.dictionary[key]
                    else:
                        RAISE_ERROR(f"TODO: To be implemented: Constexpr dictionary {p_dict_name} string lookups. String lookups are only allowed for Let statements.")
                except KeyError:
                    print(
                        f'[Error] Key "{key}" wasn\'t found in the constexpr dictionary {p_dict_name}.'
                    )
        # Shouldn't happen as the caller functions have already verified the presence of the dictionaries.
        RAISE_ERROR(f"Constexpr dictionary {p_dict_name} is undefined.")

    def parse_string_constexpr_dictionary(p_dict_name):
        parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)

        # a["key"]
        key = None
        is_raw_string = False
        if parser.check_token(lexer.Token.QUOTE):
            key = parser.extract_string_literal()
            is_raw_string = True
        else:
            key = parser.get_token()

        parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

        fn_name = f"Constexpr__Lookup__{p_dict_name}"
        if fn_name not in constexpr_lookup_functions_generated:
            constexpr_lookup_functions_generated.append(fn_name)
            m_dict = get_constexpr_dictionary(target)
            fn_declaration, fn_body = generate_contexpr_dict_runtime_lookup_code(fn_name, m_dict)

            global GlobalGeneratedFunctionDeclarations
            GlobalGeneratedFunctionDeclarations += f"{fn_declaration};\n"

            global GlobalGeneratedConstexprLookupFnDefinations
            GlobalGeneratedConstexprLookupFnDefinations += fn_body

        for d in constexpr_dictionaries:
            if d.dict_name != p_dict_name:
                continue

            if key in d.dictionary:
                value = d.dictionary[key]
            else:
                value = d.default_key_value

            if is_raw_string:
                return True, value
                #      ^^^^   we have constexpr string.
            else:
                return False, f"{fn_name}(&{key})"
                #      ^^^^^  we need to perform runtime lookup.

        # Shouldn't happen as the caller functions have already verified the presence of the dictionaries.
        RAISE_ERROR(f"Constexpr dictionary {p_dict_name} is undefined.")

    def get_constexpr_dictionary(p_dict_name):
        for dict in constexpr_dictionaries:
            if dict.dict_name == p_dict_name:
                return dict
        # Shouldn't happen as the caller functions have already verified the presence of the dictionaries.
        RAISE_ERROR(f"Constexpr dictionary {p_dict_name} is undefined.")

    def get_constexpr_dictionary_type(p_dict_name):
        for dict in constexpr_dictionaries:
            if dict.dict_name == p_dict_name:
                try:
                    return dict.dict_type
                except KeyError:
                    print(
                        f'[Error] Key "{key}" wasn\'t found in the constexpr dictionary {p_dict_name}.'
                    )
        # Shouldn't happen as the caller functions have already verified the presence of the dictionaries.
        RAISE_ERROR(f"Constexpr dictionary {p_dict_name} is undefined.")

    def parse_function_declaration(incomplete_types = None):
        # parse everything after function/c_function token.
        # function<> append<>(p_value : int) -> return_type
        #         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

        is_overloaded_fn = False
        overload_for_type = ""

        # c_function Vector<String> __contains__(value : T) -> bool:
        #            ^^^^^^^^ This is custom overload for __contains__ for the template type 'String'.
        if parser.current_token() == lexer.Token.SMALLER_THAN:
            parser.next_token()

            # c_function Vector< > __contains__(value : T)-> bool
            #             ^ indicates an overloaded function(the base overload)
            curr_token = parser.current_token()
            custom_template_type = "#BASE#"
            if curr_token == lexer.Token.GREATER_THAN:
                pass
            else:
                custom_template_type = parser.get_token()
            # print(f"Custom template for class {custom_template_type}")
            overload_for_type = custom_template_type
            is_overloaded_fn = True

            parser.consume_token(lexer.Token.GREATER_THAN)

        fn_name = parser.get_token()

        return_type = "void"

        parameters = []
        parameters_combined_list = []

        # function append<>(p_value : int)
        #                <> after function name indicate this is a templated function.
        # TODO : This logic should not be required.
        # We need this hint so that when we write mangled function name immediately,
        # we know that we should use overloaded fn name mangling convention.
        # Otherwise, the first fn will have default name mangling convention,
        # and, other overloaded fns will use overloaded fn name mangling.
        is_overloaded = False
        if parser.check_token(lexer.Token.SMALLER_THAN):
            parser.consume_token(lexer.Token.SMALLER_THAN)
            is_overloaded = True
            parser.consume_token(lexer.Token.GREATER_THAN)

        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

        while parser.has_tokens_remaining():
            # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
            #                                                               ^
            if parser.check_token(lexer.Token.RIGHT_ROUND_BRACKET):
                # function say()
                #              ^
                break

            # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
            #              ^
            param_name = parser.get_token()

            # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
            #                     ^
            parser.consume_token(lexer.Token.COLON)

            # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
            #                       ^
            param_type = parse_data_type()
            # print(f"Function Param Name : {param_name}, type : {param_type}")

            if "(*#FNNAME#)" in param_type:
                # "Function Pointer Implementation" Section. 
                # TODO: This is a temporary solution to handle function pointers.
                # Probably DataType class should be refactored to handle function pointers.
                # FIXME: Investigate.
                # This section is executed after 'parse_function_pointer()'.
                # So, investigate that function to see how it is being handled.
                # Fn(int, int) -> void
                # (*callback)(UIElement*, void*)
                #   ^^^^^^^^ #FNNAME#
                # return f"{return_type} (*#FNNAME#)({', '.join(arg_type_list)})"
                param_type = param_type.replace("#FNNAME#", param_name)
                param_name = ""

            if parser.has_tokens_remaining():
                # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
                #                            ^
                # function say(Param1 : type1)
                #                            ^
                # If not reached the closing ), then expect a comma, to read another parameter.

                if not parser.check_token(lexer.Token.RIGHT_ROUND_BRACKET):
                    parser.consume_token(lexer.Token.COMMA)

            parameters.append(MemberDataType(param_type, param_name, False))
            parameters_combined_list.append(f"{param_type} {param_name}")

        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        is_return_type_ref_type = False
        # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN) -> return_type
        if parser.has_tokens_remaining():
            if parser.check_token(lexer.Token.MINUS):
                parser.consume_token(lexer.Token.MINUS)
                parser.consume_token(lexer.Token.GREATER_THAN)
                # -> &return_type
                if parser.check_token(lexer.Token.AMPERSAND):
                    is_return_type_ref_type = True
                    parser.next_token()

                return_type = parse_data_type(False, incomplete_types)
                parser.consume_token(lexer.Token.COLON)
            elif parser.check_token(lexer.Token.COLON):
                # function next_token() :
                #                       ^ not needed.
                RAISE_ERROR(f'Function syntax is "function {fn_name}() -> return_type:" or "function {fn_name}()". No need of colon after "()".')

        return {
            "fn_name" : fn_name,
            "return_type" : return_type,
            "is_return_type_ref_type" : is_return_type_ref_type,
            "parameters" : parameters,
            "parameters_combined_list" : parameters_combined_list,
            "is_overloaded" : is_overloaded,
            "is_overloaded_fn" : is_overloaded_fn,
            "overload_for_type" : overload_for_type,
        }

    if is_inside_GUI_code:
        # JSX like parsing.
        # Parse :
        #   <Label>"Label Text"</Label>
        #   <Button>"Button Text"</Button>
        #   <Label>"Label Text"</Label>

        if check_token(lexer.Token.SMALLER_THAN):
            parser.consume_token(lexer.Token.SMALLER_THAN)

            # </UI>
            #  ^
            if parser.check_token(lexer.Token.FRONT_SLASH):
                parser.consume_token(lexer.Token.FRONT_SLASH)
                # </UI>
                #   ^^
                node_name = parser.get_token()
                parser.consume_token(lexer.Token.GREATER_THAN)

                JSXlike_element_tree.close_tag(node_name)
                continue

            # <Label>"Label Text"</Label>
            #  ^^^^^
            tag_name = parser.get_token()

            ui_Element = UIElement(tag_name)

            # <Label id="headerLabel">
            #        ^
            while parser.has_tokens_remaining():
                if parser.check_token(lexer.Token.GREATER_THAN):
                    break
                
                # <Label id="headerLabel">
                #        ^^ 
                attribute = parser.get_token()

                parser.consume_token(lexer.Token.EQUALS)

                # <Label id="headerLabel">
                #           ^^^^^^^^^^^^^
                value = parser.extract_string_literal()

                # print(f"Attribute : {attribute}, Value : \"{value}\"")

                ui_Attribute = UIAttribute(attribute, value)
                ui_Element.add_attribute(ui_Attribute)

            # <Label>"Label Text"</Label>
            #       ^
            parser.consume_token(lexer.Token.GREATER_THAN)

            closing_tag_found = False
            if parser.has_tokens_remaining():
                # <HBox id="inputRow">
                #                     ^
                #   <Input id="todoInput"></Input>

                # <Label>"Label Text"</Label>
                #        ^^^^^^^^^^^^
                if parser.check_token(lexer.Token.QUOTE):
                    node_content = parser.extract_string_literal()
                    ui_Element.set_content(node_content)

                # <Label>"Label Text"</Label>
                #                    ^
                parser.consume_token(lexer.Token.SMALLER_THAN)
                parser.consume_token(lexer.Token.FRONT_SLASH)

                closing_tag_name = parser.get_token()
                if tag_name != closing_tag_name:
                    RAISE_ERROR(f"Closing tag {closing_tag_name} doesn't match with opening tag {tag_name}.")
                parser.consume_token(lexer.Token.GREATER_THAN)
                closing_tag_found = True

            JSXlike_element_tree.add_element(ui_Element)

            if closing_tag_found:
                JSXlike_element_tree.close_tag(closing_tag_name)
        else:
            RAISE_ERROR(f"Expected '<' i.e JSX like UI Syntax but got {Line}.")

    if is_inside_def and "enddef" in Line:
        if check_token(lexer.Token.ENDDEF):
            is_inside_def = False

            macro_def = MacroDefination(
                currently_reading_def_name,
                currently_reading_def_parameter,
                currently_reading_def_body,
            )
            macro_def.first_param_initializer_list = (
                currently_reading_def_paramter_initializer_list
            )

            if currently_reading_def_macro_type == "NORMAL_MACRO":
                MacroDefinations.append(macro_def)
            elif currently_reading_def_macro_type == "CLASS_MACRO":
                struct_defination = get_struct_defination_of_type(
                    currently_reading_def_target_class
                )
                if struct_defination is None:
                    RAISE_ERROR(
                        f"Struct type {currently_reading_def_target_class} doesn't exist."
                    )
                else:
                    struct_defination.macro_definations.append(macro_def)

            currently_reading_def_name = ""
            currently_reading_def_parameter = ""
            currently_reading_def_macro_type = "NORMAL_MACRO"
            currently_reading_def_target_class = ""
            currently_reading_def_paramter_initializer_list = False
            currently_reading_def_body = ""
        continue
    
    def speculative_parse_number():
        current_token = parser.current_token()
        
        is_negative = False
        if current_token == lexer.Token.MINUS:
            parser.next_token()
            is_negative = True

            current_token = parser.current_token()

        return_value = None
        token_type = SpeculativeTokenType.NONE
        exact_type = ""

        if is_numeric_constexpr_dictionary(current_token):
            parser.next_token()
            return_value = parse_constexpr_dictionary(current_token)
            exact_type = "int"
            token_type = SpeculativeTokenType.NUMERIC_CONSTANT
        elif is_variable_int_type(current_token):
            return_value = current_token
            exact_type = "int"
            token_type = SpeculativeTokenType.NUMERIC_VARIABLE
            parser.next_token()
        elif is_variable_size_t_type(current_token):
            return_value = current_token
            exact_type = "size_t"
            token_type = SpeculativeTokenType.NUMERIC_VARIABLE
            parser.next_token()
        else:
            try:
                if current_token.isdigit():
                    return_value = current_token
                    exact_type = "int"
                    token_type = SpeculativeTokenType.NUMERIC_CONSTANT
                    parser.next_token()
            except AttributeError:
                # When the current_token is not string, current_token.isdigit() will raise AttributeError.
                pass

        if return_value == None:
            # Nothing is number as of now.
            parser.save_checkpoint()

            with SpeculativeFunctionParse():
                fn_call_parse_info = function_call_expression()
                if fn_call_parse_info == None:
                    parser.rollback_checkpoint()
                    # parser.clear_checkpoint()
                    raise RollbackTemporaryGeneratedCodes()
                else:
                    parse_result = fn_call_parse_info.function_call_metadata
                    return_type = parse_result["return_type"]
                    if return_type == "int" or return_type == "size_t":
                        return_value = fn_call_parse_info.get_fn_str()
                    else:
                        parser.rollback_checkpoint()
                        # parser.clear_checkpoint()
                        raise RollbackTemporaryGeneratedCodes()

        if is_negative:
            if return_value == None:
                RAISE_ERROR(f"Expected some numeric value, but got {current_token}.")
            return_value = '-' + return_value

        if return_value is not None:
            parse_info = SpeculativeParseInfo()
            parse_info.speculative_token_type = token_type
            parse_info.speculative_exact_type = exact_type
            parse_info.speculative_token_value = return_value
            return parse_info
        return None

    def speculative_parse_string():
        return_value = None
        token_type = SpeculativeTokenType.NONE
        exact_type = ""

        # Save the state of tokens before we attempt any matching or consumption.
        parser.save_checkpoint()

        current_token = parser.current_token()
        if current_token == lexer.Token.QUOTE:
            # str = "Hello World"
            string = escape_quotes(parser.extract_string_literal())

            return_value = f"{string}"
            exact_type = "c_str"
            token_type = SpeculativeTokenType.STRING_LITERAL
        elif is_variable_char_type(current_token):
            return_value = current_token
            exact_type = "char"
            token_type = SpeculativeTokenType.CHAR_TYPE
            parser.next_token()
        elif is_variable_str_type(current_token):
            return_value = current_token
            exact_type = "str"
            token_type = SpeculativeTokenType.STR_TYPE
            parser.next_token()

        if return_value == None:
            # Nothing is string as of now.
            # We assume we might have consumed some tokens checking above (unlikely given logic, but safe),
            # or we need to start fresh for function call parsing.
            # However, since we haven't matched anything yet, we are still effectively at the checkpoint.
            
            with SpeculativeFunctionParse():
                fn_call_parse_info = function_call_expression()

                if fn_call_parse_info == None:
                    # Function parsing failed completely. 
                    # We must restore tokens to the state before we tried parsing the function.
                    parser.rollback_checkpoint()
                    
                    if is_variable_string_class(current_token):
                        return_value = current_token
                        exact_type = "struct String"
                        token_type = SpeculativeTokenType.STRING_CLASS
                        parser.next_token()
                        # Success fallback, clear the rollback checkpoint as we consumed successfully.
                        parser.clear_checkpoint()
                    else:
                        # Parsing failed.
                        # Do NOT clear tokens here. Just raise the exception.
                        # The context manager handles LineCache cleanup.
                        # The rollback_checkpoint above handled Token cleanup.
                        raise RollbackTemporaryGeneratedCodes()
                else:
                    parse_result = fn_call_parse_info.function_call_metadata
                    return_type = parse_result["return_type"]
                    
                    if return_type in {"struct String", "String"}:
                        return_value = fn_call_parse_info.get_fn_str()
                        token_type = SpeculativeTokenType.STRING_EXPRESSION
                        parser.clear_checkpoint() # Valid parse, discard checkpoint.
                    elif return_type in {"str", "char*"}:
                        return_value = fn_call_parse_info.get_fn_str()
                        token_type = SpeculativeTokenType.STR_TYPE
                        parser.clear_checkpoint() # Valid parse, discard checkpoint.
                    else:
                        # Parsed a function, but it wasn't a string type.
                        # Revert tokens so something else can try to parse it.
                        parser.rollback_checkpoint()
                        raise RollbackTemporaryGeneratedCodes()
        else:
            # We matched one of the simple types (Literal, Char var, Str var).
            # Discard the checkpoint as we want to keep the consumed tokens.
            parser.clear_checkpoint()

        if return_value is not None:
            parse_info = SpeculativeParseInfo()
            parse_info.speculative_token_type = token_type
            parse_info.speculative_exact_type = exact_type
            parse_info.speculative_token_value = return_value
            return parse_info
        
        return None

    def speculative_parse_string_expression():
        expr = ""

        ast_params = []

        while parser.has_tokens_remaining():
            parse_info = speculative_parse_string()
            if parse_info == None:
                break

            # a = 1
            tk = parse_info.speculative_token_value

            ast_param_name = tk
            ast_type = SpeculativeTokenTypeToParameterType(parse_info.speculative_token_type)
            ast_param = (ast_param_name, ast_type)
            ast_params.append(ast_param)

            expr += tk

            if parser.has_tokens_remaining():
                if parser.current_token() == lexer.Token.PLUS:
                    parser.consume_token(lexer.Token.PLUS)
                    expr += " + "
                    continue
            break
        
        if expr == "" and len(ast_params) == 0:
            # expr might be "" for "" string.
            # So, we add this ast_params == 0 check.
            return None
        else:
            expression_info = SpeculativeExpressionInfo()
            expression_info.speculative_expression_type = SpeculativeExpressionType.STRING_EXPRESSION
            expression_info.speculative_expression_value = expr
            
            for ast_param_name, ast_type in ast_params:
                expression_info.ast.add_param(ast_param_name, ast_type)
            
            return expression_info

    def speculative_parse_integer_expression():
        expr = ""

        while parser.has_tokens_remaining():
            parse_info = speculative_parse_number()
            if parse_info == None:
                break

            # a = 1
            tk = parse_info.speculative_token_value
            expr += tk

            if parser.has_tokens_remaining():
                # a = 1 + 1
                symbols = {
                    lexer.Token.PLUS : "+",
                    lexer.Token.MINUS : "-",
                    lexer.Token.ASTERISK : "*",
                    lexer.Token.FRONT_SLASH : "/"
                }

                current_tk = parser.current_token()
                if current_tk in symbols:
                    expr += symbols[current_tk]
                    parser.next_token()
                else:
                    break
                    # error_msg = f"Unexpected token \"{current_tk}\" in integer expression."
                    # error_msg += f"The following operators (+,-,*,/) are only supported."
                    # RAISE_ERROR(error_msg)
            else:
                break
        
        if expr == "":
            return None
        else:
            expression_info = SpeculativeExpressionInfo()
            expression_info.speculative_expression_type = SpeculativeExpressionType.NUMERIC_EXPRESSION
            expression_info.speculative_expression_value = expr
            return expression_info

    def get_integer_expression(msg = None):
        """
        Parses and retrieves an integer expression.

        This function attempts to speculatively parse an integer expression. 
        If the parsing fails, an error is raised with 
        the provided error message `msg`. If `msg` is not provided, a default error message 
        "Expected integer expression." is used.

        Args:
            msg (str, optional): A custom error message to be raised if the integer expression 
                                 is not successfully parsed. Defaults to None.
        """
        integer_expression = speculative_parse_integer_expression()
        if integer_expression == None:
            if msg is not None:
                RAISE_ERROR(msg)
            else:
                RAISE_ERROR("Expected integer expression.")
        return integer_expression.speculative_expression_value

    def function_call_expression():
        expr = ""

        #            V We start parsing from here. 
        # let expr = A.B.C.Function()
        #            | ^ ^ child struct
        #            ^ base struct
        # let expr = A.Function()
        # let expr = A.B.Function()
        # let expr = Function()
        #            ^^^^^^^^  fn_name

        fn_name_unmangled = ""
        return_type = ""
        is_return_type_ref_type = False

        member_access_string = ""

        pointer_access = "->"
        fn_name_mangled = ""

        args = None
        parameters = None

        base_struct_info = None
        child_struct_info = None

        # it is used to get mangled function name from a given instance.
        # We can't get proper mangled name from struct defination so we need StructInstance.
        struct_instance = None

        parser.save_checkpoint()

        def return_member_access_expression():
            nonlocal member_access_string, struct_instance, child_struct_info    
            if member_access_string[-1] != ">" or member_access_string[-1] != ".":
                member_access_string += f"{pointer_access}{tk}"

            if "->" in member_access_string and member_access_string[0] != "&":
                pass
                # Turns out this isn't required for MEMBER_ACCESS_CALL's.
                # TODO : Investigate :P
                # member_access_string = "&" + member_access_string

            # For member access, we dont need &.
            # For example: 'fruit.position' should be as it is and not '&fruit.position'.
            # However, 'this' is not member access and becomes '&this' as it should be.
            if member_access_string[0] == "&":
                member_access_string = member_access_string[1:]
            
            parsing_fn_call_type = ParsedFunctionCallType.MEMBER_ACCESS_CALL

            parser.next_token()

            type_of_tk = child_struct_info.get_type_of_member(tk)
            if type_of_tk is None:
                RAISE_ERROR(f"Struct doesn't have member {tk}.")

            val = random.randrange(100000)

            struct_instance = None
            if get_struct_defination_of_type(type_of_tk) != None:
                # If we are acessing say a.b where b is int, then we dont need to create an instance.
                struct_instance = StructInstance(
                    type_of_tk,
                    f"tmp_struct_name_{str(val)}",
                    get_current_scope(),
                )
            
            additional_data = {
                "fn_name": "",
                "return_type": type_of_tk,
                "is_return_type_ref_type" : False,
                "has_parameters": False,
                "parameters_str": "",
                "function_call_type": parsing_fn_call_type,
                "member_access_string": member_access_string,
                "struct_instance": struct_instance,
            }

            expression_info = SpeculativeExpressionInfo()
            expression_info.speculative_expression_type = SpeculativeExpressionType.FUNCTION_CALL_EXPRESSION
            expression_info.speculative_expression_value = ""
            expression_info.function_call_metadata = additional_data
            return expression_info
    
        is_class_static_fn_call = False
        class_name = ""
        if parser.has_tokens_remaining():
            tk = parser.current_token()    

            if is_class_name(tk):
                # Rectangle::test() where test is a static function call.
                if parser.check_n_tokens([tk, lexer.Token.COLON, lexer.Token.COLON]):
                    is_class_static_fn_call = True

                    class_name = tk

                    parser.next_token()  # Consume struct name
                    parser.consume_token(lexer.Token.COLON)
                    parser.consume_token(lexer.Token.COLON)

        while parser.has_tokens_remaining():
            tk = parser.current_token()    

            #For struct member access like in a.b
            #                                 ^^  is parsed earlier.
            # current tk is b & we have no tokens remaining.
            if (not parser.has_tokens_remaining()) or (len(parser.tokens) == 1):
                if child_struct_info is not None:
                    is_acessing_struct_member = child_struct_info.has_data_member(tk)
                    if is_acessing_struct_member:
                        return return_member_access_expression()

            if parser.check_token(lexer.Token.LEFT_ROUND_BRACKET):
                fn_name_unmangled = tk
                parameters = _read_parameters_within_round_brackets()
                break
            
            next_token = None
            try:
                # There is nothing further.
                # So likely it is a variable or a constant say a = 42.
                next_token = parser.peek_token()
            except IndexError:
                next_token = None
            if next_token == None:
                parser.rollback_checkpoint()
                # parser.clear_checkpoint()
                return None
            if next_token in [lexer.Token.DOT, lexer.Token.LEFT_ROUND_BRACKET,lexer.Token.LEFT_SQUARE_BRACKET]:
                parser.next_token()
            else:
                if child_struct_info is not None:
                    is_acessing_struct_member = child_struct_info.has_data_member(tk)
                    if is_acessing_struct_member:
                        return return_member_access_expression()

                # Probably invalid token
                parser.rollback_checkpoint()
                return None

            is_member_access_token = parser.check_token(lexer.Token.DOT)

            if parser.check_token(lexer.Token.LEFT_ROUND_BRACKET):
                fn_name_unmangled = tk
                parameters = _read_parameters_within_round_brackets()
                break
            elif is_member_access_token or parser.check_token(lexer.Token.LEFT_SQUARE_BRACKET):
                #            V ----------- tk
                # let expr = A.B.C.Function()
                #             ^

                # let expr = A.Function()
                #             ^
                # In this case, 'A' is instanced struct, so we can get its defination.
                # We haven't set 'base_struct_info' so, we can make this check to know
                # that this is the base struct.
                try:
                    base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance = Speculative_parse_struct_member_access(tk, base_struct_info, child_struct_info, member_access_string, pointer_access)
                except ParseErrorException as e:
                    # print(f"Caught an exception: {e}")
                    parser.rollback_checkpoint()
                    return None

                if is_member_access_token:
                    parser.consume_token(lexer.Token.DOT)
                    continue

                # For Dictionary Like items member acess.
                # let test = TOKEN_MAP["Option"]

                # let str = str2[0]
                #               ^
                fn_name_unmangled = "__getitem__"
                parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                parameters = [_read_a_parameter()]
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

                # [BEGIN] Automatic Conversion for String class
                is_parameter_string_object = False
                param_struct_info = get_instanced_struct(parameters[0].param)
                if param_struct_info != None:
                    is_parameter_string_object = param_struct_info.struct_type == "String"

                if is_parameter_string_object:
                    get_item_fn_args = param_struct_info.get_fn_arguments("__getitem__")
                    # Get the data type for the first function argument.
                    param_type = get_item_fn_args[0].data_type
                    expects_string_argument = param_type == "char*" or param_type == "str"

                    if expects_string_argument:
                        c_str_fn_name = param_struct_info.get_mangled_function_name("c_str")
                        # Recreate the obtained parameter as,
                        # Dictionary__getitem__(&CHARACTER_TOKENS, token)
                        #                                          ^^^^^
                        # to
                        # Dictionary__getitem__(&CHARACTER_TOKENS, Stringc_str(&token))
                        #                                          ^^^^^^^^^^^^^^^^^^^
                        m_param = f"{c_str_fn_name}(&{parameters[0].param})"
                        m_param_type = ParameterType.VARIABLE
                        #                            ^^^^^^^^^ Just a placeholder,nothing special.
                        param = Parameter(m_param, m_param_type)
                        parameters = [param]
                    args = get_item_fn_args
                # [END] Automatic Conversion for String class
                break

        parsing_fn_call_type = ParsedFunctionCallType.STRUCT_FUNCTION_CALL

        if base_struct_info == None:
            if is_class_static_fn_call:
                fn_name = fn_name_unmangled

                m_fn = get_class_static_function(class_name, fn_name)
                if m_fn == None:
                    RAISE_ERROR(f"Symbol {fn_name} is not a static function of class {class_name}.")

                args = m_fn.fn_arguments
                return_type = m_fn.return_type
                fn_name_mangled = get_mangled_fn_name(class_name, fn_name)
                parsing_fn_call_type = ParsedFunctionCallType.CLASS_STATIC_FUNCTION_CALL
            else:
                # let expr = Function()
                #            ^^^^^^^^
                # Just a global function call.
                global_fn_name = fn_name_unmangled
            
                m_fn = get_global_function_by_name(global_fn_name)
                if m_fn == None:
                    RAISE_ERROR(f"Symbol {global_fn_name} is not a global function.")

                args = m_fn.fn_arguments
                return_type = m_fn.return_type
                fn_name_mangled = global_fn_name
                parsing_fn_call_type = ParsedFunctionCallType.GLOBAL_FUNCTION_CALL
        else:
            provided_types = _parameters_to_types_str_list(parameters)
            
            args = struct_instance.get_fn_arguments(fn_name_unmangled, provided_types)
            return_type = struct_instance.get_return_type_of_fn(fn_name_unmangled, provided_types)
            is_return_type_ref_type = struct_instance.is_return_type_ref_type(fn_name_unmangled, provided_types)
            fn_name_mangled = struct_instance.get_mangled_function_name(fn_name_unmangled, provided_types)
            parsing_fn_call_type = ParsedFunctionCallType.STRUCT_FUNCTION_CALL
        fn_args = []
        if args is not None:
            fn_args = [arg.data_type for arg in args]

        if len(fn_args) != len(parameters):
            RAISE_ERROR(
                f'Expected {len(fn_args)} arguments for function "{fn_name_unmangled}" but provided {len(parameters)} arguments.'
            )

        parameters_quoted = _quote_string_params(fn_args, parameters)
        parameters = parameters_quoted

        # Promotion of char to char* when char is provided to a function that expects a char*.
        char_to_string_promotion_code = ""
        for i, (arg, parameter) in enumerate(zip(fn_args, parameters)):
            param = parameter.param
            param_type = parameter.param_type

            expects_string_argument = (arg == "char*") or (arg == "str")
            if expects_string_argument:
                if param_type == ParameterType.CHAR_TYPE:
                    # This new string replaces the old char param.
                    parameters[i].param = promote_char_to_string(param)
                elif param_type == ParameterType.STRING_CLASS:
                    param_struct_info = get_instanced_struct(param)
                    # Our Expression parser said this is STRING_CLASS,
                    # doesn't mean we got String object, we might have char* as well.
                    if param_struct_info is None:
                        if is_variable_str_type(param):
                            pass
                        else:
                            RAISE_ERROR("Error in conversion between strings for function parameters.")
                    else:
                        c_str_fn_name = param_struct_info.get_mangled_function_name("c_str")
                        # A function expects a string, & we provided a String class object.
                        # So, we perform getting string from String Class object.
                        # var -> Stringc_str(&var)
                        # ^^^ String Object.
                        parameters[i].param = f"{c_str_fn_name}(&{parameters[i].param})"

            # Automatic String literal to String Object conversion for function parameters which expect String object.
            if arg == "struct String" and param_type == ParameterType.RAW_STRING:
                # For f(a:String){..} we provided f("constant string literal");
                #         ^^^^^^ arg                 ^^^^^^^^^^^^^^^^^^^^^^^ param_type
                #
                global temp_string_object_variable_count    
                temp_string_var_name = f"tmp_string_{temp_string_object_variable_count}"
                temp_string_object_variable_count += 1
                instance = StructInstance("String", f"{temp_string_var_name}", get_current_scope())
                # instance.is_pointer_type = True
                instance.should_be_freed = False
                instanced_struct_names.append(instance)

                emit(f"struct String {temp_string_var_name};\n")
                string_len_without_quotes = len(param) - 2
                #                                                                  v the param already has quotes.
                emit(f"Stringinit__STATIC__(&{temp_string_var_name}, {param}, {string_len_without_quotes});\n")
                # TODO: By using String Object above, we could get the mangled function name instead of hardcoding
                # the mangled function.
                REGISTER_VARIABLE(f"{temp_string_var_name}", f"String")
                parameter.param = temp_string_var_name
                parameter.param_type = ParameterType.STRING_CLASS

        if char_to_string_promotion_code != "":
            emit(f"{char_to_string_promotion_code}\n")

        parameters_str = ""
        has_parameters = len(parameters) > 0
        if has_parameters:
            params = [parameter.param for parameter in parameters]
            parameters_str = ",".join(params)

        # StringSplit(this, "\n") 
        #             ^^^^ member_access_string
        # StringSplit(&this->Car.get_name(), "\n") 
        if "->" in member_access_string and member_access_string[0] != "&":
            member_access_string = "&" + member_access_string

        additional_data = {
            "fn_name": fn_name_mangled,
            "return_type": return_type,
            "is_return_type_ref_type": is_return_type_ref_type,
            "has_parameters": has_parameters,
            "parameters_str": parameters_str,
            "function_call_type": parsing_fn_call_type,
            "member_access_string": member_access_string,
        }
    
        expression_info = SpeculativeExpressionInfo()
        expression_info.speculative_expression_type = SpeculativeExpressionType.FUNCTION_CALL_EXPRESSION
        expression_info.speculative_expression_value = ""
        expression_info.function_call_metadata = additional_data
        return expression_info

    def parse_term():
        """Parse a variable(a single term)"""
        parameter_info = _read_a_parameter()
        parameter = parameter_info.param
        parameter_type = parameter_info.param_type
        struct_instance = parameter_info.struct_instance

        return {
            "value": parameter,
            "type": parameter_type,
            "struct_instance": struct_instance,
            "parameter": parameter_info
        }

    def get_comparision_operator():
        """Read comparision operator (if any) which includes >, <, >=, <=, ==, !=, in, not in"""
        has_comparision_operator = False
        operators_as_str = ""

        global parser

        if parser.has_tokens_remaining():
            token_map = {
                lexer.Token.GREATER_THAN: ">",
                lexer.Token.SMALLER_THAN: "<",
                lexer.Token.EQUALS: "=",
                lexer.Token.EXCLAMATION: "!",
            }

            token = parser.current_token()

            if token in token_map:
                parser.consume_token(token)

                has_comparision_operator = True
                operators_as_str = token_map[token]

                if (
                    parser.has_tokens_remaining()
                    and parser.current_token() == lexer.Token.EQUALS
                ):
                    parser.consume_token(lexer.Token.EQUALS)
                    operators_as_str += "="
            elif token == lexer.Token.IN:
                parser.consume_token(lexer.Token.IN)

                has_comparision_operator = True
                operators_as_str = "in"
            elif token == lexer.Token.NOT:
                parser.consume_token(lexer.Token.NOT)
                # not is implemented only for in to make 'not in'.
                parser.consume_token(lexer.Token.IN)

                has_comparision_operator = True
                operators_as_str = "not in"

        return {
            "has_comparision_operator": has_comparision_operator,
            "operators_as_str": operators_as_str,
        }

    def handle_equality(var_to_check_against, var_to_check, l_type, r_type, left_struct_info, is_lhs_struct, negation):
        operator = "!=" if negation else "=="
        if l_type == ParameterType.STR_TYPE and r_type == ParameterType.STR_TYPE:
            return f"strcmp({var_to_check}, {var_to_check_against}) {operator} 0"

        if r_type == ParameterType.RAW_STRING:
            return handle_raw_string_equality(var_to_check_against, var_to_check, l_type, left_struct_info, is_lhs_struct, negation)

        if is_lhs_struct:
            return handle_struct_equality(var_to_check_against, var_to_check, r_type, left_struct_info, negation)

        if l_type == ParameterType.CHAR_TYPE:
            if r_type == ParameterType.CHAR_TYPE:
                # char a, char b
                comparision_code = f'{var_to_check_against} == {var_to_check}'
                return f"!({comparision_code})" if negation else comparision_code

            # char a , 'B'
            return handle_char_equality(var_to_check_against, var_to_check, l_type, negation)

        # if Char == "\""
        if var_to_check == '"':
            return f'{var_to_check_against} {operator} "{var_to_check}"'
        else:
            comparision_code = (
                f'{var_to_check_against} == {var_to_check}'
            )
            return f"!({comparision_code})" if negation else comparision_code

    def boolean_expression() -> BooleanExpressionType:
        expr = BooleanExpressionType()

        lhs = parse_term()

        comparision_operation = get_comparision_operator()
        if comparision_operation["has_comparision_operator"]:
            operators_as_str = comparision_operation["operators_as_str"]

            rhs = parse_term()

            var_to_check_against = lhs["value"]
            l_type = lhs["type"]

            var_to_check = rhs["value"]
            r_type = rhs["type"]

            left_struct_info = get_instanced_struct(var_to_check_against)
            if left_struct_info == None and lhs["struct_instance"] != None:
                left_struct_info = lhs["struct_instance"]
            is_lhs_struct = left_struct_info != None

            right_struct_info = get_instanced_struct(var_to_check)
            if right_struct_info == None and rhs["struct_instance"] != None:
                # Sometimes var_to_check is a->b, the functionality of get_instanced_struct()
                # can be acheived by rhs["struct_instance"].
                right_struct_info = rhs["struct_instance"]
            is_rhs_struct = right_struct_info != None

            if is_lhs_struct:
                expr.structs_involved.append(var_to_check_against)
            if is_rhs_struct:
                expr.structs_involved.append(var_to_check)

            negation = operators_as_str == "!="
            
            if operators_as_str in {"==", "!="}:
                return_code = handle_equality(
                    var_to_check_against, var_to_check, l_type, r_type,
                    left_struct_info, is_lhs_struct, negation
                )

                # handle_struct_equality() may return a dict.
                if isinstance(return_code, Dict):
                    #return {"code": code, "return_type": return_type}
                    expr.return_value = return_code["code"]
                    expr.return_type = return_code["return_type"]
                else:
                    expr.return_value = return_code
                    expr.return_type = "bool"
            elif operators_as_str in {"in", "not in"}:
                # if var_to_check in var_to_check_against {
                # if random_index not in this.scope_stack{
                var_to_check = lhs["value"]
                var_to_check_against = rhs["value"]

                if operators_as_str == "not in":
                    negation = True

                expr.return_type = "bool"

                if is_rhs_struct:
                    # Create a function expression and merge the tokens to the current parser.
                    # Parse the function expression using the recently merged tokens.
                    ANIL_code = f"{var_to_check_against}.__contains__({var_to_check})"
                    if lhs['type'] == ParameterType.RAW_STRING:
                        ANIL_code = f"{var_to_check_against}.__contains__(\"{var_to_check}\")"
                    ANIL_code = ANIL_code.replace("->",".")
                    if ANIL_code[0] == "&":
                        ANIL_code = ANIL_code[1:]

                    fn_parser = Parser.Parser(ANIL_code)
                    parser.tokens = fn_parser.tokens + parser.tokens
                    fn_call_parse_info = function_call_expression()
                    if fn_call_parse_info == None:
                        RAISE_ERROR(f"For \"{ANIL_code}\", Boolean expression fn call parsing failed.")
                    else:
                        return_code = fn_call_parse_info.get_fn_str()
                        if negation:
                            return_code = f"!{return_code}" 

                        expr.return_value = return_code
                elif is_variable_array_type(var_to_check_against):
                    return_code = handle_array_in_operator(var_to_check, var_to_check_against)
                    if negation:
                        return_code = f"!{return_code}" 
                    expr.return_value = return_code
                    # The arrays we have created till now aren't arrays of structs,
                    # just array of <ints>, hope this code below for that too.
                else:
                    RAISE_ERROR(f"Target variable {var_to_check_against} is undefined. It is neither an array nor a struct.")
            else:
                return_code = f"{lhs['value']} {operators_as_str} {rhs['value']}"
                expr.return_value = return_code
                expr.return_type = "bool"
                # TODO : Check if return_type is bool.
        else:
            return_code = lhs["value"]

            var_to_check_against = lhs["value"]

            left_struct_info = get_instanced_struct(var_to_check_against)
            if left_struct_info == None and lhs["struct_instance"] != None:
                left_struct_info = lhs["struct_instance"]
            is_lhs_struct = left_struct_info != None
            if is_lhs_struct:
                expr.structs_involved.append(var_to_check_against)

            expr.return_value = return_code
            expr.returns_single_value = True
            expr.return_type = lhs["type"]
        return expr

    if len(parser.tokens) > 0:
        parsed_member = parser.current_token()

        instanced_struct_info = get_instanced_struct(parsed_member)
        is_instanced_struct_ = instanced_struct_info != None

        if is_instanced_struct_:
            # This is to handle function calls and assignments on struct members.
            # tokens.push("A")
            tk = parsed_member
            parser.save_checkpoint()

            child_struct_info = None
            base_struct_info = None
            struct_instance = None
            member_access_string = ""
            pointer_access = "->"

            parser.next_token()

            class ExpressionType(Enum):
                ASSIGNMENT = 0 # a.b = 10
                FUNCTION_CALL = 1 # a.b()
                ADD = 2 # a += b + c + ...
                SUBTRACT = 3 # a -= 1 ..
                INDEXED_MEMBER_ACCESS = 4 # a[3] = ..
            expression_type = ExpressionType.ASSIGNMENT

            struct_tk = tk

            while True:
                if child_struct_info != None:
                    if child_struct_info.has_member_fn(tk) or child_struct_info.has_macro(tk):
                        expression_type = ExpressionType.FUNCTION_CALL
                        break

                base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance = Speculative_parse_struct_member_access(tk, base_struct_info, child_struct_info, member_access_string, pointer_access)
            
                tk = parser.get_token()
                
                if tk == lexer.Token.DOT:
                    tk = parser.get_token()
                    continue
                elif tk == lexer.Token.EQUALS:
                    expression_type = ExpressionType.ASSIGNMENT
                    break
                elif tk == lexer.Token.PLUS:
                    expression_type = ExpressionType.ADD
                    break
                elif tk == lexer.Token.MINUS:
                    expression_type = ExpressionType.SUBTRACT
                    break
                elif tk == lexer.Token.LEFT_SQUARE_BRACKET:
                    expression_type = ExpressionType.INDEXED_MEMBER_ACCESS
                    break                    
                elif tk == lexer.Token.LEFT_ROUND_BRACKET:
                    expression_type = ExpressionType.FUNCTION_CALL
                    break

            if expression_type == ExpressionType.FUNCTION_CALL:
                StructInfo = child_struct_info
                if StructInfo.has_member_fn(tk):
                    # ctok.push(10)

                    parser.rollback_checkpoint()

                    with SpeculativeFunctionParse():
                        fn_call_parse_info = function_call_expression()
                        if fn_call_parse_info == None:
                            RAISE_ERROR("Parsing function expression failed.")

                        code = fn_call_parse_info.get_fn_str()

                        if tk == "__init__" and (not main_fn_found) and (not is_inside_user_defined_function):
                            # Found a constructor function in global scope.
                            global_variables_initialization_code.append(f"{code};\n")
                        else:
                            emit(f"{code};\n")
                elif StructInfo.has_macro(tk):
                    # macro type functinons
                    struct_name = StructInfo.name
                    parse_macro(struct_name, "CLASS_MACRO", tk)
                    code_generator.emit_comment(f"Class Macro.")
                else:
                    RAISE_ERROR(f"FATAL ERROR(Should never happen):{tk} is neither a struct macro nor a member function.")
                continue
            
            parsed_member = member_access_string
            # We are generating ANIL code, so all these C specific value accessors(a->b etc) are removed.
            # This will be generated again(as required) when parsing the ANIL code in next line.
            # _parse_struct_member_access() above added these symbols, we remove them now.
            parsed_member = parsed_member.replace("->",".")
            if parsed_member[0] == "&":
                parsed_member = parsed_member[1:]

            if expression_type == ExpressionType.ADD:
                # str += "World"
                #      ^
                parser.consume_token(lexer.Token.EQUALS)

                term = parse_term()

                # String expressions have an ast.
                parameters = term["parameter"]
                ast = parameters.ast

                if ast.params:
                    # String expression parameters have ast values.
                    ANIL_code = generate_add_string_expression_code_For_simple_AST(parsed_member, ast)
                    insert_intermediate_lines(index, ANIL_code)
                    continue
                else:
                    add_method = "__add__"
                    data_type = struct_instance.struct_type
                    # TODO: Check if we have __add__ method for this object.
                    value_to_add = parameters.param
                    if member_access_string[0] == "&":
                        member_access_string = member_access_string[1:]
                    ANIL_code = [f"{member_access_string}.{add_method}({value_to_add});\n"]
                    insert_intermediate_lines(index, ANIL_code)
                    # RAISE_ERROR("Expected String expression and .") 
                continue                                
            elif expression_type == ExpressionType.SUBTRACT:
                # this.size -= 1
                #            ^
                parser.consume_token(lexer.Token.EQUALS)

                value = get_integer_expression("Expected integer expression to subtract from existing integer named \"{parsed_member}\".")
            
                data_type = struct_instance.struct_type
                if data_type == "int":
                    # member_access_string = this->size
                    ANIL_code = f"{member_access_string} -= {value}; \n"
                    emit(ANIL_code)
                else:
                    RAISE_ERROR(f"Subtraction operation is not implemented for data type {data_type}.")
                continue
            elif expression_type == ExpressionType.ASSIGNMENT:
                data_type = struct_instance.struct_type
                if data_type in {"int", "bool", "float", "char"}: 
                    value = parser.get_token()
                    code_generator.emit_reassignment(member_access_string, value)
                else:
                    # Expression Reassignment.
                    reassign_ANIL_code = f"{parsed_member}.__reassign__("
                    #                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ convert to tokens.
                    tmp_code_parser = Parser.Parser(reassign_ANIL_code)
                    reassign_expr_tokens = tmp_code_parser.tokens

                    # Insert the 'reassign_ANIL_code' line as tokens to current token stream.
                    # This converts a = b expression to a.__reassign__(b).
                    parser.tokens = reassign_expr_tokens + parser.tokens + [lexer.Token.RIGHT_ROUND_BRACKET]
                    
                    # Parse the newly formed expression.
                    with SpeculativeFunctionParse():
                        fn_call_parse_info = function_call_expression()
                        if fn_call_parse_info == None:
                            RAISE_ERROR(f"For \"{reassign_ANIL_code}\", expression fn call parsing failed.")
                        else:
                            return_code = fn_call_parse_info.get_fn_str()
                            emit(f"{return_code};\n")
                continue
            elif expression_type == ExpressionType.INDEXED_MEMBER_ACCESS:
                # This should parse
                # TOKEN_MAP["="] = 1
                #                  ^____ Value
                #           ^^^_________ Key
                # parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                parameters = [_read_a_parameter()]
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)
                parser.consume_token(lexer.Token.EQUALS)

                parameters.append(_read_a_parameter())

                # For Dictionary Like Data Items.
                # TOKEN_MAP["="] = 1
                code = ""
                parameters_str = ""
                if len(parameters) > 0:
                    params = []
                    for parameter in parameters:
                        if parameter.param_type == ParameterType.RAW_STRING:
                            m_param = escape_quotes(f"{parameter.param}")
                            params.append(f'"{m_param}"')
                        else:
                            params.append(parameter.param)

                    parameters_str = ",".join(params)

                code = f"{parsed_member}.__setitem__({parameters_str})\n"
                index_to_insert_at = index
                Lines.insert(index_to_insert_at, code)
                continue
            else:
                RAISE_ERROR("FATAL ERROR(Should never happen): Unrecognized expression type.")
        elif is_macro_name(parsed_member):
            # add_token_raw "="
            parse_macro(parsed_member, "NORMAL_MACRO")

            # Index is already incremented above so,
            # ##Lines.insert(index + 1, code)
            # Lines.insert(index, code)
            continue
        elif is_global_function(parsed_member):
            m_fn = get_global_function_by_name(parsed_member)
            if m_fn.return_type == "void" and len(m_fn.fn_arguments) == 0:
                code_generator.emit_function_call(function_name = parsed_member, arguments = [])
            else:
                fn_name = parsed_member
                parser.next_token()

                # call_func(ANIL_function, 5)
                #          ^
                parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

                params = []

                while True:
                    if parser.check_token(lexer.Token.RIGHT_ROUND_BRACKET):
                        break
                    # FIXME: Only a single keyword long parameter is supported as of now.
                    # Use built in other parsers to parse the parameters.
                    # The advantage now is that it doesn't perform strict
                    # checking of the parameters.
                    param = parser.get_token()
                    params.append(param)
                    if parser.check_token(lexer.Token.COMMA):
                        parser.consume_token(lexer.Token.COMMA)
                parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

                code_generator.emit_function_call(function_name = fn_name, arguments = params)
            continue
        elif is_variable_boolean_type(parsed_member):
            # escape_back_slash = False
            parser.next_token()

            parser.consume_token(lexer.Token.EQUALS)

            curr_token = parser.get_token()
            is_true_token = curr_token == lexer.Token.TRUE
            is_false_token = curr_token == lexer.Token.FALSE

            if is_true_token or is_false_token:
                code_generator.emit_boolean_reassignment(parsed_member, is_true_token)
            else:
                RAISE_ERROR("Expected a boolean value.")
            continue
        elif is_variable_int_type(parsed_member):
            # new_scope = random_index
            parser.next_token()

            parser.consume_token(lexer.Token.EQUALS)
            value_to_assign = get_integer_expression(f"Expected integer expression to reassign to existing integer named \"{parsed_member}\".")

            code_generator.emit_reassignment(parsed_member, value_to_assign)
            continue
        elif is_class_name(parsed_member):
            # ClassName::StaticFunctionCall()
            # ^^^^^^^^^
            fn_call_parse_info = function_call_expression()
            if fn_call_parse_info == None:
                RAISE_ERROR("Parsing static function call expression failed.")

            parse_result = fn_call_parse_info.function_call_metadata
            fn_name = parse_result["fn_name"]
            return_type = parse_result["return_type"]
            is_return_type_ref_type = parse_result["is_return_type_ref_type"]
            has_parameters = parse_result["has_parameters"]
            parsed_fn_call_type = parse_result["function_call_type"]

            if parsed_fn_call_type != ParsedFunctionCallType.CLASS_STATIC_FUNCTION_CALL:
                RAISE_ERROR("Expected a class static function call after ClassName::FunctionCall() syntax.")

            if return_type != "void":
                RAISE_ERROR("Class static function calls are supposed to return void as of now.")
            
            code = fn_call_parse_info.get_fn_str()
            emit(f"{code};\n")
            continue
    else:
        emit("\n")
        continue

    if check_token(lexer.Token.RIGHT_CURLY):

        top_of_stack = nesting_levels[-1]

        if top_of_stack == NestingLevel.FOR_LOOP:
            _ = nesting_levels.pop(-1)
            for_loop_depth -= 1
            decrement_scope()
            emit("}\n")
        elif top_of_stack == NestingLevel.IF_STATEMENT:
            _ = nesting_levels.pop(-1)
            decrement_scope()
            emit("}\n")

            if len(parser.tokens) >= 2:
                parser.next_token()
                if check_token(lexer.Token.ELSE):
                    parser.next_token()
                    emit("else")

                    has_else_if_branch = False

                    if len(parser.tokens) >= 1:
                        # else if branch
                        if check_token(lexer.Token.IF):
                            # Merge the line to next line
                            # so it will be parsed as separate if statement.
                            has_else_if_branch = True
                            new_line = Line.split("else", 1)[1]
                            index_to_insert_at = index
                            Lines.insert(index_to_insert_at, new_line)
                        else:
                            has_else_if_branch = False
                    else:
                        has_else_if_branch = False

                    if not has_else_if_branch:
                        # normal else branch.
                        emit("{\n")
                        nesting_levels.append(NestingLevel.ELSE_STATEMENT)
                        increment_scope()
        elif top_of_stack == NestingLevel.ELSE_STATEMENT:
            _ = nesting_levels.pop(-1)
            decrement_scope()
            emit("}\n")
        elif top_of_stack == NestingLevel.WHILE_STATEMENT:
            _ = nesting_levels.pop(-1)
            decrement_scope()
            emit("}\n")
        else:
            RAISE_ERROR("UnImplemented Right Curly.")
    elif check_token(lexer.Token.HASH):
        # Comments.
        # Just create a split at the first '#'.
        parser.next_token()

        if check_token(lexer.Token.INCLUDE):
            # this has to handle #include<string.h> as well.
            if is_anil_file:
                IncludeLines.append(Line)
            else:
                emit(Line)
        else:
            parts = Line.split("#", 1)
            line_without_hash = parts[1]
            code_generator.emit_comment(line_without_hash.strip())
    elif parser.current_token() == "print":
        parser.next_token()
        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
        
        if parser.check_token(lexer.Token.QUOTE):
            # print("Value: {x}")
            actual_str = parser.extract_string_literal()
        else:
            # print(x)
            # We wrap the variable name in braces to reuse the existing PrintNode logic.
            var_token = parser.get_token()
            actual_str = "{" + var_token + "}"
            
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        print_node = PrintNode(actual_str)
        emit(print_node.codegen())
    elif check_token(lexer.Token.CONST):
        parser.consume_token(lexer.Token.CONST)

        # const string_var = "Hello World"
        # compiled to const char* string_var = "Hello World";

        # On other hand, let string_var = "Hello World"
        # compiled to let string_var = String{"Hello World"};

        string_name = parser.get_token()

        parser.consume_token(lexer.Token.EQUALS)

        if parser.check_token(lexer.Token.QUOTE):
            # const str = "Hello World"
            #           ^
            string = parser.extract_string_literal()
            code_generator.emit_string_literal_declaration(string_name = string_name, string = string)
            REGISTER_VARIABLE(string_name, "c_str")
        else:
            RAISE_ERROR("Only const strings are supported as of now.")
    elif check_token(lexer.Token.LET):
        parser.consume_token(lexer.Token.LET)

        array_name = parser.get_token()
        # print(f"Obtained array name = {array_name}")

        # let x : int = 10
        # let x : float = 10

        parsing_POD_type = False
        POD_type = ""
        if parser.check_token(lexer.Token.COLON):
            parser.consume_token(lexer.Token.COLON)

            parsing_POD_type = True
            # As of now var_name : datatype syntax as above,
            # is used only for POD types so.

            # let x : float = 10
            #         ^^^^^ POD_type
            POD_type = parser.get_token()

        # Custom templated array.
        # let arr<T> = [ 1, 2, 3, 4, 5 ];
        #        ^
        if parser.check_token(lexer.Token.SMALLER_THAN):
            parser.consume_token(lexer.Token.SMALLER_THAN)

            type_name = parser.get_token()
            # print(f"Obtained array type = {type_name}")
            parser.consume_token(lexer.Token.GREATER_THAN)

            parse_array(type_name, array_name)
        elif check_token(lexer.Token.EQUALS):
            # let str = "Hello World";
            # let obj = Point{10, 20};
            #         ^
            parser.consume_token(lexer.Token.EQUALS)

            if parsing_POD_type:
                if POD_type == "int":
                    integer_expr = speculative_parse_integer_expression()
                    if integer_expr is None:
                        RAISE_ERROR(f"Expected integer expression for {array_name}.")
                    integer_value = integer_expr.speculative_expression_value

                    value_node = LiteralNode(integer_value, "int")
                    stmt_node = VariableDeclarationNode(array_name, "int", value_node)
                elif POD_type == "char":
                    parser.consume_token(lexer.Token.QUOTE)
                    char_value = parser.get_token()
                    if len(char_value) != 1:
                        RAISE_ERROR(f'Char value should be length 1 but got "{char_value}" of length {len(char_value)}.')
                    parser.consume_token(lexer.Token.QUOTE)

                    value_node = LiteralNode(char_value, "char")
                    stmt_node = VariableDeclarationNode(array_name, "char", value_node)
                elif POD_type == "bool":
                    boolean_expr_code = boolean_expression().return_value
                    value_node = LiteralNode(boolean_expr_code == "true", "bool")
                    stmt_node = VariableDeclarationNode(array_name, "bool", value_node)
                else:
                    RAISE_ERROR(f'Parsing POD Type "{POD_type}" not implemented yet.')

                REGISTER_VARIABLE(array_name, POD_type)

                if is_inside_form:
                    if POD_type == "int":
                        gui_manager.register_default_value(integer_value)

                        # <form>
                        # let age : int = 10 ## 10, 20, 30
                        if parser.has_tokens_remaining() and parser.current_token() == lexer.Token.HASH:
                            parser.next_token()
                            if parser.has_tokens_remaining() and parser.current_token() == lexer.Token.HASH:
                                parser.next_token()
                                options = Line[Line.rfind("#") + 1:]
                                options = [option.strip() for option in options.split(",")]
                                gui_manager.add_gui_item_option(options)
                    elif POD_type == "char":
                        gui_manager.register_default_value(char_value)
                    elif POD_type == "bool":
                        if boolean_expr_code == "true":
                            gui_manager.register_default_value("true")

                emit(stmt_node.codegen() + "\n")
                continue
 
            if parser.check_token(lexer.Token.QUOTE):
                term = parse_term()
                term_type = term['type']

                string = ""

                if term_type == ParameterType.RAW_STRING:
                    string = term["value"]
                elif term_type == ParameterType.STRING_CLASS:
                    # First param is RAW_STRING because of Token.QUOTE.
                    params = term["parameter"].ast.params
                    p_name, p_type = params[0]
                    string = p_name
                else:
                    RAISE_ERROR("Expected some kind of string expression.")

                # let str = "Hello World";
                #           ^
                # ^^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.
                # let str = String{"Hello World"};

                # Emit ANIL code to create a struct 'String' object.
                # This line will be parsed by the compiler in next line.
                # ANIL_code = f"let {array_name} = String{{\"{string}\"}};\n"

                # Same as the above commented line, 
                # but this optimization removes a strlen() at runtime.
                escaped_length = lexer.get_escaped_length(string)
                ANIL_code = [f"let {array_name} = String{{\"{string}\", {escaped_length}}};\n"]

                parameters = term["parameter"]

                if parameters.ast.params:
                    # String expression parameters have ast values.
                    ast = parameters.ast

                    if len(ast.params) > 1:
                        # Remove the first string, as we have already initialized the string with the first string.
                        # The remaining parameters are used to generate add expression code.
                        parameters.ast.remove_param(0)
                        ast = parameters.ast

                        ANIL_code.extend(generate_add_string_expression_code_For_simple_AST(array_name, ast))
                insert_intermediate_lines(index, ANIL_code)
                continue
            elif parser.check_token(lexer.Token.TRUE) or parser.check_token(
                lexer.Token.FALSE
            ):
                # let inside_string = False
                #                     ^
                REGISTER_VARIABLE(array_name, "bool")

                value_node = LiteralNode(parser.current_token() == lexer.Token.TRUE, "bool")
                stmt_node = VariableDeclarationNode(array_name, "bool", value_node)
                emit(stmt_node.codegen() + "\n")
            elif parser.check_token(lexer.Token.LEFT_SQUARE_BRACKET):
                #let test_list = [];
                parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

                #This will be converted to:
                #     let test_list = List{};
                #And, ^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.

                # Emit ANIL code to create a struct 'List' object.
                # This line will be parsed by the compiler in next line.
                ANIL_code = f"let {array_name} = List{{ }};\n"

                index_to_insert_at = index
                Lines.insert(index_to_insert_at, ANIL_code)
                continue
            elif parser.check_token(lexer.Token.LEFT_CURLY):
                #Parse Dictionary Type.
                #let dict = {};
                parser.consume_token(lexer.Token.LEFT_CURLY)
                parser.consume_token(lexer.Token.RIGHT_CURLY)

                #This will be converted to:
                #     let dict = Dictionary{};
                #And, ^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.

                # Emit ANIL code to create a struct 'Dictionary' object of Lib/Dictionary.
                # This line will be parsed by the compiler in next line.
                ANIL_code = f"let {array_name} = Dictionary<int>{{ }};\n"

                index_to_insert_at = index
                Lines.insert(index_to_insert_at, ANIL_code)
                continue
            else:
                # let obj = Point{10, 20};
                #           ^
                #           | struct_type
                struct_name = array_name

                struct_type = parser.current_token()
                # print(f"struct type = {struct_type}")

                StructInfo = get_struct_defination_of_type(struct_type)
                if StructInfo != None:
                    # Rectangle::
                    # ^
                    if parser.check_n_tokens([struct_type, lexer.Token.COLON, lexer.Token.COLON]):
                        # This is a static function call/variable access.
                        parse_access_struct_member(struct_name)
                        continue

                    parser.next_token()

                    # We are creating a struct type.
                    parse_create_struct(struct_type, struct_name)
                else:
                    # let str3 = str2.strip()
                    var_name = struct_name
                    target = struct_type

                    is_constexpr_dict = is_constexpr_dictionary(target)

                    #  let color = Color["Red"]
                    if is_constexpr_dict:
                        #  let color = Color["Red"]
                        #                   ^
                        parser.next_token()

                        if is_string_constexpr_dictionary(target):
                            found_raw_string_during_lookup, actual_value = parse_string_constexpr_dictionary(target)

                            if found_raw_string_during_lookup:
                                # We are performing direct lookup on string dict with a string literal,
                                # so we write the string initialization code directly.
                                emit(f"struct String {var_name}; \n")
                                emit(f'String__init__from_charptr(&{var_name}, "{actual_value}", {len(actual_value)}); \n')
                            else:
                                # We passed variable to constexpr dict lookup.
                                # So, we write a function call instead.
                                code_generator.emit_variable_declaration(
                                    variable_type="struct String",
                                    variable_name=var_name,
                                    initialization_value=actual_value
                                )

                            instance = StructInstance("String", f"{var_name}", get_current_scope())
                            instanced_struct_names.append(instance)
                            REGISTER_VARIABLE(f"{var_name}", "String")
                        else:
                            actual_value = parse_constexpr_dictionary(target)
                            dict_type = get_constexpr_dictionary_type(target)
                            if dict_type == "number":
                                code_generator.emit_variable_declaration(variable_type="int", variable_name=var_name, initialization_value=actual_value)
                                REGISTER_VARIABLE(f"{var_name}", "int")
                            else:
                                RAISE_ERROR(f"Dict type {dict_type} undefined.")
                    else:
                        parse_access_struct_member(var_name)
    elif check_token(lexer.Token.IF):
        nesting_levels.append(NestingLevel.IF_STATEMENT)
        increment_scope()

        parser.consume_token(lexer.Token.IF)
        code = boolean_expression()
        parser.consume_token(lexer.Token.LEFT_CURLY)

        emit(f"\nif({code.return_value}){{\n")
    elif check_token(lexer.Token.FOR):
        # Normal, Un-Enumerated loops.
        # for current_array_value_variable in array_name{

        # Enumerated loops.
        # for ranged_index_item_variable,current_array_value_variable in enumerate array_name{
        nesting_levels.append(NestingLevel.FOR_LOOP)
        for_loop_depth += 1
        increment_scope()

        parser.consume_token(lexer.Token.FOR)

        current_array_value_variable = parser.get_token()
        ranged_index_item_variable = None

        is_enumerated_for_loop = False

        # Enumerated loop Access probably.
        if parser.check_token(lexer.Token.COMMA):
            parser.next_token()

            is_enumerated_for_loop = True

            ranged_index_item_variable = current_array_value_variable
            # In enumerated loop, ranged_index_item_variable appears first,
            # but we parse 'current_array_value_variable' earlier,
            # so we need to fix their order.

            current_array_value_variable = parser.get_token()

        parser.consume_token(lexer.Token.IN)

        if is_enumerated_for_loop:
            parser.consume_token(lexer.Token.ENUMERATE)

        is_struct = False

        array_name = ""

        if parser.current_token() == "range":
            array_name = "range"
            parser.next_token()
        else:
            # Workaround to fix a[] and a[::-1] parsing issues.
            # a.b
            if parser.peek_token() == lexer.Token.DOT:
                parsed_term = parse_term()
                array_name = parsed_term["value"]
                is_struct = parsed_term != None and parsed_term["struct_instance"] != None
                array_name = array_name.replace("->",".")
            else:
                array_name = parser.get_token()
                is_struct = is_instanced_struct(array_name)

        if is_enumerated_for_loop:
            create_array_enumerator(
                array_name, ranged_index_item_variable, current_array_value_variable
            )
        else:
            if is_variable_const_char_ptr(array_name):
                create_const_charptr_iterator(array_name, current_array_value_variable)
            elif is_struct:
                # This generates new ANIL code which creates a for loop(which makes a new scope).
                # We already have created a scope above by increment scope.
                # We call decrement_scope() to compensate that.
                for_loop_depth -= 1
                
                # 'NestingLevel.FOR_LOOP' is added to 'nesting_levels' above.
                # But this FOR_LOOP scope is not used, because
                # create_array_iterator_from_struct below generates another
                # range FOR LOOP which creates it's own scope.
                # So, we need to remove the FOR Loop scope that we just added above.
                popped = nesting_levels.pop(-1)
                if popped != NestingLevel.FOR_LOOP:
                    RAISE_ERROR("FATAL ERROR(Should never happen): Currently removed scope should be FOR_LOOP.")

                decrement_scope()
                create_array_iterator_from_struct(
                    array_name, current_array_value_variable
                )
            elif array_name == "range":
                # If we have some list named range, then it will be parsed earlier above ^^^.
                create_range_iterator(current_array_value_variable)
            else:
                create_normal_array_iterator(array_name, current_array_value_variable)
        
        parser.match_token(lexer.Token.LEFT_CURLY)
    elif check_token(lexer.Token.WHILE):
        nesting_levels.append(NestingLevel.WHILE_STATEMENT)
        increment_scope()

        parser.consume_token(lexer.Token.WHILE)
        code = boolean_expression()
        parser.consume_token(lexer.Token.LEFT_CURLY)

        emit(f"\nwhile({code.return_value}){{\n")
    elif check_token(lexer.Token.STRUCT):
        #  struct Point {T x, T y };
        parser.consume_token(lexer.Token.STRUCT)
        struct_name = parser.get_token()

        is_struct_templated = False

        generic_data_types = []

        incomplete_types = []
        # For templated struct like struct<T> A{X<T> B};
        #                                  ^ is stored in 'incomplete_types'.
        # That means while parsing if we encounter 'T' like for X<T>, 
        # we dont mangle X<T> and leave as it is to hint that it should be mangled,
        # during template instantiation and not parsing phase.

        if parser.check_token(lexer.Token.SMALLER_THAN):
            parser.next_token()
            is_struct_templated = True
            # print("Generic Struct")
            # struct GenericStruct<X>{X a, float b};
            #                      ^
            generic_data_type = parser.current_token()
            generic_data_types.append(generic_data_type)
            # print(f"Generic type : {generic_data_type}")
            parser.next_token()
            parser.consume_token(lexer.Token.GREATER_THAN)

            incomplete_types.append(generic_data_type)

        # print(f"Found Struct Defination : {struct_name}")

        parser.consume_token(lexer.Token.LEFT_CURLY)

        struct_members_list = []

        while parser.current_token() != lexer.Token.RIGHT_CURLY:
            struct_member_type = parse_data_type(inner=False, incomplete_types=incomplete_types)

            data_type_is_self = False
            if not is_struct_templated:
                # struct DictObject<T>{Self *next};
                # For templated class this is performed later in 'substitute_template_for_member_types()'.
                if struct_member_type == "Self":
                    struct_member_type = f"struct {struct_name}"
                    data_type_is_self = True

            # parse_data_type returns "struct Vector" with struct keyword.
            # We dont write "struct" for MemberDataTypes, so we need to strip it.
            struct_member_type = data_type_with_struct_stripped(struct_member_type)

            pointerless_struct_member_type = struct_member_type

            # Pointer, Pointer to pointer, pointer to ...
            is_data_type_pointer_type = False
            while parser.check_token(lexer.Token.ASTERISK):
                struct_member_type += "*"
                is_data_type_pointer_type = True
                parser.next_token()

            struct_member = parser.current_token()

            # struct Node{Self next};
            #             ^^^^ creates self referential structure, so don't allow it, but allow,
            # struct Node{Self* next};
            if data_type_is_self and not is_data_type_pointer_type:
                RAISE_ERROR(f"For structure \"{struct_name}\", member \"{struct_member}\" is self referential. It should be Self* and not Self.")

            is_generic = False
            if pointerless_struct_member_type in generic_data_types:
                is_generic = True
                # print(f"{pointerless_struct_member_type} is generic.")

            struct_members_list.append(
                MemberDataType(struct_member_type, struct_member, is_generic)
            )
            # print(
            # f"Type : {struct_member_type} Generic : {is_generic} Struct Member : {struct_member} "
            # )

            parser.next_token()
            if parser.check_token(lexer.Token.COMMA):
                parser.next_token()

        parser.next_token()
        parser.match_token(lexer.Token.SEMICOLON)

        struct_data = Struct(struct_name, struct_members_list)

        if is_struct_templated:
            struct_data.is_class_templated = True
            struct_data.template_defination_variable = generic_data_types[0]
        else:
            # Dont immediately instantiate structs which are generic.
            struct_code = struct_data.get_struct_initialization_code()
            GlobalGeneratedStructDefinations += struct_code

            # GlobalStructInitCode += struct_code

        struct_definations[struct_data.name] = struct_data
        # Non generic structs shouldn't be written out early, but since the c_function blocks write out functions despite being templated we leave the base templated struct defined, so that the funtions generated don't have defination error.
        # GlobalStructInitCode += struct_code

        # emit(code)
        # [struct_name,x,y,z..]
    elif check_token(lexer.Token.CFUNCTION):
        # c_function say(Param1:type1, Param2:type2, ... ParamN:typeN)
        # void say(struct Point *this) { printf("x : %d , y : %d \n", this->x, this->y); }

        # Skip "c_function", its not a keyword, just a placeholder to identify a function call.
        parser.consume_token(lexer.Token.CFUNCTION)

        is_static_function = has_static_annotation()
        if is_static_function:
            consume_annotation("static")

        if not is_inside_name_space:
            parse_global_c_function()
            continue
        
        struct_name = namespace_name

        should_write_fn_body = True

        StructInfo = get_struct_defination_of_type(struct_name)
        if StructInfo == None:
            RAISE_ERROR(f'Struct name : "{struct_name}" is undefined.')

        struct_members_list = StructInfo.members
        # print(StructInfo.members)  # [['X', 'a', True], ['float', 'b', False]]

        is_struct_templated = StructInfo.is_templated()

        function_declaration = parse_function_declaration()

        fn_name = function_declaration["fn_name"] 
        return_type = function_declaration["return_type"]
        is_return_type_ref_type = function_declaration["is_return_type_ref_type"]
        parameters = function_declaration["parameters"]
        parameters_combined_list = function_declaration["parameters_combined_list"]
        is_overloaded = function_declaration["is_overloaded"]
        is_overloaded_fn = function_declaration["is_overloaded_fn"]
        overload_for_type = function_declaration["overload_for_type"]

        def resolve_template_data_type(p_type):
            if StructInfo != None:
                if p_type == StructInfo.template_defination_variable:
                    if StructInfo.templated_data_type:
                        return StructInfo.templated_data_type
            return p_type

        for i,param in enumerate(parameters):
            # This could be done in parameters parsing loop above,
            # but we are doing this here so that FUNCTION body is same as C_FUNCTION.
            parameters[i].data_type = resolve_template_data_type(parameters[i].data_type)

        return_type = resolve_template_data_type(return_type)

        code = ""

        unmangled_name = fn_name

        if is_struct_templated:
            should_write_fn_body = False
        else:
            if is_overloaded:
                # Get datatype from MemberDataType,
                # as get_overloaded_mangled_fn_name requires list of strings(data_type).
                data_types_str_list = [p.data_type for p in parameters]
                fn_name = get_overloaded_mangled_fn_name(struct_name, fn_name, data_types_str_list)
            else:
                fn_name = get_mangled_fn_name(struct_name, fn_name)

            return_type = format_struct_data_type(return_type)

            has_parameters = len(parameters) > 0
            
            code = f"{return_type} {fn_name}("
            if not is_static_function:
                code += f"struct {struct_name} *this"
                if has_parameters:
                    code += ", "

            if has_parameters:
                params = [
                    f"struct {p.data_type} {p.member}" if is_data_type_struct_object(p.data_type) else f"{p.data_type} {p.member}"
                    for p in parameters
                ]
                parameters_str = ",".join(params)
                code += f"{parameters_str}"
            
            code += ")"
            GlobalGeneratedFunctionDeclarations += f"{code};\n"
            code += "{\n"

        currently_reading_fn_name = unmangled_name
        currently_reading_fn_parent_struct = struct_name
        currently_reading_parameters = parameters
        currently_reading_fn_name_unmangled = unmangled_name
        currently_reading_return_type = return_type

        fn = MemberFunction(unmangled_name, parameters, return_type)
        fn.is_overloaded_function = is_overloaded_fn
        fn.overload_for_template_type = overload_for_type
        fn.is_return_type_ref_type = is_return_type_ref_type
        fn.is_static_fn = is_static_function

        add_fn_member_to_struct(struct_name, fn)

        c_function_body_start_pos = len(GlobalStructInitCode)

        GlobalStructInitCode += code
        is_inside_struct_c_function = True
    elif check_token(lexer.Token.ENDCFUNCTION):
        if is_inside_user_defined_function:
            RAISE_ERROR("Use \"endfunction\" to close a function and not \"endc_function\".")

        if is_inside_struct_c_function or is_inside_global_c_function:
            if is_inside_struct_c_function and should_write_fn_body:
                GlobalStructInitCode += "}\n\n"

            currently_reading_fn_body_dup = currently_reading_fn_body
            # Just comment out @hook_{begin/end} symbols.

            # if "@hook_begin" in currently_reading_fn_body:
            # pass
            hook_start_pos = currently_reading_fn_body.find("@hook_begin")
            hook_end_pos = currently_reading_fn_body.find("@hook_end")

            if hook_start_pos != -1 and hook_end_pos != -1:
                # Hook end_pos first to not messup string index.
                currently_reading_fn_body_dup = insert_string(
                    currently_reading_fn_body_dup, hook_end_pos, "//"
                )

                currently_reading_fn_body_dup = insert_string(
                    currently_reading_fn_body_dup, hook_start_pos, "//"
                )

                # This need modification too.

                global_hook_start_pos = GlobalStructInitCode.find("@hook_begin")
                global_hook_end_pos = GlobalStructInitCode.find("@hook_end")
                if global_hook_start_pos != -1 and global_hook_end_pos != -1:
                    index_to_insert_at = global_hook_end_pos

                    GlobalStructInitCode = insert_string(
                        GlobalStructInitCode, global_hook_end_pos, "//"
                    )

                    GlobalStructInitCode = insert_string(
                        GlobalStructInitCode, global_hook_start_pos, "//"
                    )

                hook_body = currently_reading_fn_body[hook_start_pos:hook_end_pos]

                # Get the actual fn_hook_body_without @hook_{begin/end}.
                hook_body_actual = hook_body
                hook_body_actual_start_pos = hook_body_actual.find(")")
                hook_body_actual_end_pos = hook_body_actual.find("@hook_end")
                hook_body_actual = hook_body_actual[
                    hook_body_actual_start_pos:hook_body_actual_end_pos
                ]

                # @hook_begin("custom_integer_printer" "int" data)
                parsed_hook_info = parse_hook_info(hook_body)
                hook_fn_name = parsed_hook_info.hook_fn_name
                hook_fn_arg_type = parsed_hook_info.hook_fn_arg_type
                hook_var_name = parsed_hook_info.hook_var_name

                hooker_fn_code = (
                    f"typedef void (*{hook_fn_name})({hook_fn_arg_type}); \n"
                )

                index_to_insert_at = c_function_body_start_pos

                """                        
                index_to_insert_at = c_function_body_start_pos
                GlobalStructInitCode = (
                    GlobalStructInitCode[:index_to_insert_at]
                    + hooker_fn_code
                    + GlobalStructInitCode[index_to_insert_at:]
                )
                """

                previous_fn_body = GlobalStructInitCode[index_to_insert_at:]

                # Find the ending pos of fn_name of previous fn_body.
                fn_name_end_pos = previous_fn_body.find("(")
                # Append '_hooked' to the end of fn name.
                previous_fn_body = insert_string(
                    previous_fn_body, fn_name_end_pos, f"_hooked_{hook_fn_name}"
                )

                # Find the ending pos of fn parameter of previous fn_body.
                fn_params_end_pos = previous_fn_body.find(")")
                # Silently pass the hooked_fn as a parameter.
                hooked_fn_as_fn_parameter = f"{hook_fn_name} p_{hook_fn_name}"

                previous_fn_body = insert_string(
                    previous_fn_body,
                    fn_params_end_pos,
                    f", {hooked_fn_as_fn_parameter}",
                )

                hook_body_start_pos = previous_fn_body.find("@hook_begin")
                # We need to call the function pointer at this position.

                # Replace everything between @hook_begin/end.
                pattern = r"@hook_begin.*?@hook_end"
                previous_fn_body = re.sub(
                    pattern, "", previous_fn_body, flags=re.DOTALL
                )

                call_hooked_fn = f"p_{hook_fn_name}({hook_var_name});\n"
                # We somehow inhert //
                # so we place a \n before 'call_hooked_fn'.
                previous_fn_body = insert_string(
                    previous_fn_body, hook_body_start_pos, f"\n{call_hooked_fn}"
                )

                GlobalStructInitCode += "\n" + hooker_fn_code
                GlobalStructInitCode += previous_fn_body

                """

                parameters_duplicated: list = currently_reading_parameters
                parameters_duplicated.append(f"{hook_fn_name} p_{hook_fn_name}")

                # Create a duplicated function.
                add_fn_member_to_struct(
                    currently_reading_fn_parent_struct,
                    MemberFunction(
                        currently_reading_fn_name_unmangled,
                        parameters_duplicated,
                        currently_reading_return_type,
                    ),
                )

                add_fnbody_to_member_to_struct(
                    currently_reading_fn_parent_struct,
                    currently_reading_fn_name + f"_hooked{hook_fn_name}",
                    hooker_fn_code + currently_reading_fn_body,
                )

                """
        
            for param in registered_function_parameters:
                remove_struct_instance(param)

            if is_inside_global_c_function:
                if should_write_fn_body:
                    emit(currently_reading_fn_body)
                    code = "}\n"
                    emit(code)
            else:
                add_fnbody_to_member_to_struct(
                    currently_reading_fn_parent_struct,
                    currently_reading_fn_name,
                    currently_reading_fn_body_dup,
                )

            currently_reading_fn_body = ""
            should_write_fn_body = True
            is_inside_struct_c_function = False
            is_inside_global_c_function = False
            registered_function_parameters = []
        else:
            RAISE_ERROR("End c_function without being in c_function block.")
    elif check_token(lexer.Token.DEF):
        # This is used to define function like macros.
        # This macros are just replaced in place.
        """
        def add_token_raw(p_token):
            tokens.push p_token
        enddef

        add_token_raw "="
        """
        parser.consume_token(lexer.Token.DEF)

        fn_name = parser.get_token()

        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

        param1 = ""
        currently_reading_def_paramter_initializer_list = False

        if parser.current_token() != lexer.Token.RIGHT_ROUND_BRACKET:
            # This macro defination has a parameter.
            param1 = parser.current_token()
            # print(f"Obtained First param : {param1}")

            parser.next_token()

            # X...
            if parser.current_token() == lexer.Token.DOT:
                parser.next_token()
                if parser.current_token() == lexer.Token.DOT:
                    parser.next_token()
                    if parser.current_token() == lexer.Token.DOT:
                        parser.next_token()
                        currently_reading_def_paramter_initializer_list = True
                    else:
                        RAISE_ERROR("Expected 3 dots for initializer list.")
                else:
                    RAISE_ERROR("Expected 3 dots for initializer list.")

        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        target_class = ""

        # def pushn(X...) for Vector: <- class macros.
        # def push(X...): <- normal macros
        macro_type = "NORMAL_MACRO"
        if parser.current_token() == lexer.Token.COLON:
            parser.match_token(lexer.Token.COLON)
            macro_type = "NORMAL_MACRO"
        elif parser.current_token() == lexer.Token.FOR:
            parser.consume_token(lexer.Token.FOR)
            macro_type = "CLASS_MACRO"

            target_class = parser.get_token()
            parser.consume_token(lexer.Token.COLON)
            # print(f"The macro is defined for {target_class} class.")

        is_inside_def = True
        currently_reading_def_name = fn_name
        currently_reading_def_parameter = param1
        currently_reading_def_macro_type = macro_type
        currently_reading_def_target_class = target_class
    elif check_token(lexer.Token.CONSTEXPR):
        # constexpr Color = {"Red":255, "Green":200}
        parser.consume_token(lexer.Token.CONSTEXPR)

        dict_name = parser.get_token()
        # print(f"Obtained Constexpr Dictionary name = {dict_name}")

        parser.consume_token(lexer.Token.EQUALS)

        parser.consume_token(lexer.Token.LEFT_CURLY)

        dict = {}
        default_key_value = None
        dict_type = "number"

        while True:
            # default : ""}
            # ^^^^^^^
            key = None
            if parser.check_token(lexer.Token.QUOTE):
                key = parser.extract_string_literal()
            else:
                key = parser.get_token()
                if key == "default":
                    if default_key_value is not None:
                        RAISE_ERROR("Ther can be only one default value")
                else:
                    RAISE_ERROR("Only default word except strings can be Constexpr Dictionary key format.")

            parser.consume_token(lexer.Token.COLON)

            #constexpr COMPILE_FLAGS = {"Win32" : "-mwindows -lgdi32 -lcomctl32", 
            #                                     ^
            value = None
            if parser.check_token(lexer.Token.QUOTE):
                value = parser.extract_string_literal()
                dict_type = "string"
            else:
                value = parser.get_token()

            if key == "default":
                default_key_value = value
            else:
                dict[key] = value

            if parser.check_token(lexer.Token.RIGHT_CURLY):
                parser.next_token()
                break
            elif parser.check_token(lexer.Token.COMMA):
                parser.consume_token(lexer.Token.COMMA)
            else:
                RAISE_ERROR("Incorrect Constexpr Dictionary Format.")

        if is_constexpr_dictionary(dict_name):
            RAISE_ERROR(f'Constexpr dictionary "{dict_name}" already defined.')
        m_dict = ConstexprDictionaryType(dict_name, dict)
        m_dict.default_key_value = default_key_value
        m_dict.dict_type = dict_type
        constexpr_dictionaries.append(m_dict)
    elif check_token(lexer.Token.AT):
        # @apply_hook("custom_integer_printer", CustomPrint)
        parser.consume_token(lexer.Token.AT)

        if check_token(lexer.Token.APPLY_HOOK):
            parser.consume_token(lexer.Token.APPLY_HOOK)
            parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
            HOOKS_hook_fn_name = parser.extract_string_literal()
            parser.consume_token(lexer.Token.COMMA)
            HOOKS_target_fn = parser.get_token()
            parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)
        else:
            # @route("/Home")
            #         ^^^^^^ annotation_argument_value
            #  ^^^^^ annotation_name
            # function Home():
            #          ^^^^ annotated_fn_name
            annotation_name = parser.get_token()

            annotation_argument_values = []

            # @static
            # annotation without arguments is also valid.
            if parser.has_tokens_remaining():
                parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
                
                # @route("GET", "/get_todos")
                while True:
                    annotation_argument_value = parser.extract_string_literal()
                    annotation_argument_values.append(annotation_argument_value)

                    if parser.check_token(lexer.Token.COMMA):
                        parser.consume_token(lexer.Token.COMMA)
                    elif parser.check_token(lexer.Token.RIGHT_ROUND_BRACKET):
                        break

                parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

            annotated_item = Annotation(annotation_name, annotation_argument_values, "")
            # Leave annotated_fn_name empty. Patch it later. 
            # PATCH_ANNOTATION

            # @default_route("Home")
            # @route("/Home")
            # function Home()
            # Multiple annotations can be stacked line by line, so save it in a stack.
            temporary_annotations_list.append(annotated_item)
    elif check_token(lexer.Token.FUNCTION):
        # function say()
        # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
        # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN) -> return_type
        parser.consume_token(lexer.Token.FUNCTION)

        # @static
        # function say()
        is_static_function = has_static_annotation()
        if is_static_function:
            # only static annotation is supported for class functions.
            # remove static annotation.
            consume_annotation("static")

        should_write_fn_body = True
        scopes_with_return_stmnt = []
        registered_function_parameters = []

        function_declaration = parse_function_declaration()

        function_name = function_declaration["fn_name"]
        return_type = function_declaration["return_type"]
        is_return_type_ref_type = function_declaration["is_return_type_ref_type"]
        parameters = function_declaration["parameters"]
        parameters_combined_list = function_declaration["parameters_combined_list"]
        is_overloaded = function_declaration["is_overloaded"]
        is_overloaded_fn = function_declaration["is_overloaded_fn"]
        overload_for_type = function_declaration["overload_for_type"]

        # if this function is a struct member function, it needs to be mangled below.
        func_name = function_name

        defining_fn_for_custom_class = False
        
        if is_inside_name_space:
            defining_fn_for_custom_class = True 
            class_fn_defination["class_name"] = namespace_name
            class_fn_defination["function_name"] = function_name
            class_fn_defination["start_index"] = ctx.get_generated_lines_count()

            if function_name == "WinMain":
                RAISE_ERROR(f"WinMain is a global entry point. It can't be a member function of \"{namespace_name}\".")
        
        if parser.has_tokens_remaining():
            # function my_first_ANIL_function() for String
            if parser.check_token(lexer.Token.FOR):
                parser.consume_token(lexer.Token.FOR)

                target_class = parser.get_token()

                if is_inside_name_space:
                    RAISE_ERROR(f"Is already inside a namespace \"{namespace_name}\". Doesn't need to define target class(\"{target_class}\") for the provided function using the FOR keyword.")

                if function_name == "WinMain":
                    RAISE_ERROR(f"WinMain is a global entry point. It can't be a member function of \"{namespace_name}\".")

                defining_fn_for_custom_class = True
                class_fn_defination["class_name"] = target_class
                class_fn_defination["function_name"] = function_name
                class_fn_defination["start_index"] = ctx.get_generated_lines_count()
            else:
                RAISE_ERROR('Expected for after function declaration like "function A() for namespace_name".')

        increment_scope()
        curr_scope = get_current_scope()

        struct_def = None
        if defining_fn_for_custom_class and not is_static_function:
            struct_name = class_fn_defination["class_name"]
            struct_def = get_struct_defination_of_type(struct_name)
            
            instance = StructInstance(f"{struct_name}", "this", curr_scope)
            instance.is_pointer_type = True
            instance.should_be_freed = False

            instanced_struct_names.append(instance)
            REGISTER_VARIABLE("this", f"{struct_name}")

        # TODO: This need not be performed as the template types should be resolved at template instantiation time.
        # This could be performed by updating 'unparsed_function' during template instantiation.
        def resolve_template_data_type(p_type):
            if struct_def != None:
                if p_type == struct_def.template_defination_variable:
                    if struct_def.templated_data_type:
                        return struct_def.templated_data_type
            return p_type

        # Register the parameters so they can be used inside function body.
        # Mark them such that their destructors wont be called.
        for i,param in enumerate(parameters):
            # This could be done in parameters parsing loop above,
            # but we are doing this here so that FUNCTION body is same as C_FUNCTION.
            param_type = resolve_template_data_type(param.data_type)
            param_name = param.member

            parameters[i].data_type = param_type

            param_type = data_type_with_struct_stripped(param_type)
                
            if is_data_type_struct_object(param_type):
                instance = StructInstance(param_type, param_name, curr_scope)

                # Function parameters shouldn't be freed at the end of the scope.
                # So, add a tag.
                instance.should_be_freed = False

                instanced_struct_names.append(instance)
                registered_function_parameters.append(param_name)
            REGISTER_VARIABLE(param_name, param_type)


        if defining_fn_for_custom_class:
            if is_overloaded:
                # Get datatype from MemberDataType,
                # as get_overloaded_mangled_fn_name requires list of strings(data_type).
                data_types_str_list = [p.data_type for p in parameters]
                func_name = get_overloaded_mangled_fn_name(class_fn_defination["class_name"], function_name, data_types_str_list)
            else:
                func_name = get_mangled_fn_name(class_fn_defination["class_name"], function_name)
        
            if temporary_annotations_list:
                RAISE_ERROR("Annotations are not supported for class functions.")
        else:
            # Annotations are implemented for global functions only as of now.
            if temporary_annotations_list:
                # PATCH_ANNOTATION
                # Patch stored (stacked) temporary annotations, which didn't have fn_name.
                for i in range(len(temporary_annotations_list)):
                    temporary_annotations_list[i].annotated_fn_name = function_name
                annotations_list.extend(temporary_annotations_list)
                temporary_annotations_list = []

        if defining_fn_for_custom_class:
            if struct_def != None:
                if struct_def.is_templated():
                    should_write_fn_body = False

        code = ""

        return_type = format_struct_data_type(resolve_template_data_type(return_type))

        has_parameters = len(parameters) > 0

        code = f"{return_type} {func_name}("
        if function_name == "WinMain":
            # We dont need to use func_name, as we have validated that function_name is a global function,
            # and not a mangled name.
            code = f"{return_type} WINAPI {function_name}("

        if defining_fn_for_custom_class and not is_static_function:
            struct_name = class_fn_defination["class_name"]
            code += f"struct {struct_name} *this"
            if has_parameters:
                code += ","
                
        if has_parameters:
            params = [f"{format_struct_data_type(p.data_type)} {p.member}" for p in parameters]
            parameters_str = ",".join(params)
            code += f"{parameters_str}"

        GlobalGeneratedFunctionDeclarations += code + ");\n"
        
        code += f") {{\n"

        emit(code)

        if is_anil_file and function_name in ("main", "WinMain"):
            # Normal .c file has identifiers which indicates where the global variables constructors is placed in main.
            # .anil files doesn't have that, so, the first line for main function is the global variables constructors
            # initialization code.
            if len(global_variables_initialization_code) > 0:
                emit("//Global Variables Initialization.\n")
                ctx.extend_generated_lines(global_variables_initialization_code)
                emit("\n")
                global_variables_initialization_code = []
        
        fn = MemberFunction(function_name, parameters, return_type)
        fn.is_overloaded_function = is_overloaded_fn
        fn.overload_for_template_type = overload_for_type
        fn.is_return_type_ref_type = is_return_type_ref_type
        fn.is_static_fn = is_static_function
        
        if defining_fn_for_custom_class:
            if should_write_fn_body:
                GlobalStructInitCode += code
            add_fn_member_to_struct(class_fn_defination["class_name"], fn)
            class_fn_defination["function_destination"] = "class"
        else:
            GlobalFunctions.append(fn)
            class_fn_defination["function_destination"] = "global"
            class_fn_defination["function_name"] = function_name

        is_inside_user_defined_function = True

        tracking_scopes_for_current_fn = True
        tracked_scopes_for_current_fn.append(curr_scope)
    elif check_token(lexer.Token.ENDFUNCTION):
        current_scope = get_current_scope()

        if not return_encountered_in_fn:
            decrement_scope() 
        else:
            # Remove all struct instances form current scope.
            # decrement_scope() above automatically does this.
            for scope in tracked_scopes_for_current_fn[::-1]:
                if scope in symbol_table.scopes:
                    symbol_table.get_scope_by_id(scope).remove_all_variables()

        if not is_inside_user_defined_function:
            RAISE_ERROR("End function without being in Function block.")
        
        if is_inside_struct_c_function:
            RAISE_ERROR("Use \"endc_function\" to close a c function and not \"end_function\".")

        if class_fn_defination["function_destination"] == "class" and class_fn_defination["function_name"] == "__del__":
            # Write destructors for member variables of this class in reverse order.
            # TODO: This may be implemented as a macro written in ANIL itself.

            current_class = class_fn_defination["class_name"]

            struct_defination = get_struct_defination_of_type(current_class)
            if struct_defination is None:
                # Shouldn't happen.
                RAISE_ERROR(f"Internal Error: The current function we are writing is a destructor of {current_class} but {current_class} isn't a valid class.")
            else:
                # Write destructors in reverse order.
                for member in struct_defination.members[::-1]:
                    member_type = member.data_type

                    member_struct_def = get_struct_defination_of_type(member_type)
                    if member_struct_def is not None:
                        if member_struct_def.has_destructor():
                            destructor_mangled_fn_name = get_mangled_fn_name(member_type, "__del__")
                            emit(f"{destructor_mangled_fn_name}(&this->{member.member});\n")

        class_fn_defination["end_index"] = ctx.get_generated_lines_count()

        if class_fn_defination["function_destination"] == "class":
            start = class_fn_defination["start_index"]
            end = class_fn_defination["end_index"]

            # Extract function body (skip the function defination line).
            fn_body = "".join(ctx.get_generated_lines_slice(start + 1, end))

            # Remove the whole function (including function defination line).
            ctx.remove_generated_lines(start, end)

            # When using FUNCTION, C code is generated from ANIL code.
            # The c code is part of the function body and not needed in ctx.generated_lines.
            # So, remove it from ctx.generated_lines after adding the function body to the struct.
            if should_write_fn_body:
                GlobalStructInitCode += fn_body + "\n}\n\n"

            add_fnbody_to_member_to_struct(
                class_fn_defination["class_name"],
                class_fn_defination["function_name"],
                fn_body,
            )

            class_fn_defination["class_name"] = ""
            class_fn_defination["function_name"] = ""
            class_fn_defination["start_index"] = -1
            class_fn_defination["end_index"] = -1
            class_fn_defination["function_destination"] = "global"

            # Remove 'this*' StructInstance, so it doesn't mess up, as there can be different 'this*' parameters for different classes.
            remove_struct_instance("this")

        else:
            if should_write_fn_body:
                code = "}\n"
                emit(code)

        # Remove all the variables that were brought into scope by the function.
        for param in registered_function_parameters:
            remove_struct_instance(param)

        for struct in instanced_struct_names:
            if struct.scope in tracked_scopes_for_current_fn:
                remove_struct_instance(struct.struct_name)

        tracking_scopes_for_current_fn = False
        tracked_scopes_for_current_fn = []
        registered_function_parameters = []

        return_encountered_in_fn = False
        scopes_with_return_stmnt = []

        should_write_fn_body = True
        is_inside_user_defined_function = False
    elif parser.current_token() == lexer.Token.RETURN:
        parser.consume_token(lexer.Token.RETURN)

        if not parser.has_tokens_remaining():
            code_generator.emit_return_statement()
            continue

        result = boolean_expression()

        # Boolean expressions for Structs function call need to save the result
        # of the comparision temporarily somewhere.
        # It is because we write the destructors then the return expression.
        # Since, the destructors are called earlier, we dont have the struct to return
        # with a return statement.
        # For e.g:
        # ListObject__del__(&node);
        # return ListObject__eq__OVDint(&node, token);
        # ^^^^^^ So, we convert the above expression to following.
        # bool return_value = ListObject__eq__OVDint(&node, token);
        # ListObject__del__(&node);
        # return return_value;

        current_scope = get_current_scope()

        # return s
        #        ^  s shouldn't be freed, because it is returned.
        # Should be the job of the caller to free it.
        # TODO: For 'return s.f()'. This should be also handled too.
        # As of now, s.f() is a function call and function call generates temporary return variable automatically.
        if result.returns_single_value:
            for struct_involved in result.structs_involved:
                struct = get_instanced_struct(struct_involved)
                if struct == None:
                    continue

                if struct.scope != current_scope:
                    continue

                struct.should_be_freed = False
                # This loop should run for one iteration anyway(result.returns_single_value), 
                # so the loop could be unrolled.
                break

        structs_to_be_freed = []
        structs_with_destructors = 0

        if result.structs_involved:
            for scope in tracked_scopes_for_current_fn[::-1]:
                if scope not in symbol_table.scopes:
                    continue

                symbols = symbol_table.get_scope_by_id(scope).symbols
                for symbol in symbols:
                    struct_info = get_instanced_struct(symbol)
                    if struct_info != None:
                        if struct_info.struct_name in result.structs_involved:
                            if struct_info.should_be_freed:
                                structs_to_be_freed.append(struct_info.struct_name)
                            if struct_info.struct_type_has_destructor():
                                structs_with_destructors += 1


        create_temporary_for_return = False

        structs_returned_set = set(result.structs_involved)
        structs_freed_set = set(structs_to_be_freed)

        # If any of the structs involved in the return statement need their destructors called,
        # then we need to create a temporary variable to store the return value. 
        # By doing this, we can call the destructors and then return the value using the temporary variable.
        has_items_that_call_destructors = len(structs_returned_set.intersection(structs_freed_set)) > 0

        # return value == p_type
        # Given : p_type is not freed and value is freed.
        # Since value is freed, the length of intersection should be greater than 0.
        # That means we need to create a temporary return variable.
        if has_items_that_call_destructors:
            if structs_with_destructors > 0:
                create_temporary_for_return = True
        else:
            if "(" in result.return_value:
                # HACK: If it is a fn expression then some structs may be involved here.
                # Assuming that, for fn expressions, we create a temp variable to store the result.
                # in case the variables involved in fn expressions need their destructors called.
                # TODO: Properly parse structs involed in function call to figure out if their destructors need to be called.
                create_temporary_for_return = True

        if create_temporary_for_return:
            return_type = result.return_type
            if result.returns_single_value:
                if return_type == ParameterType.VARIABLE:
                    return_type = get_type_of_variable(result.structs_involved[0])
                elif return_type == ParameterType.STRING_EXPRESSION:
                    return_type = "struct String"
            code_generator.emit_variable_declaration(variable_type=return_type, variable_name="return_value", initialization_value=result.return_value)

        # Write destructors.
        for scope in tracked_scopes_for_current_fn[::-1]:
            if scope in symbol_table.scopes:
                destructor_code = symbol_table.get_scope_by_id(scope).get_destructor_code_for_all_variables()
                if destructor_code != "":
                    emit(destructor_code)

        if class_fn_defination["function_name"] in ("main", "WinMain"):
            if True or is_anil_file:
                emit("\n// GLOBAL_VARIABLES_DESTRUCTOR_CODE //\n")

        return_encountered_in_fn = True
        scopes_with_return_stmnt.append(current_scope)

        # Write return itself.
        # We assume we have single return statement.
        code_generator.emit_return_statement(return_value="return_value" if create_temporary_for_return else result.return_value)
    elif check_token(lexer.Token.NAMESPACE):
        if is_inside_name_space:
            RAISE_ERROR(f"Is already inside a namespace(\"{namespace_name}\"). Can't declare a new namespace.")
        parser.consume_token(lexer.Token.NAMESPACE)
        namespace_name = parser.get_token()

        struct_defination = get_struct_defination_of_type(namespace_name)
        if struct_defination is None:
            RAISE_ERROR(f"\"{namespace_name}\" isn't a valid namespace name. Namespace name is one of the classes'(struct) name. Namespaces are for implementing the member functions for the provided class(struct).")

        is_inside_name_space = True

        templated_type_mappings = {}
        # Suppose for Vector_int, template_defination_variable = T, templated_data_type = int,
        # we store {"T" : "int"}.
        # So, when we encounter T in function parameters or within this namespace, we replace it with int.
        if struct_defination.template_defination_variable and struct_defination.templated_data_type: 
            templated_type_mappings[struct_defination.template_defination_variable] = struct_defination.templated_data_type

        if struct_defination.unparsed_functions_emitted:
            # The temporary functions we added in instantiate_template() is cleared now.
            # Only after it has been emitted, because we may need function declarations that we created in
            # instantiate_template() in other namespaces before us.
            struct_defination.member_functions.clear()
            struct_defination.unparsed_functions_emitted = False

        is_namespace_for_templated_struct = struct_defination.is_templated()
        if is_namespace_for_templated_struct:
            # Gather all the functions to compile later.
            # We can't compile them immediately(at template defination time), 
            # because we need to know the template type,which is available at 
            # instantiation time.
            unparsed_fn = []
            fn = []
            j = index
            endnamespace_found = False
            while j < len(Lines):
                x = Lines[j].strip()

                # Gather all the text between function and endfunction.
                if x.startswith("function"):
                    # Get all the text till we receive endfunction.
                    fn_body = []
                    end_fn_found = False
                    while j < len(Lines):
                        inner_text = Lines[j].strip()
                        fn_body.append(inner_text)
                        # Replace the current line with empty string.
                        Lines[j] = ""
                        if inner_text.startswith("endfunction"):
                            end_fn_found = True
                            break

                        j += 1
                    
                    if not end_fn_found:
                        RAISE_ERROR(f"For line {x}, couldn't find endfunction for templated function.")

                    fn.extend(fn_body)
                elif x.startswith("c_function"):
                    # Get all the text till we receive endfunction.
                    c_fn_body = []
                    end_c_fn_found = False
                    while j < len(Lines):
                        inner_text = Lines[j].strip()
                        c_fn_body.append(inner_text)
                        # Replace the current line with empty string.
                        Lines[j] = ""
                        if inner_text.startswith("endc_function"):
                            end_c_fn_found = True
                            break

                        j += 1
                    
                    if not end_c_fn_found:
                        RAISE_ERROR(f"For line {x}, couldn't find endc_function for templated function.")

                    fn.extend(c_fn_body)                    


                # if x.startswith("endfunction") or x.startswith("endnamespace"):
                if x.startswith("endnamespace"):
                    endnamespace_found = True
                    break

                j += 1

            if not endnamespace_found:
                RAISE_ERROR(f"Couldn't find endnamespace for namespace {namespace_name}.")

            # if there is no \n in any line append it.
            for i in range(len(fn)):
                if not fn[i].endswith("\n"):
                    fn[i] += "\n"

            unparsed_fn = fn

            struct_defination.unparsed_functions.extend(unparsed_fn)
    elif check_token(lexer.Token.ENDNAMESPACE):
        parser.consume_token(lexer.Token.ENDNAMESPACE)
        if not is_inside_name_space:
            RAISE_ERROR("Isn't inside a namespace.First, declare a new namespace as \"namespace 'namespace_name'\"")
        
        if is_inside_user_defined_function:
            RAISE_ERROR('Use "endfunction" to close a function and not "endnamespace".')

        if len(GlobalGeneratedFunctionDeclarations) > 0:
            # Just for decoration.
            # Add \n after all the function declarations for a class.
            GlobalGeneratedFunctionDeclarations += "\n"

        namespace_name = ""
        templated_type_mappings = {}
        is_inside_name_space = False
    elif check_token(lexer.Token.TILDE):
        # ~a -> a.__del__() calls destructor to a if a has a destructor.
        parser.consume_token(lexer.Token.TILDE)

        if not is_inside_name_space:
            RAISE_ERROR("Manual Destructor calls(~) are only allowed inside a namespace.")

        # ~arr
        #  ^^^ member_name
        member_name = parser.get_token()
        arrindex = None

        if parser.has_tokens_remaining():
            if parser.check_token(lexer.Token.LEFT_SQUARE_BRACKET):
                parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                # ~ arr[i]
                #       ^ arrindex
                arrindex = parser.get_token()
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

        struct_defination = get_struct_defination_of_type(namespace_name)
        if struct_defination is None:
            RAISE_ERROR(f"[Internal Error]: \"{namespace_name}\" isn't a valid namespace name")

        member_type = struct_defination.get_type_of_member(member_name)
        # t* -> remove pointer
        member_type = member_type.replace("*","")

        member_type_struct_info = get_struct_defination_of_type(member_type)
        if member_type_struct_info != None:
            # Only structs have destructors, so write the destructors.
            # Skip for other types.
            if member_type_struct_info.has_destructor():
                is_refering_to_array_element = arrindex != None
                if is_refering_to_array_element:
                    # arr[i].__del__()
                    # ANIL_code = f"this.{member_name}[{arrindex}].__del__();\n"
                    # FIMXE: Unimplemented array function call.      
                    destructor_mangled_fn_name = get_mangled_fn_name(member_type, "__del__")
                    des_code = f"{destructor_mangled_fn_name}(&this->{member_name}[{arrindex}]);\n"                                  
                    insert_intermediate_lines(index, [des_code])
                else:
                    # arr.__del__()
                    ANIL_code = f"this.{member_name}.__del__();\n"
                    insert_intermediate_lines(index, [ANIL_code])
    else:
        emit(Line)

if len(global_variables_initialization_code) > 0 and not main_fn_found:
    print("====== Global Initialization Code ======")
    for g_code in global_variables_initialization_code:
        print(g_code)
    RAISE_ERROR("Couldn't find main function to place global variable initialization code.")

def get_initial_global_definations():
    combined_global_definitions = []
    if is_anil_file:
        # [ 
        #   "#include<stdio.h>",
        #   "#include<stdlib.h>",
        #   "#include<stdio.h>",
        # ]
        UniqueIncludeLines = []
        for line in IncludeLines:
            line = line.strip().replace(" ", "")
            if line not in UniqueIncludeLines:
                UniqueIncludeLines.append(line + "\n")

        combined_global_definitions += UniqueIncludeLines + ["\n\n"]
    combined_global_definitions += GlobalGeneratedStructDefinations + GlobalGeneratedFunctionDeclarations + "\n\n" + GlobalGeneratedConstexprLookupFnDefinations + "\n\n" + GlobalGeneratedStructDestructors + GlobalStructInitCode
    return combined_global_definitions

# A main function can have multiple return values.
# Every return needs to call destructors.
# TODO?? But does this work with actual multiple returns ??
destructor_for_all_global_variables = symbol_table.destructor_code_for_all_remaining_variables()

for i in range(ctx.get_generated_lines_count()):
    line = ctx.get_generated_line(i)
    if "// STRUCT_DEFINATIONS //" in line:
        ctx.set_generated_line(i, get_initial_global_definations())
    elif "// DESTRUCTOR_CODE //" in line:
        ctx.set_generated_line(i, "")
    elif "// HWND_VARIABLE_DECLARATIONS //" in line:
        ctx.set_generated_line(i, gui_manager.get_hwnd_variable_declaration_code())
    elif "// GUI_NODES_CREATION //" in line:
        ctx.set_generated_line(i, gui_manager.get_gui_nodes_creation_code())
    elif "// ASSIGN_GUI_OUTPUTS //" in line:
        ctx.set_generated_line(i, gui_manager.get_gui_assignment_code())
    elif "// INPUT_FIELD_AUTO_BIND_TO_STRING //"  in line:
        ctx.set_generated_line(i, gui_manager.get_all_input_field_update_code())
    elif "// GLOBAL_VARIABLES_DESTRUCTOR_CODE //" in line:
        ctx.set_generated_line(i, "\n" + destructor_for_all_global_variables)

ctx.write_final_code(output_file_name)

import subprocess

use_clang_format = True
if args.no_clang_format:
    use_clang_format = False

if use_clang_format:
    try:
        subprocess.run(["clang-format", "-i", output_file_name], check=True)
        print(f"Successfully compiled {output_file_name} & formatted using clang-format.")
    except subprocess.CalledProcessError as e:
        print(f"Successfully compiled {output_file_name} but error running clang-format: {e}")
