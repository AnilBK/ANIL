import Parser
import lexer
from ErrorHandler import ErrorHandler

from typing import Callable, Dict, Optional
from enum import Enum
import re
import os
import random
from collections import OrderedDict

import argparse

# We don't typically pass filenames through command line, this is mostly for batch compile operations.
filename_parser = argparse.ArgumentParser()
filename_parser.add_argument("--filename", help="Name of source file to be compiled.")
args = filename_parser.parse_args()

# source_file = "examples\\01_variables.c"
# source_file = "examples\\02_List.c"
# source_file = "examples\\03_Dictionary.c"
# source_file = "examples\\04_Classes.c"
# source_file = "examples\\04_b_Classes.c"
# source_file = "examples\\05_Strings.c"
# source_file = "examples\\06_Vector.c"
# source_file = "examples\\07_Set.c"

# source_file = "examples\\unique_ptr_source.c"
# source_file = "examples\\initializer_list.c"
# source_file = "examples\\Reflection.c"

# source_file = "examples\\constexpr_dict.c"
# source_file = "examples\\decorators_inside_fn_body.c"
# source_file = "examples\\enumerate_source.c"

# source_file = "examples\\fileexample.c"
# source_file = "examples\\function_example.c"

# source_file = "Bootstrap\\lexer_test.c"
# source_file = "Bootstrap\\Parser_test.c"
source_file = "Bootstrap\\preprocess_test.c"

if args.filename:
    source_file = args.filename


output_file_name = source_file.split(".")[0] + "_generated.c"

Lines = []
with open(source_file, "r") as file:
    Lines = file.readlines()

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
    for module_name in imported_modules:
        relative_path = "Lib\\" + module_name + ".c"
        module_file_path = os.path.join(os.getcwd(), relative_path)

        lines = []
        with open(module_file_path, "r") as module_file:
            lines = module_file.readlines()
        ImportedCodeLines += lines

    Lines = ImportedCodeLines + Lines


LinesCache = []

inside_match_loop = False
match_condition_count = 0
match_variable_name = ""
match_type = "variable"  # variable,struct
match_struct_type_info = []

struct_definations = []
instanced_struct_names = []
GlobalStructInitCode = ""

# Cached Items.
# If any structs have __init__ method, then we register them here.
# This could be stored in StructDefination.
structs_with_constructors = set()

def has_constructors(p_struct_type: str) -> bool:
    return p_struct_type in structs_with_constructors

# Map "Vector_String" as {"Vector_String":["Vector", "String"]} i.e
# store individual structs and the final mangled struct name.
templated_data_type_mapping = {}
# This list stores all the individual structs that make up a final templated data type temporarily.
# This list is then assigned to the appropriate key to the dictionary above.
templated_data_type_mapping_list = []

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
    db = {"char": "c", "int": "d", "float": "f", "size_t": "llu"}

    is_array_type = p_type[0] == "[" and p_type[-1] == "]"
    if is_array_type:
        p_type = p_type[1:-1]

    if p_type in db.keys():
        return db[p_type]
    else:
        return "d"


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
        data_type = ""
        if isinstance(param,str):
            data_type = param
        else:
            data_type = param.data_type
        if data_type == "char*":
            function_name += "str"
        else:
            #"struct String", we dont want those spaces in final function name.
            data_type_no_spaces = data_type.replace(" ", "")
            function_name += data_type_no_spaces
    return function_name
# UTILS END


class Symbol:
    def __init__(self, p_name, p_data_type):
        self.name = p_name
        self.data_type = p_data_type


class SymbolTable:
    def __init__(self):
        self.symbols = {}
        self.scope_stack = []

    def current_scope(self):
        if len(self.scope_stack) == 0:
            self.enter_scope()
        return self.scope_stack[-1]

    def new_unique_scope_id(self):
        if len(self.scope_stack) == 0:
            # We haven't created any scopes.
            # So, the first scope created is 0.
            return 0

        latest_scope = self.current_scope()
        new_scope = latest_scope + 1

        if new_scope in self.scope_stack:
            while True:
                random_index = random.randrange(100000)
                if not (random_index in self.scope_stack):
                    new_scope = random_index
                    break

        return new_scope

    def enter_scope(self):
        new_scope_id = self.new_unique_scope_id()
        self.scope_stack.append(new_scope_id)
        self.symbols[new_scope_id] = OrderedDict()

    def exit_scope(self):
        if self.scope_stack:
            exiting_scope_id = self.scope_stack.pop()
            destructor_code = self.destructor_for_all_variables_in_scope(
                exiting_scope_id
            )
            if destructor_code != "":
                LinesCache.append(destructor_code)

    def print_symbol_table(self):
        print("-------------------Symbol Table------------------")
        for scope in self.scope_stack:
            for name,symbol in self.symbols[scope].items():
                print(f"{scope} {name} {symbol.data_type}")
        print("-------------------------------------------------")

    def declare_variable(self, name, p_type):
        current_scope = self.current_scope()

        if name in self.symbols[current_scope]:
            self.print_symbol_table()
            RAISE_ERROR(f"Variable '{name}' already declared in this scope.")

        for scope in self.scope_stack:
            if name in self.symbols[scope]:
                self.print_symbol_table()
                RAISE_ERROR(
                    f"Variable '{name}' already declared in previous scope {scope}."
                )

        self.symbols[current_scope][name] = Symbol(name, p_type)

    def find_variable(self, name):
        for scope in reversed(self.scope_stack):
            if name in self.symbols[scope]:
                return self.symbols[scope][name]
        return None

    def destructor_for_all_variables_in_scope(self, scope_id):
        # Return the destructor code for all variables in the provided scope
        # And, free(unregister) those variables as well.
        des_code = ""
        if scope_id in self.symbols:
            for variable in reversed(self.symbols[scope_id]):
                # Call the destructor for the variable
                # print(f"Destroying variable '{variable}' in scope {scope_id}")
                code = get_destructor_for_struct(variable)
                if code != None:
                    # print(f"~() = {code}")
                    des_code += code
            del self.symbols[scope_id]
        return des_code

    def destructor_code_for_all_remaining_variables(self):
        destructor_code = ""
        while True:
            if self.scope_stack:
                exiting_scope_id = self.scope_stack.pop()
                des_code = self.destructor_for_all_variables_in_scope(exiting_scope_id)
                if des_code != "":
                    destructor_code += des_code
            else:
                break
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


def get_type_of_variable(p_var_name):
    var = symbol_table.find_variable(p_var_name)
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


class NestingLevel(Enum):
    FOR_LOOP = 0
    IF_STATEMENT = 1
    ELSE_STATEMENT = 2


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


class Struct:
    def __init__(self, p_struct_name: str, p_members: list) -> None:
        self.name = p_struct_name
        self.members = p_members
        self.member_functions = []
        self.macro_definations = []
        self.is_class_templated = False
        self.templated_data_type = ""
        self.template_defination_variable = ""

    def is_templated(self) -> bool:
        return self.is_class_templated

    def has_destructor(self) -> bool:
        return any(fn.is_destructor() for fn in self.member_functions)

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
            if is_data_type_struct_object(type):
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


class StructInstance:

    def __init__(
        self,
        p_struct_type,
        p_struct_name,
        p_is_templated: bool,
        p_templated_data_type: str,
        p_scope,
    ) -> None:
        self.struct_type = p_struct_type
        self.struct_name = p_struct_name
        self.is_templated = p_is_templated
        self.templated_data_type = p_templated_data_type

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

    def is_templated_instance(self) -> bool:
        return self.is_templated

    def get_struct_defination(self) -> Optional[Struct]:
        StructInfo = get_struct_defination_of_type(self.struct_type)
        return StructInfo

    def struct_type_has_destructor(self) -> bool:
        return self.get_struct_defination().has_destructor()

    def _validate_template_type(self):
        if self.templated_data_type == "":
            # Shouldn't happen, as this validation is called if is_templated_instance() is true.
            RAISE_ERROR(
                "[Fatal Error] Struct is templated but it's type isn't Registered."
            )

    def get_mangled_function_name(self, p_fn_name: str, parameters = None) -> str:
        if self.is_templated_instance():
            self._validate_template_type()
            return get_templated_mangled_fn_name(
                self.struct_type, p_fn_name, self.templated_data_type
            )
        else:
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
            return struct_info.is_return_type_of_fn_ref_type(p_fn_name, self.templated_data_type if self.is_templated_instance() else "")

    def get_return_type_of_fn(self, p_fn_name, parameters = None):
        return_type = None
        
        struct_info = self.get_struct_defination()
        if struct_info.function_is_overloaded(p_fn_name):
            return_type = struct_info.get_return_type_of_overloaded_fn(p_fn_name, parameters if parameters != None else [])
        else:
            return_type = struct_info.get_return_type_of_fn(p_fn_name)
        
        if self.is_templated_instance():
            # struct<T>{..}
            # return T
            if return_type == struct_info.template_defination_variable: 
                return_type = self.templated_data_type

        if is_data_type_struct_object(return_type):
            # For Vector<String>, the String here is of struct type.
            # TODO : templated_data_type should probably be 'struct String' instead of just 'String' to avoid all this comparision.
            return_type = f"struct {return_type}"

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
    for defined_struct in struct_definations:
        if defined_struct.name == p_struct_type:
            return defined_struct
    return None


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


def is_instanced_struct(p_struct_name: str):
    return any(struct.struct_name == p_struct_name for struct in instanced_struct_names)


def get_instanced_struct(p_struct_name) -> Optional[StructInstance]:
    for struct in instanced_struct_names:
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
                if struct.struct_type_has_destructor():
                    destructor_fn_name = struct.get_destructor_fn_name()
                    des_code = f"{destructor_fn_name}(&{struct_name});\n"
                    return des_code
    return None


class ObjectInstance:
    def __init__(
        self, p_struct_name, p_is_templated, p_templated_data_type=None
    ) -> None:
        self.struct_name = p_struct_name
        self.is_templated = p_is_templated
        self.templated_data_type = p_templated_data_type


# This stores which structs are instanced, so to fix redefination error while generating C code.
ObjectInstances = []


def is_class_already_instantiated(p_struct_name, p_is_templated, p_templated_data_type):
    for ObjInstance in ObjectInstances:
        struct_name = ObjInstance.struct_name
        is_templated = ObjInstance.is_templated
        templated_data_type = ObjInstance.templated_data_type

        struct_name_matches = struct_name == p_struct_name
        template_matches = False

        # Both the template and the templated type matches.
        if is_templated and p_is_templated:
            if templated_data_type == p_templated_data_type:
                template_matches = True

        # Normal, untemplated classes.
        if (not is_templated) and (not p_is_templated):
            template_matches = True

        if struct_name_matches and template_matches:
            return True
    return False


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


constexpr_dictionaries = []


def is_constexpr_dictionary(p_dict_name) -> bool:
    return any(m_dict.dict_name == p_dict_name for m_dict in constexpr_dictionaries)


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

Error_Handler = ErrorHandler()
Error_Handler.register_source_file(source_file)

def RAISE_ERROR(error_msg):
    Error_Handler.raise_error(error_msg)

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

    if not is_inside_new_code:
        # Normal C code, so just write that.
        LinesCache.append(Line)
        continue

    if is_inside_struct_c_function and not "endc_function" in Line:
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

    def create_const_charptr_iterator(array_name, current_array_value_variable):
        global LinesCache
        global temp_c_str_iterator_variable_count

        iterator_var_name = f"{array_name}_iterator_{temp_c_str_iterator_variable_count}"

        LinesCache.append(
            f"char *{iterator_var_name} = {array_name};"
            f"while (*{iterator_var_name} != '\\0') {{"
            f"char {current_array_value_variable} = *{iterator_var_name};"
            f"{iterator_var_name}++;"
        )

        REGISTER_VARIABLE(current_array_value_variable, "char")

        temp_c_str_iterator_variable_count += 1

    def create_normal_array_iterator(array_name, current_array_value_variable):
        global LinesCache

        # The variable type is in format '[int]'.
        array_type = get_type_of_variable(array_name)
        if array_type == None:
            RAISE_ERROR(f"{array_type} isn't a registered array type.")

        array_type = array_type[1:-1]

        LinesCache.append(
            f"for (unsigned int i = 0; i < {array_name}_array_size; i++){{\n"
            f"{array_type} {current_array_value_variable} = {array_name}[i];\n"
        )

        REGISTER_VARIABLE(current_array_value_variable, array_type)

    def create_array_iterator_from_struct(array_name, current_array_value_variable):
        global temp_arr_length_variable_count
        global for_loop_depth

        step_size = "1"
        #for x in list[::-1]
        #             ^^^^^^
        if parser.has_tokens_remaining():
            if parser.current_token() == lexer.Token.LEFT_SQUARE_BRACKET:
                parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                
                parser.consume_token(lexer.Token.COLON)
                parser.consume_token(lexer.Token.COLON)

                step_size = get_integer_expression("Expected integer expression for step_size.")
                
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

        loop_indices = "ijklmnopqrstuvwxyzabcdefgh"
        loop_counter_index = loop_indices[for_loop_depth % 26]

        temporary_len_var_name = f"tmp_len_{temp_arr_length_variable_count}"

        # Emit CPL code to perform looping.
        # This line will be parsed by the compiler in next line.
        
        code = []
        
        if step_size[0] == "-":
            # Starts with Negative index.
            l1 = f"let {temporary_len_var_name} = {array_name}.len() - 1\n"
            l2 = f"{temporary_len_var_name} -= 1;\n"
            l3 = f"for {loop_counter_index} in range({temporary_len_var_name}..=0,{step_size}){{\n"
            l4 = f"let {current_array_value_variable} = {array_name}[{loop_counter_index}]\n"
            code = [l1, l2, l3, l4]
        else:
            l1 = f"let {temporary_len_var_name} = {array_name}.len()\n"
            l2 = f"for {loop_counter_index} in range(0..{temporary_len_var_name}){{\n"
            l3 = f"let {current_array_value_variable} = {array_name}[{loop_counter_index}]\n"
            code = [l1, l2, l3]

        insert_intermediate_lines(index, code)

        temp_arr_length_variable_count += 1

    def create_array_enumerator(
        array_name, ranged_index_item_variable, current_array_value_variable
    ):
        global LinesCache
        LinesCache.append(
            f"Iterator {array_name}_iter = create_iterator_from_array({array_name}, {array_name}_array_size); \n"
            f"Enumerator {array_name}_enumerator;\n"
            f"{array_name}_enumerator.index = -1;\n\n"
            f"while (enumerate(&{array_name}_iter, &{array_name}_enumerator)) {{\n"
            f"int {ranged_index_item_variable} = {array_name}_enumerator.index;\n"
            f"int {current_array_value_variable} = {array_name}_enumerator.val;\n"
        )

    def parse_data_type(inner=False):
        # inner is used as a tag for name mangling during recursive data parsing.

        # String,Vector<String>
        # This all should be parsed.
        # String
        # ^^^^^^
        data_type = parser.get_token()

        if data_type == "str":
            # str is an alias for char*, even though in most places we have checks for both "char*" and "str".
            data_type = "char*"

        data_type_str = data_type

        struct_defination = get_struct_defination_of_type(data_type)
        is_struct_type = struct_defination != None

        is_templated = False

        global templated_data_type_mapping
        global templated_data_type_mapping_list
        
        if not inner:
            templated_data_type_mapping_list = []
        
        if is_struct_type:
            templated_data_type_mapping_list.append(data_type_str)

            if not inner:
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

            inner_data_type = parse_data_type(inner=True)
            data_type_str += f"_{inner_data_type}"
            parser.consume_token(lexer.Token.GREATER_THAN)

            instantiate_template(data_type, inner_data_type)

        if is_templated:
            #Convert "struct Vector_String" to "Vector_String"
            stripped = data_type_str
            if data_type_str.startswith("struct "):
                stripped = data_type_str[len("struct "):]
            templated_data_type_mapping[stripped] = templated_data_type_mapping_list
            templated_data_type_mapping_list = []

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

        array_element_count = len(array_values)
        array_elements_str = ",".join(array_values)

        global LinesCache
        LinesCache.append(f"{type_name} {array_name}[] = {{ {array_elements_str} }};\n")
        LinesCache.append(
            f"unsigned int {array_name}_array_size = {array_element_count};\n\n"
        )

        # register variable as '[int]' to indicate the array of type int.
        REGISTER_VARIABLE(array_name, f"[{type_name}]")

    def instantiate_template(struct_type, templated_data_type):
        # Immediately instantiate templated member functions.

        StructInfo = get_struct_defination_of_type(struct_type)
        if StructInfo == None:
            return

        global GlobalStructInitCode

        # Recreate Generic Structs on instantiation.
        m_struct_name = f"{struct_type}_{templated_data_type}"
        m_struct_info = get_struct_defination_of_type(m_struct_name)
        if m_struct_info != None:
            # This struct has already been defined.
            # So, we don't need to instantiate again.
            return

        struct_code = f"struct {m_struct_name}  {{\n"

        struct_members_list = StructInfo.members
        for struct_member in struct_members_list:
            type = struct_member.data_type
            mem = struct_member.member
            is_generic = struct_member.is_generic
            if is_generic:
                if "*" in type:
                    actual_type, star = type.split("*")
                    type = templated_data_type + "*"
                else:
                    type = templated_data_type

                if is_data_type_struct_object(templated_data_type):
                    type = f"struct {type}"

            struct_code += f"{type} {mem};\n"
        struct_code += f"}};\n\n"

        # Register this templated struct in order to insantiate the same generic type in the future.
        struct_data = Struct(m_struct_name, struct_members_list)
        # struct_data.is_class_templated = True
        # if is_data_type_struct_object(templated_data_type):
        #     struct_data.templated_data_type = f"struct {templated_data_type}"

        GlobalStructInitCode += struct_code

        templated_fn_code = f"//template {struct_type}<{templated_data_type}> {{\n"

        if StructInfo != None:
            defined_struct = StructInfo
            for fn in defined_struct.member_functions:
                parameters = fn.fn_arguments
                fn_name = fn.fn_name
                return_type = fn.return_type

                # This is just a raw unmodified copy, we need this to recreate the function defination, otherwise these parameters are modified below.
                parameters_copy = parameters

                if fn.is_overloaded_function:
                    # print("======================================")
                    # print(f"For struct {defined_struct.name}")
                    # print(
                    # f"fn {fn_name} is overloaded for class {fn.overload_for_template_type}"
                    # )

                    use_this_overload = False
                    if fn.overload_for_template_type == templated_data_type:
                        # This current fn is the suitable overload.
                        use_this_overload = True
                    else:
                        has_suitable_overload = False
                        for m_fn in defined_struct.member_functions:
                            if m_fn.fn_name != fn_name:
                                continue

                            if m_fn.overload_for_template_type == templated_data_type:
                                has_suitable_overload = True

                        if has_suitable_overload:
                            use_this_overload = False
                        else:
                            # TODO: We don't warn if there is no base overload.
                            if fn.overload_for_template_type == "#BASE#":
                                # No other suitable overloads found,
                                # and this fn is the base overload.
                                # Use this current fn to overload.
                                use_this_overload = True

                    if not use_this_overload:
                        continue

                # resolve templated fn params.
                if defined_struct.is_templated():
                    # print(f"Template type {templated_data_type}")

                    params_copy = []
                    for param in parameters:
                        data_type = param.data_type
                        param_name = param.member
                        if data_type == defined_struct.template_defination_variable:
                            data_type = templated_data_type
                            if is_data_type_struct_object(data_type):
                                data_type = "struct " + data_type

                        params_copy.append(data_type + " " + param_name)
                    parameters = params_copy

                    if return_type == defined_struct.template_defination_variable:
                        return_type = templated_data_type
                        if is_data_type_struct_object(return_type):
                            return_type = "struct " + return_type

                    # Register this function, but if it is templated, resolved the templated type and write the function.
                    m_fn = MemberFunction(fn_name, parameters_copy, return_type)
                    # m_fn.is_overloaded_function = is_overloaded_fn
                    # m_fn.overload_for_template_type = overload_for_type
                    struct_data.member_functions.append(m_fn)
                else:
                    struct_data.member_functions.append(fn)

                templated_struct_name = defined_struct.name + f"_{templated_data_type}"

                fn_name = get_templated_mangled_fn_name(
                    defined_struct.name, fn_name, templated_data_type
                )

                if len(parameters) > 0:
                    parameters_str = ",".join(parameters)
                    templated_fn_code += f"{return_type} {fn_name}(struct {templated_struct_name} *this, {parameters_str}) {{\n"
                else:
                    templated_fn_code += f"{return_type} {fn_name}(struct {templated_struct_name} *this) {{\n"
                templated_fn_code += f"{fn.fn_body} }}\n\n"
            templated_fn_code += (
                f"//template {struct_type}<{templated_data_type}> }}\n\n"
            )

            # if we want to use template type in fn body, we use following syntax.
            # @TEMPLATED_DATA_TYPE@
            if "@TEMPLATED_DATA_TYPE@" in templated_fn_code:
                m_templated_data_type = templated_data_type
                if is_data_type_struct_object(templated_data_type):
                    m_templated_data_type = "struct " + m_templated_data_type

                templated_fn_code = templated_fn_code.replace(
                    "@TEMPLATED_DATA_TYPE@", m_templated_data_type
                )

            GlobalStructInitCode += templated_fn_code

            global struct_definations
            struct_definations.append(struct_data)

    def parse_create_struct(struct_type, struct_name):
        global LinesCache

        is_templated_struct = False
        templated_data_type = ""

        StructInfo = get_struct_defination_of_type(struct_type)
        if StructInfo is None:
            RAISE_ERROR(f'Struct type "{struct_type}" undefined.')

        if parser.check_token(lexer.Token.SMALLER_THAN):
            parser.next_token()
            templated_data_type = parser.current_token()
            # print(f"Templated struct of data type : {templated_data_type}")
            struct_members_list = StructInfo.members
            # print(StructInfo.members)  # [['X', 'a', True], ['float', 'b', False]]

            # GlobalStructInitCode += struct_code

            is_templated_struct = True

            parser.next_token()

            parser.consume_token(lexer.Token.GREATER_THAN)

        instanced_struct_info = StructInstance(
            struct_type,
            struct_name,
            is_templated_struct,
            templated_data_type,
            get_current_scope(),
        )

        global instanced_struct_names
        instanced_struct_names.append(instanced_struct_info)

        REGISTER_VARIABLE(struct_name, struct_type)

        class_already_instantiated = is_class_already_instantiated(
            struct_type, is_templated_struct, templated_data_type
        )

        if not class_already_instantiated:
            # GlobalStructInitCode += struct_code
            global ObjectInstances
            ObjectInstances.append(
                ObjectInstance(struct_type, is_templated_struct, templated_data_type)
            )

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

        code = ""

        if is_templated_struct:
            code += f"struct {struct_type}_{templated_data_type} {struct_name};\n"
        else:
            code += f"struct {struct_type} {struct_name};\n"

        global GlobalStructInitCode

        # Immediately instantiate templated member functions.
        if is_templated_struct and (not class_already_instantiated):
            instantiate_template(struct_type, templated_data_type)

        has_constuctor = has_constructors(struct_type)

        if has_constuctor:
            values_str = ""
            if len(values_list) > 0:
                values_str = ",".join(values_list)

            LinesCache.append(code)
            constructor_CPL_code = f"{struct_name}.__init__({values_str})\n"

            index_to_insert_at = index
            Lines.insert(index_to_insert_at, constructor_CPL_code)
            return
        else:
            struct_var_values_pairs = list(zip(StructInfo.members, values_list))
            for struct_member, values in struct_var_values_pairs:
                # type = struct_member[0]
                mem = struct_member.member

                code += f"{struct_name}.{mem} = {values};\n"

        LinesCache.append(code)

    class ParameterType(Enum):
        UNDEFINED = -1
        RAW_STRING = 0
        CHAR_TYPE = 1
        STR_TYPE = 2
        STRING_CLASS = 3
        NUMBER = 4
        VARIABLE = 5
        BOOLEAN_CONSTANT = 6 # True or False

    class Parameter:
        def __init__(self, p_param, p_param_type: ParameterType) -> None:
            self.param = p_param
            self.param_type = p_param_type
            self.struct_instance = None # For a.b, we may need instance info of b later, for e.g in boolean_expression()
            
    def _read_a_parameter():
        # Parse a number or string..
        # In any case, just a single symbol.
        parameter = None
        parameter_type = ParameterType.UNDEFINED

        tk = parser.current_token()

        if tk == lexer.Token.QUOTE:
            parameter = escape_quotes(parser.extract_string_literal())
            parameter_type = ParameterType.RAW_STRING
            return Parameter(parameter, parameter_type)

        parser.save_checkpoint()
        fn_call_parse_info = function_call_expression()
        if fn_call_parse_info == None:
            parser.rollback_checkpoint()
            parser.clear_checkpoint()
        else:
            parse_result = fn_call_parse_info.function_call_metadata
            return_type = parse_result["return_type"]
            value = fn_call_parse_info.get_fn_str()

            parameter = value
            parameter_type = return_type
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
                if "struct_instance" in parse_result:
                    parameter = Parameter(parameter, parameter_type)
                    parameter.struct_instance = parse_result["struct_instance"]
                    return parameter

                return Parameter(parameter, parameter_type)
        
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
            elif tk == "false":
                parameter = tk
                parameter_type = ParameterType.BOOLEAN_CONSTANT

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

            curr_param = parser.current_token()
            if curr_param != lexer.Token.RIGHT_ROUND_BRACKET:
                # There are few other tokens rather than ).
                # That should be a comma to indicate next parameter.
                parser.consume_token(lexer.Token.COMMA)
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        return parameters
    
    def _parameters_to_types_str_list(parameters : list) -> list:
        strs = []
        for param in parameters:
            if param.param_type == ParameterType.NUMBER or param.param_type == "int":
                strs.append("int")
            elif param.param_type == ParameterType.CHAR_TYPE:
                strs.append("char")
            elif param.param_type == ParameterType.RAW_STRING or param.param_type == ParameterType.STR_TYPE:
                strs.append("char*")
            elif param.param_type == ParameterType.VARIABLE:
                strs.append(get_type_of_variable(param.param))
            elif param.param_type == ParameterType.STRING_CLASS or param.param_type == "struct String":
                strs.append("struct String")
            else:
                RAISE_ERROR(f"Unimplemented for {param.param_type}.")
        return strs


    class ParsedFunctionCallType(Enum):
        # To denote x = A.fn();
        STRUCT_FUNCTION_CALL = 0
        # To denote x = fn();
        GLOBAL_FUNCTION_CALL = 1
        # To denote x = A.B;
        MEMBER_ACCESS_CALL =  2

    def _parse_struct_member_access(tk, base_struct_info, child_struct_info, member_access_string, pointer_access):
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
                RAISE_ERROR(f"{tk} isn't an instanced struct.")
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

            struct_instance = StructInstance(
                type_of_tk,
                f"tmp_struct_name_{str(val)}",
                False,
                "",
                get_current_scope(),
            )

            child_struct_info = struct_instance.get_struct_defination()

            member_access_string += f"{pointer_access}{tk}"
            pointer_access = "."
        return base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance

    class ParseErrorException(Exception):
        def __init__(self, message):
            self.message = message
            super().__init__(self.message)

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

            struct_instance = StructInstance(
                type_of_tk,
                f"tmp_struct_name_{str(val)}",
                False,
                "",
                get_current_scope(),
            )

            child_struct_info = struct_instance.get_struct_defination()

            member_access_string += f"{pointer_access}{tk}"
            pointer_access = "."
        return base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance

    def parse_access_struct_member(var_name):
        # let str = str2[0]
        #     ^^^   ^^^^ ^
        #     |     |    |
        #     |     |    .________ parameters
        #     |     ._____________ target
        #     .___________________ varname
        fn_call_parse_info = function_call_expression()
        if fn_call_parse_info == None:
            RAISE_ERROR("Parsing function expression failed.")

        parse_result = fn_call_parse_info.function_call_metadata
        fn_name = parse_result["fn_name"]
        return_type = parse_result["return_type"]
        is_return_type_ref_type = parse_result["is_return_type_ref_type"]
        has_parameters = parse_result["has_parameters"]
        parsed_fn_call_type = parse_result["function_call_type"]
        member_access_string = parse_result["member_access_string"]

        if "struct" in return_type:
            #              "struct Vector__String"
            # return type   ^^^^^^^^^^^^^^^^^^^^^
            # raw_return_type      ^^^^^^^^^^^^^^
            raw_return_type = return_type.split("struct")[1].strip()

            global instanced_struct_names

            instance = StructInstance(
                raw_return_type, var_name, False, "", get_current_scope()
            )

            # Map mangled struct type, which performs same as this commented code.
            # if raw_return_type == "Vector_String":
            #     instance = StructInstance(
            #         "Vector", var_name, True, "String", get_current_scope()
            #     )
            if raw_return_type in templated_data_type_mapping:
                template_types = templated_data_type_mapping[raw_return_type]
                instance = StructInstance(
                    template_types[0], var_name, True, template_types[1], get_current_scope()
                )  

            if is_return_type_ref_type:
                instance.should_be_freed = False
                
            instanced_struct_names.append(instance)

        REGISTER_VARIABLE(var_name, return_type)

        assignment_code = f"{return_type} {var_name} = {fn_name}("

        if parsed_fn_call_type == ParsedFunctionCallType.STRUCT_FUNCTION_CALL:
            assignment_code += f"{member_access_string}"
            if has_parameters:
                assignment_code += ","

        if has_parameters:
            parameters_str = parse_result["parameters_str"]
            assignment_code += f"{parameters_str}"
        assignment_code += ");"

        global LinesCache
        LinesCache.append(f"{assignment_code}\n")

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

                    struct_def = get_struct_defination_of_type(class_name)
                    if struct_def == None:
                        RAISE_ERROR(f"{class_name} class isn't registered.")
                    
                    def _get_members():
                        return [stringify(member.member) for member in struct_def.members]

                    def _get_member_functions():
                        return [stringify(fn.fn_name) for fn in struct_def.member_functions]

                    def _get_instances():
                        global instanced_struct_names
                        return [stringify(struct.struct_name) 
                                for struct in instanced_struct_names 
                                if struct.struct_type == class_name]
                    
                    constexpr_map: Dict[str, Callable] = {
                        "members_of": _get_members,
                        "member_functions_of": _get_member_functions,
                        "instances_of_class": _get_instances
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
                new_code.append(line)

        insert_intermediate_lines(index, new_code)

    def parse_constexpr_dictionary(p_dict_name):
        parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
        key = parser.extract_string_literal()
        parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

        for dict in constexpr_dictionaries:
            if dict.dict_name == p_dict_name:
                try:
                    return dict.dictionary[key]
                except KeyError:
                    print(
                        f'[Error] Key "{key}" wasn\'t found in the constexpr dictionary {p_dict_name}.'
                    )
        # Shouldn't happen as the caller functions have already verified the presence of the dictionaries.
        RAISE_ERROR(f"Constexpr dictionary {p_dict_name} is undefined.")

    def parse_function_declaration():
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

                return_type = parse_data_type()
                parser.consume_token(lexer.Token.COLON)

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
    
    class SpeculativeTokenType(Enum):
        NONE = 0
        NUMERIC_CONSTANT = 1 # 42
        NUMERIC_VARIABLE = 2 # a = 45
        STRING_LITERAL = 3 # a = "Hello World"
        STR_TYPE = 4
        STRING_CLASS = 5 # a = String{"Hello World"}
        CHAR_TYPE = 6
        STRING_EXPRESSION = 7 # "StringStrip(&s1, 1, 5)"

    class SpeculativeParseInfo:
        def __init__(self):
            self.speculative_token_type = SpeculativeTokenType.NONE
            self.speculative_token_value = ""
            self.speculative_exact_type = ""
    
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

        if is_constexpr_dictionary(current_token):
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
        elif current_token.isdigit():
            return_value = current_token
            exact_type = "int"
            token_type = SpeculativeTokenType.NUMERIC_CONSTANT
            parser.next_token()

        if return_value == None:
            # Nothing is number as of now.
            parser.save_checkpoint()
            fn_call_parse_info = function_call_expression()
            if fn_call_parse_info == None:
                parser.rollback_checkpoint()
                parser.clear_checkpoint()
            else:
                parse_result = fn_call_parse_info.function_call_metadata
                return_type = parse_result["return_type"]
                if return_type == "int" or return_type == "size_t":
                    return_value = fn_call_parse_info.get_fn_str()
                else:
                    parser.rollback_checkpoint()
                    parser.clear_checkpoint()

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

        current_token = parser.current_token()
        if current_token == lexer.Token.QUOTE:
            # str = "Hello World"
            string = parser.extract_string_literal()
            return_value = f"\"{string}\""
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
            parser.save_checkpoint()
            fn_call_parse_info = function_call_expression()
            if fn_call_parse_info == None:
                parser.rollback_checkpoint()
                parser.clear_checkpoint()
            else:
                parse_result = fn_call_parse_info.function_call_metadata
                return_type = parse_result["return_type"]
                if return_type in {"struct String", "String", "str"}:
                    return_value = fn_call_parse_info.get_fn_str()
                    token_type = SpeculativeTokenType.STRING_EXPRESSION
                else:
                    parser.rollback_checkpoint()
                    parser.clear_checkpoint()

            if return_value is None:
                if is_variable_string_class(current_token):
                    return_value = current_token
                    exact_type = "struct String"
                    token_type = SpeculativeTokenType.STRING_CLASS
                    parser.next_token()
                    
        if return_value is not None:
            parse_info = SpeculativeParseInfo()
            parse_info.speculative_token_type = token_type
            parse_info.speculative_exact_type = exact_type
            parse_info.speculative_token_value = return_value
            return parse_info
        return None

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
                    code = f"{fn_name}({member_access_string}, {parameters_str})"
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

    def speculative_parse_string_expression():
        expr = ""

        while parser.has_tokens_remaining():
            parse_info = speculative_parse_string()
            if parse_info == None:
                break

            # a = 1
            tk = parse_info.speculative_token_value
            expr += tk
            break
        
        if expr == "":
            return None
        else:
            expression_info = SpeculativeExpressionInfo()
            expression_info.speculative_expression_type = SpeculativeExpressionType.STRING_EXPRESSION
            expression_info.speculative_expression_value = expr
            return expression_info

    def _quote_string_params(actual_fn_args, provided_parameters):
        parameters_quoted = []
        for arg, parameter in zip(actual_fn_args, provided_parameters):
            param = parameter.param
            param_type = parameter.param_type

            if param_type == ParameterType.RAW_STRING:
                if (arg == "char*") or (arg == "str"):
                    param = f'"{param}"'
                else:
                    param = f"'{param}'"
                parameters_quoted.append(Parameter(param, param_type))
            else:
                parameters_quoted.append(parameter)
        return parameters_quoted

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
            
            parsing_fn_call_type = ParsedFunctionCallType.MEMBER_ACCESS_CALL

            parser.next_token()

            type_of_tk = child_struct_info.get_type_of_member(tk)
            if type_of_tk is None:
                RAISE_ERROR(f"Struct doesn't have member {tk}.")

            val = random.randrange(100000)

            struct_instance = StructInstance(
                type_of_tk,
                f"tmp_struct_name_{str(val)}",
                False,
                "",
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

            parser.clear_checkpoint()
            expression_info = SpeculativeExpressionInfo()
            expression_info.speculative_expression_type = SpeculativeExpressionType.FUNCTION_CALL_EXPRESSION
            expression_info.speculative_expression_value = ""
            expression_info.function_call_metadata = additional_data
            return expression_info
    
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
                    c_str_fn_name = param_struct_info.get_mangled_function_name("c_str")
                    # A function expects a string, & we provided a String class object.
                    # So, we perform getting string from String Class object.
                    # var -> Stringc_str(&var)
                    # ^^^ String Object.
                    parameters[i].param = f"{c_str_fn_name}(&{parameters[i].param})"

        if char_to_string_promotion_code != "":
            global LinesCache
            LinesCache.append(f"{char_to_string_promotion_code}\n")

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
            "struct_instance": struct_instance
        }

    def get_comparision_operator():
        """Read comparision operator (if any) which includes >, <, >=, <=, ==, !=, in"""
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

        return {
            "has_comparision_operator": has_comparision_operator,
            "operators_as_str": operators_as_str,
        }

    def promote_char_to_string(var_to_check):                             
        global temp_char_promoted_to_string_variable_count
        
        promoted_char_var_name = f"{var_to_check}_promoted_{temp_char_promoted_to_string_variable_count}"
        temp_char_promoted_to_string_variable_count += 1

        LinesCache.append(f"char {promoted_char_var_name}[2] = {{ {var_to_check}, '\\0'}};\n")
        REGISTER_VARIABLE(f"{promoted_char_var_name}", "str")

        return promoted_char_var_name

    def handle_equality(var_to_check_against, var_to_check, l_type, r_type, left_struct_info, is_lhs_struct, negation):
        operator = "!=" if negation else "=="
        if l_type == ParameterType.STR_TYPE and r_type == ParameterType.STR_TYPE:
            return f"strcmp({var_to_check}, {var_to_check_against}) {operator} 0"

        if r_type == ParameterType.RAW_STRING:
            return handle_raw_string_equality(var_to_check_against, var_to_check, l_type, left_struct_info, is_lhs_struct, negation)

        if is_lhs_struct:
            return handle_struct_equality(var_to_check_against, var_to_check, r_type, left_struct_info, negation)

        if l_type == ParameterType.CHAR_TYPE:
            return handle_char_equality(var_to_check_against, var_to_check, l_type, negation)

        # if Char == "\""
        if var_to_check == '"':
            return f'{var_to_check_against} {operator} "{var_to_check}"'
        else:
            comparision_code = (
                f'{var_to_check_against} == {var_to_check}'
            )
            return f"!({comparision_code})" if negation else comparision_code

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
        parameters = [Parameter(var_to_check, r_type)]

        fn_name = struct_info.get_mangled_function_name("__eq__", _parameters_to_types_str_list(parameters))
        return_type = struct_info.get_return_type_of_fn("__eq__", _parameters_to_types_str_list(parameters))
        code = f'{fn_name}(&{var_to_check_against}, {var_to_check})'

        if negation:
            code = f"!{code}"

        return {"code": code, "return_type": return_type}

    def handle_char_equality(var_to_check_against, var_to_check, l_type, negation):
        code = f"{var_to_check_against} == '{var_to_check}'"
        return f"!({code})" if negation else code

    def handle_array_in_operator(var_to_check, var_to_check_against):
        global temp_arr_search_variable_count

        search_variable = f"{var_to_check_against}__contains__{var_to_check}_{temp_arr_search_variable_count}"
        temp_arr_search_variable_count += 1

        LinesCache.append(
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

    def boolean_expression():
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

            negation = operators_as_str == "!="
            
            if operators_as_str in {"==", "!="}:
                return handle_equality(
                    var_to_check_against, var_to_check, l_type, r_type,
                    left_struct_info, is_lhs_struct, negation
                )
            elif operators_as_str == "in":
                # if var_to_check in var_to_check_against {
                var_to_check = lhs["value"]
                var_to_check_against = rhs["value"]

                if is_rhs_struct:
                    fn_name = right_struct_info.get_mangled_function_name("__contains__")
                    contains_fn_args = right_struct_info.get_fn_arguments("__contains__")

                    is_var_to_check_string_object = is_lhs_struct and left_struct_info.struct_type == "String"

                    # Get the data type for the first function argument.
                    param_type = contains_fn_args[0].data_type
                    expects_string_argument = param_type in {"char*", "str"}

                    if l_type == ParameterType.CHAR_TYPE:
                        gen_code = ""
                        # if var_to_check in var_to_check_against {
                        #    ^^^^^^^^^^^^ this is a char
                        # but the function signature for "__contains__" expects a char*/str.
                        # So, promote the char variable to a string.
                        if expects_string_argument:
                            var_to_check = promote_char_to_string(var_to_check)

                    if l_type == ParameterType.RAW_STRING:
                        gen_code = (
                            f'{fn_name}(&{var_to_check_against}, "{var_to_check}")'
                        )
                    elif expects_string_argument and is_var_to_check_string_object:
                        # For Automatic Conversion for String class.
                        # for token in Character_Tokens:
                        #     ^^^^     ^^^^^^^^^^^^^^^^
                        #         String               Dictionary
                        # The __contains__ for Dictionary expects a string,
                        # And the string class has a c_str method that returns char*,
                        # so implement this for String Class. Could be extended to any classes with c_str() method. Food for thought :)
                        c_str_fn_name = left_struct_info.get_mangled_function_name("c_str")
                        gen_code = f"{fn_name}(&{var_to_check_against}, {c_str_fn_name}(&{var_to_check}))"
                    else:
                        if right_struct_info.is_pointer_type:
                            gen_code = f"{fn_name}({var_to_check_against}, {var_to_check})"
                        else:
                            gen_code = f"{fn_name}(&{var_to_check_against}, {var_to_check})"
                    return gen_code
                else:
                    if is_variable_array_type(var_to_check_against):
                        return handle_array_in_operator(var_to_check, var_to_check_against)

                    RAISE_ERROR(f"Target variable {var_to_check_against} is undefined. It is neither an array nor a struct.")

            return f"{lhs['value']} {operators_as_str} {rhs['value']}"


        return lhs["value"]

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
                INDEXED_MEMBER_ACCESS = 3 # a[3] = ..
            expression_type = ExpressionType.ASSIGNMENT

            struct_tk = tk

            while True:
                if child_struct_info != None:
                    if child_struct_info.has_member_fn(tk) or child_struct_info.has_macro(tk):
                        expression_type = ExpressionType.FUNCTION_CALL
                        break

                base_struct_info, child_struct_info, member_access_string, pointer_access, struct_instance = _parse_struct_member_access(tk, base_struct_info, child_struct_info, member_access_string, pointer_access)
            
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
                    fn_call_parse_info = function_call_expression()
                    if fn_call_parse_info == None:
                        RAISE_ERROR("Parsing function expression failed.")

                    code = fn_call_parse_info.get_fn_str()
                    LinesCache.append(f"{code};\n")
                elif StructInfo.has_macro(tk):
                    # macro type functinons
                    struct_name = StructInfo.name
                    parse_macro(struct_name, "CLASS_MACRO", tk)
                    LinesCache.append(f"//Class Macro.\n")
                else:
                    RAISE_ERROR(f"FATAL ERROR(Should never happen):{tk} is neither a struct macro nor a member function.")
                continue
            
            parsed_member = member_access_string
            # We are generating CPL code, so all these C specific value accessors(a->b etc) are removed.
            # This will be generated again(as required) when parsing the CPL code in next line.
            # _parse_struct_member_access() above added these symbols, we remove them now.
            parsed_member = parsed_member.replace("->",".")
            if parsed_member[0] == "&":
                parsed_member = parsed_member[1:]

            if expression_type == ExpressionType.ADD:
                # str += "World"
                #      ^
                parser.consume_token(lexer.Token.EQUALS)

                add_fn = "__add__"
                lines = []

                while parser.has_tokens_remaining():
                    gen_code = ""

                    parse_info = speculative_parse_string()
                    if parse_info == None:
                        RAISE_ERROR("Expected string literal or String object for + operator.")
                    else:
                        string_tk = parse_info.speculative_token_value
                        if parse_info.speculative_token_type == SpeculativeTokenType.STRING_EXPRESSION:
                            #left_part += string_to_insert + s1.substr(p_index, s1.len() - p_index)
                            #                                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ This will return C expression.
                            # i.e string_tk = Stringsubstr(&s1, p_index, Stringlen(&s1) - p_index).
                            # This can't be passed to {parsed_member}.{add_fn}({string_tk}), as
                            # string_tk is C Expression.
                            # In this case we directly generate the appropriate C expression.
                            temp_string_var_name = f"tmp_string_{temp_string_object_variable_count}"
                            temp_string_object_variable_count += 1
                            instance = StructInstance(
                                "String", f"{temp_string_var_name}", False, "", get_current_scope()
                            )
                            # instance.is_pointer_type = True
                            # instance.should_be_freed = False
                            instanced_struct_names.append(instance)

                            LinesCache.append(f"struct String {temp_string_var_name} = {string_tk};\n")
                            REGISTER_VARIABLE(f"{temp_string_var_name}", f"String")
                            string_tk = f"{temp_string_var_name}"
                        gen_code = f"{parsed_member}.{add_fn}({string_tk})\n"
                        
                    # Emit CPL code to perform addition.
                    # This line will be parsed by the compiler in next line.
                    # The conversion of String class to char*, char to char*,
                    # all these are handled by the compiler during function call parse.
                    lines.append(gen_code)

                    if not parser.has_tokens_remaining():
                        # token += Char
                        #              ^
                        break

                    #str += "World" + str2 + "str3" + str4
                    #               ^
                    parser.consume_token(lexer.Token.PLUS)
                    if not parser.has_tokens_remaining():
                        #str += "World" + 
                        #                 ^
                        RAISE_ERROR("Expected string literal or String object after + operator.")

                insert_intermediate_lines(index, lines)
                continue                                
            elif expression_type == ExpressionType.ASSIGNMENT:
                data_type = struct_instance.struct_type
                if data_type in {"int", "bool", "float", "char"}: 
                    value = parser.get_token()
                    LinesCache.append(f"{member_access_string} = {value};\n")
                else:
                    reassign_CPL_code = f"{parsed_member}.__reassign__("
                    if parser.check_token(lexer.Token.QUOTE):
                        # str = "Reassign"
                        value = parser.extract_string_literal()
                        reassign_CPL_code += f"\"{value}\""
                    else:
                        # token = Char #TODO : Not implemented properly.
                        value = parser.current_token()
                        reassign_CPL_code += f"{value}"
                    reassign_CPL_code += ")\n"
                    index_to_insert_at = index  
                    Lines.insert(index_to_insert_at, reassign_CPL_code)
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
                LinesCache.append(f"{parsed_member}(); \n")
                continue
            else:
                # TODO ??
                RAISE_ERROR(f"UserDefined Function : {parsed_member} calling void functions can't take parameters as of now. ")
        elif is_variable_boolean_type(parsed_member):
            # escape_back_slash = False
            parser.next_token()

            parser.consume_token(lexer.Token.EQUALS)

            curr_token = parser.get_token()
            is_true_token = curr_token == lexer.Token.TRUE
            is_false_token = curr_token == lexer.Token.FALSE

            if is_true_token:
                LinesCache.append(f"{parsed_member} = true; \n")
            elif is_false_token:
                LinesCache.append(f"{parsed_member} = false; \n")
            else:
                RAISE_ERROR("Expected a boolean value.")
            continue
    else:
        LinesCache.append("\n")
        continue

    if check_token(lexer.Token.RIGHT_CURLY):

        top_of_stack = nesting_levels[-1]

        if top_of_stack == NestingLevel.FOR_LOOP:
            _ = nesting_levels.pop(-1)
            for_loop_depth -= 1
            decrement_scope()
            LinesCache.append("}\n")
        elif top_of_stack == NestingLevel.IF_STATEMENT:
            _ = nesting_levels.pop(-1)
            decrement_scope()
            LinesCache.append("}\n")

            if len(parser.tokens) >= 2:
                parser.next_token()
                if check_token(lexer.Token.ELSE):
                    parser.next_token()
                    LinesCache.append("else")

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
                        LinesCache.append("{\n")
                        nesting_levels.append(NestingLevel.ELSE_STATEMENT)
                        increment_scope()
        elif top_of_stack == NestingLevel.ELSE_STATEMENT:
            _ = nesting_levels.pop(-1)
            decrement_scope()
            LinesCache.append("}\n")
        else:
            RAISE_ERROR("UnImplemented Right Curly.")
    elif check_token(lexer.Token.HASH):
        # Comments.
        # Just create a split at the first '#'.
        parser.next_token()

        if check_token(lexer.Token.INCLUDE):
            # this has to handle #include<string.h> as well.
            LinesCache.append(Line)
        else:
            parts = Line.split("#", 1)
            line_without_hash = parts[1]
            LinesCache.append(f" //{line_without_hash}")
    elif inside_match_loop:
        if "%}" in Line:
            inside_match_loop = False
            # LinesCache.append("}\n")
        elif match_type == "variable":
            # 1 => printf("One");
            rhs = Line.split("=>")
            match_condition_value = rhs[0].strip()
            statements = rhs[1].strip()

            is_else_condition = False
            if match_condition_count == 0:
                LinesCache.append("if ")
            else:
                if match_condition_value == "_":
                    is_else_condition = True
                    LinesCache.append("else")
                else:
                    LinesCache.append("else if")

            if is_else_condition:
                LinesCache.append(f"{{\n")
            else:
                # 3 | 4 | 5
                if "|" in match_condition_value:
                    values_to_test = match_condition_value.split("|")

                    statement_list = []
                    for value in values_to_test:
                        statement_list.append(f"{match_variable_name} == {value}")
                    combined_statement = " || ".join(statement_list)
                    LinesCache.append(f" ({combined_statement}){{\n")
                elif "..." in match_condition_value:
                    # 5 ... 10
                    operands = match_condition_value.split("...")
                    start_range = operands[0]
                    end_range = operands[1]
                    LinesCache.append(
                        f" ( {match_variable_name} >= {start_range} && {match_variable_name} <= {end_range} ){{\n"
                    )
                else:
                    LinesCache.append(
                        f" ( {match_variable_name} == {match_condition_value}){{\n"
                    )
            LinesCache.append(f"    {statements} \n}}\n")

            match_condition_count += 1
        elif match_type == "struct":
            # (0, 0) => printf("Origin.");
            rhs = Line.split("=>")
            match_condition = rhs[0].strip()
            statements = rhs[1].strip()

            type, name = match_struct_type_info
            # print(match_struct_type_info)

            match_condition = match_condition.split("(")[1]
            match_condition = match_condition.split(")")[0]
            conds = match_condition.split(",")

            # Get all the members for the given struct type.
            StructInfo = get_struct_defination_of_type(type)
            if StructInfo is None:
                RAISE_ERROR("Struct undefined.")
            members = StructInfo.members

            cond_struct_members_pair = list(zip(conds, members))
            condition_list = []
            assignment_list = []

            # (x,y,z)
            are_all_conditions_assignments = all(
                conds.strip().isnumeric() for conds, memb in cond_struct_members_pair
            )

            for conds, memb in cond_struct_members_pair:
                conds = conds.strip()
                if conds == "_":
                    continue
                elif conds.isnumeric():
                    condition_list.append(f"({name}.{memb} ==  {conds})")
                    # print(f"if({name}.{memb} ==  {conds}){{")
                else:
                    assignment_list.append(f"int {conds} = {name}.{memb};")
                    # print(f"float {conds} = {name}.{memb};")

            # print(" && ".join(condition_list))
            conditions_joined = " && ".join(condition_list)

            # Bug: #(x,y,z) condition works only on the last arm of match.
            # (x,y,z) is final pattern same as using _.
            # so there can be only one such condition ;D
            # if are_all_conditions_assignments:
            #    LinesCache.append("else{")
            if match_condition_count == 0:
                LinesCache.append("if")
            else:
                LinesCache.append("else if")

            if len(condition_list) == 0:
                # (x,y,z), (_,_,_)
                conditions_joined = "true"
            LinesCache.append(f"({conditions_joined})\n{{\n")

            if len(assignment_list) > 0:
                assignment_joined = "\n".join(assignment_list)
                LinesCache.append(f"    {assignment_joined} \n\n")
            LinesCache.append(f"    {statements} \n}}\n")

            match_condition_count += 1
    elif parser.current_token() == "print":
        # print("Hello {meaning}")
        parser.next_token()

        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
        actual_str = parser.extract_string_literal()
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)

        braces_open = False
        str_text = ""
        extracted_var_name_list = []
        extracted_var_name = ""
        for char in actual_str:
            if char == "{":
                braces_open = True
                extracted_var_name = ""
            elif char == "}":
                braces_open = False
                format_specifier = "d"
                return_type = get_type_of_variable(extracted_var_name)
                if return_type != None:
                    format_specifier = get_format_specifier(return_type)
                str_text += f"%{format_specifier}"
                extracted_var_name_list.append(extracted_var_name)
            elif braces_open:
                extracted_var_name += char
            else:
                str_text += char

        str_to_write = f'printf("{str_text}"'
        if len(extracted_var_name_list) != 0:
            str_to_write += "," + ",".join(extracted_var_name_list)
        str_to_write += ");\n"
        LinesCache.append(str_to_write)
    elif check_token(lexer.Token.MATCH):
        # % match x{
        parser.consume_token(lexer.Token.MATCH)
        match_variable_name = parser.get_token()
        # print(f"Matching variable {match_variable_name}")
        parser.match_token(lexer.Token.LEFT_CURLY)

        inside_match_loop = True
        match_condition_count = 0

        is_Struct = False
        user_struct_type = ""
        type = ""
        name = ""
        # [struct_type,struct_name]
        for s in instanced_struct_names:
            type = s.struct_type
            name = s.struct_name
            if name == match_variable_name:
                is_Struct = True
                user_struct_type = type
                break

        if is_Struct:
            match_struct_type_info = [type, name]
            match_type = "struct"  # variable,struct
        else:
            match_type = "variable"  # variable,struct
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
            # print(f"Obtained String : {string}")
            LinesCache.append(f'char {string_name}[{len(string)+1}] = "{string}";\n')
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
                    integer_expression = speculative_parse_integer_expression()
                    if integer_expression == None:
                        RAISE_ERROR(f"Expected integer expression for {array_name}.")
                    integer_value = integer_expression.speculative_expression_value
                    LinesCache.append(f"{POD_type} {array_name} = {integer_value};\n")
                    REGISTER_VARIABLE(array_name, f"{POD_type}")
                    continue
                elif POD_type == "char":
                    parser.consume_token(lexer.Token.QUOTE)
                    char_value = parser.get_token()
                    if len(char_value) != 1:
                        RAISE_ERROR(f"Char value should be of length 1 but got \"{char_value}\" of length {len(char_value)}.")
                    parser.consume_token(lexer.Token.QUOTE)
                    LinesCache.append(f"{POD_type} {array_name} = \'{char_value}\';\n")
                    REGISTER_VARIABLE(array_name, f"{POD_type}")
                    continue
                else:
                    RAISE_ERROR(
                        f'Parsing POD Type "{POD_type}" not Implemented as of now.'
                    )
            if parser.check_token(lexer.Token.QUOTE):
                string = parser.extract_string_literal()
                # let str = "Hello World";
                #           ^
                # ^^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.
                # let str = String{"Hello World"};

                # Emit CPL code to create a struct 'String' object.
                # This line will be parsed by the compiler in next line.
                CPL_code = f"let {array_name} = String{{\"{string}\"}};\n"

                index_to_insert_at = index
                Lines.insert(index_to_insert_at, CPL_code)
                continue
            elif parser.check_token(lexer.Token.TRUE) or parser.check_token(
                lexer.Token.FALSE
            ):
                # let inside_string = False
                #                     ^
                REGISTER_VARIABLE(array_name, "bool")
                if parser.check_token(lexer.Token.TRUE):
                    LinesCache.append(f"bool {array_name} = true;\n")
                else:
                    LinesCache.append(f"bool {array_name} = false;\n")
            elif parser.check_token(lexer.Token.LEFT_SQUARE_BRACKET):
                #let test_list = [];
                parser.consume_token(lexer.Token.LEFT_SQUARE_BRACKET)
                parser.consume_token(lexer.Token.RIGHT_SQUARE_BRACKET)

                #This will be converted to:
                #     let test_list = List{};
                #And, ^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.

                # Emit CPL code to create a struct 'List' object.
                # This line will be parsed by the compiler in next line.
                CPL_code = f"let {array_name} = List{{ }};\n"

                index_to_insert_at = index
                Lines.insert(index_to_insert_at, CPL_code)
                continue
            elif parser.check_token(lexer.Token.LEFT_CURLY):
                #Parse Dictionary Type.
                #let dict = {};
                parser.consume_token(lexer.Token.LEFT_CURLY)
                parser.consume_token(lexer.Token.RIGHT_CURLY)

                #This will be converted to:
                #     let dict = Dictionary{};
                #And, ^^^^^^^^^^^^^^^^^^^^^^ this line will generate the required C Code.

                # Emit CPL code to create a struct 'Dictionary' object of Lib/Dictionary.
                # This line will be parsed by the compiler in next line.
                CPL_code = f"let {array_name} = Dictionary{{ }};\n"

                index_to_insert_at = index
                Lines.insert(index_to_insert_at, CPL_code)
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
                        actual_value = parse_constexpr_dictionary(target)
                        LinesCache.append(f"int {var_name} = {actual_value};\n")
                    else:
                        parse_access_struct_member(var_name)
    elif check_token(lexer.Token.IF):
        nesting_levels.append(NestingLevel.IF_STATEMENT)
        increment_scope()

        parser.consume_token(lexer.Token.IF)
        code = boolean_expression()
        parser.consume_token(lexer.Token.LEFT_CURLY)

        LinesCache.append(f"\nif({code}){{\n")
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

        array_name = parser.get_token()
        
        if is_enumerated_for_loop:
            create_array_enumerator(
                array_name, ranged_index_item_variable, current_array_value_variable
            )
        else:
            if is_variable_const_char_ptr(array_name):
                create_const_charptr_iterator(array_name, current_array_value_variable)
            elif is_instanced_struct(array_name):
                # This generates new CPL code which creates a for loop(which makes a new scope).
                # We already have created a scope above by increment scope.
                # We call decrement_scope() to compensate that.
                for_loop_depth -= 1
                decrement_scope()
                create_array_iterator_from_struct(
                    array_name, current_array_value_variable
                )
            elif array_name == "range":
                # If we have some list named range, then it will be parsed earlier above ^^^.
                # for i in range(1..10){    -> 0 >= i < 10
                # for i in range(1..10,2){  -> 0 >= i < 10
                # for i in range(1..=10,2){ -> 0 >= i <= 10
                #                       ^     step
                #                    ^        stop
                #                ^            start
                parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)

                includes_stop = False
                step = '1'

                start = get_integer_expression("Exptected integer expression for start value.")

                parser.consume_token(lexer.Token.DOT)
                parser.consume_token(lexer.Token.DOT)

                if parser.check_token(lexer.Token.EQUALS):
                    # for i in range(1..=10,2){ -> 0 >= i <= 10
                    #                   ^                
                    parser.next_token()
                    includes_stop = True
                
                stop = get_integer_expression("Exptected integer expression for stop value.")

                # for i in range(1..10,2){  -> 0 >= i < 10
                #                     ^
                if parser.check_token(lexer.Token.COMMA):
                    parser.next_token()

                    step = get_integer_expression("Exptected integer expression for step value.")

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
                    LinesCache.append(
                        f"for (size_t {current_array_value_variable} = {start}; {current_array_value_variable} {stopping_condition} (size_t) - 1; {current_array_value_variable}{increment_operation}){{\n"
                    )
                elif step[0] == "-":
                    LinesCache.append(
                        f"for (size_t {current_array_value_variable} = {stop}; {current_array_value_variable} {stopping_condition} {start}; {current_array_value_variable}{increment_operation}){{\n"
                    )
                else:
                    LinesCache.append(
                        f"for (size_t {current_array_value_variable} = {start}; {current_array_value_variable} {stopping_condition} {stop}; {current_array_value_variable}{increment_operation}){{\n"
                    )
                REGISTER_VARIABLE(current_array_value_variable, "size_t")
            else:
                create_normal_array_iterator(array_name, current_array_value_variable)
        
        parser.match_token(lexer.Token.LEFT_CURLY)
        
    elif check_token(lexer.Token.STRUCT):
        #  struct Point {T x, T y };
        parser.consume_token(lexer.Token.STRUCT)
        struct_name = parser.get_token()

        is_struct_templated = False

        generic_data_types = []

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

        # print(f"Found Struct Defination : {struct_name}")

        parser.consume_token(lexer.Token.LEFT_CURLY)

        struct_members_list = []

        while parser.current_token() != lexer.Token.RIGHT_CURLY:
            struct_member_type = parse_data_type() #parser.get_token()
            if struct_member_type.startswith("struct "):
                struct_member_type = struct_member_type[len("struct "):]
            
            pointerless_struct_member_type = struct_member_type
            if parser.check_token(lexer.Token.ASTERISK):
                struct_member_type += "*"
                parser.next_token()
            struct_member = parser.current_token()

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
            GlobalStructInitCode += struct_code

        struct_definations.append(struct_data)
        # Non generic structs shouldn't be written out early, but since the c_function blocks write out functions despite being templated we leave the base templated struct defined, so that the funtions generated don't have defination error.
        # GlobalStructInitCode += struct_code

        # LinesCache.append(code)
        # [struct_name,x,y,z..]
    elif check_token(lexer.Token.CFUNCTION):
        # c_function say(Param1:type1, Param2:type2, ... ParamN:typeN)
        # void say(struct Point *this) { printf("x : %d , y : %d \n", this->x, this->y); }

        # Skip "c_function", its not a keyword, just a placeholder to identify a function call.
        parser.consume_token(lexer.Token.CFUNCTION)

        if not is_inside_name_space:
            RAISE_ERROR("c_function blocks are only allowed inside a namespace. Namespaces denote that the current function being defined belongs to that namespace(i.e class).")
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

        code = ""

        is_fn_constructor_type = fn_name == "__init__"

        # __init__Vector
        # Primitive Name Mangling.
        if is_fn_constructor_type:
            # fn_name = fn_name + struct_name
            structs_with_constructors.add(struct_name)
            
        unmangled_name = fn_name

        if is_struct_templated:
            should_write_fn_body = False
        else:
            if is_overloaded:
                fn_name = get_overloaded_mangled_fn_name(struct_name, fn_name, parameters)
            else:
                fn_name = get_mangled_fn_name(struct_name, fn_name)

            StructInfo = get_struct_defination_of_type(return_type)
            if StructInfo is not None:
                return_type = f"struct {return_type}"

            if len(parameters) > 0:
                parameters_str = ",".join(parameters_combined_list)
                code = f"{return_type} {fn_name}(struct {struct_name} *this, {parameters_str}) {{\n"
            else:
                code = f"{return_type} {fn_name}(struct {struct_name} *this) {{\n"

        currently_reading_fn_name = unmangled_name
        currently_reading_fn_parent_struct = struct_name
        currently_reading_parameters = parameters
        currently_reading_fn_name_unmangled = unmangled_name
        currently_reading_return_type = return_type

        fn = MemberFunction(unmangled_name, parameters, return_type)
        fn.is_overloaded_function = is_overloaded_fn
        fn.overload_for_template_type = overload_for_type
        fn.is_return_type_ref_type = is_return_type_ref_type

        add_fn_member_to_struct(struct_name, fn)

        c_function_body_start_pos = len(GlobalStructInitCode)

        GlobalStructInitCode += code
        is_inside_struct_c_function = True
    elif check_token(lexer.Token.ENDCFUNCTION):
        if is_inside_user_defined_function:
            RAISE_ERROR("Use \"endfunction\" to close a function and not \"endc_function\".")

        if is_inside_struct_c_function:
            if should_write_fn_body:
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

            add_fnbody_to_member_to_struct(
                currently_reading_fn_parent_struct,
                currently_reading_fn_name,
                currently_reading_fn_body_dup,
            )
            currently_reading_fn_body = ""
            should_write_fn_body = True
            is_inside_struct_c_function = False
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

        while True:
            key = parser.extract_string_literal()

            parser.consume_token(lexer.Token.COLON)

            value = parser.get_token()

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
        constexpr_dictionaries.append(m_dict)
    elif check_token(lexer.Token.AT):
        # @apply_hook("custom_integer_printer", CustomPrint)
        parser.consume_token(lexer.Token.AT)
        parser.consume_token(lexer.Token.APPLY_HOOK)
        parser.consume_token(lexer.Token.LEFT_ROUND_BRACKET)
        HOOKS_hook_fn_name = parser.extract_string_literal()
        parser.consume_token(lexer.Token.COMMA)
        HOOKS_target_fn = parser.get_token()
        parser.consume_token(lexer.Token.RIGHT_ROUND_BRACKET)
    elif check_token(lexer.Token.FUNCTION):
        # function say()
        # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN)
        # function say(Param1 : type1, Param2 : type2 ... ParamN : typeN) -> return_type
        parser.consume_token(lexer.Token.FUNCTION)

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

        increment_scope()
        curr_scope = get_current_scope()

        if is_inside_name_space:
            #print(f"Registering this for {namespace_name}.")
            instance = StructInstance(
                f"{namespace_name}", "this", False, "", curr_scope
            )
            instance.is_pointer_type = True
            instance.should_be_freed = False

            #is_overloaded = instance.get_struct_defination().function_is_overloaded(function_name)

            instanced_struct_names.append(instance)
            REGISTER_VARIABLE("this", f"{namespace_name}")

        # Register the parameters so they can be used inside function body.
        # Mark them such that their destructors wont be called.
        for param in parameters:
            # This could be done in parameters parsing loop above,
            # but we are doing this here so that FUNCTION body is same as C_FUNCTION.

            param_type = param.data_type
            param_name = param.member
            if "struct" in param_type:
                param_type = param_type.split("struct")[1]
                param_type = param_type.strip()

            if is_data_type_struct_object(param_type):
                instance = StructInstance(
                    param_type, param_name, False, "", curr_scope
                )

                # Function parameters shouldn't be freed at the end of the scope.
                # So, add a tag.
                instance.should_be_freed = False

                instanced_struct_names.append(instance)
            REGISTER_VARIABLE(param_name, param_type)


        defining_fn_for_custom_class = False
        if parser.has_tokens_remaining():
            # function my_first_CPL_function() for String
            if parser.check_token(lexer.Token.FOR):
                parser.consume_token(lexer.Token.FOR)

                target_class = parser.get_token()

                if is_inside_name_space:
                    RAISE_ERROR(f"Is already inside a namespace \"{namespace_name}\". Doesn't need to define target class(\"{target_class}\") for the provided function using the FOR keyword.")

                defining_fn_for_custom_class = True
                class_fn_defination["class_name"] = target_class
                class_fn_defination["function_name"] = function_name
                class_fn_defination["start_index"] = len(LinesCache)

                func_name = get_mangled_fn_name(class_fn_defination["class_name"], function_name)

        if not defining_fn_for_custom_class:
            if is_inside_name_space:
                defining_fn_for_custom_class = True
                class_fn_defination["class_name"] = namespace_name
                class_fn_defination["function_name"] = function_name
                class_fn_defination["start_index"] = len(LinesCache)

                if is_overloaded:
                    func_name = get_overloaded_mangled_fn_name(class_fn_defination["class_name"], function_name, parameters)
                else:
                    func_name = get_mangled_fn_name(class_fn_defination["class_name"], function_name)

        code = ""
        struct_name = class_fn_defination["class_name"]

        has_parameters = len(parameters) > 0

        code = f"{return_type} {func_name}("
        if defining_fn_for_custom_class:
            code += f"struct {struct_name} *this"
            if has_parameters:
                code += ","

            is_fn_constructor_type = function_name == "__init__"
            if is_fn_constructor_type:
                structs_with_constructors.add(struct_name)
                
        if has_parameters:
            parameters_str = ",".join(parameters_combined_list)
            code += f"{parameters_str}"
        code += f") {{\n"

        LinesCache.append(code)
        
        fn = MemberFunction(function_name, parameters, return_type)
        fn.is_overloaded_function = is_overloaded_fn
        fn.overload_for_template_type = overload_for_type
        fn.is_return_type_ref_type = is_return_type_ref_type
        
        if defining_fn_for_custom_class:
            GlobalStructInitCode += code
            add_fn_member_to_struct(class_fn_defination["class_name"], fn)
            class_fn_defination["function_destination"] = "class"
        else:
            GlobalFunctions.append(fn)
            class_fn_defination["function_destination"] = "global"

        is_inside_user_defined_function = True
    elif check_token(lexer.Token.ENDFUNCTION):
        decrement_scope()
        # ^^^^^^^^^^^^^^^^ This calls destructors.
        # Since, we have return statement, that handles the destructors.
        # If no return, then this should be performed.
        # TODO : Check this logic once.
        # TODO : What for void functions??
        # They don't have return statements but their destructors should be called.
        # As of now the destructors aren't called.

        if not is_inside_user_defined_function:
            RAISE_ERROR("End function without being in Function block.")
        
        if is_inside_struct_c_function:
            RAISE_ERROR("Use \"endc_function\" to close a c function and not \"end_function\".")

        code = "}\n"
        LinesCache.append(code)
        class_fn_defination["end_index"] = len(LinesCache)

        if class_fn_defination["function_destination"] == "class":
            fn_body = ""
            for i in range(class_fn_defination["start_index"] + 1, class_fn_defination["end_index"]-1):
                line = LinesCache[i]
                # print(line)
                fn_body += line

            del LinesCache[class_fn_defination["start_index"] : class_fn_defination["end_index"]]
            # When using FUNCTION, C code is generated from CPL code.
            # The c code is part of the function body and not needed in LinesCache.
            # So, remove it from LinesCache after adding the function body to the struct.

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

            # TODO:Should other StructInstances be freed after leaving a scope.
            # Remove 'this*' StructInstance, so it doesn't mess up, as there can be different 'this*' parameters for different classes.
            i = 0
            for struct in instanced_struct_names:
                if struct.struct_name == "this":
                    # print(f"Removing this from {namespace_name}.")
                    del instanced_struct_names[i]
                    break
                i += 1

        is_inside_user_defined_function = False
    elif parser.current_token() == lexer.Token.RETURN:
        parser.consume_token(lexer.Token.RETURN)

        result = boolean_expression()

        # Boolean expressions for Structs function call need to save the result
        # of the comparision temporarily somewhere.
        # It is because we write the destructors then the return expression.
        # Since, the destructors are called earlier, we dont have the struct to return
        # with a return statement.
        # For e.g:
        # CPLObject__del__(&node);
        # return CPLObject__eq__OVDint(&node, token);
        # ^^^^^^ So, we convert the above expression to following.
        # bool return_name = CPLObject__eq__OVDint(&node, token);
        # CPLObject__del__(&node);
        # return return_name;

        create_temporary = False
        
        if isinstance(result, Dict):
            # isinstance(result,dict) -> doesn't work, so we use Dict.
            # Since Dict is an alias for dict.
            fn_return_type = result["return_type"]
            fn_return_code = result["code"]
            create_temporary = True
            LinesCache.append(f"{fn_return_type} return_name = {fn_return_code};\n")

        # return s
        #        ^  s shouldn't be freed, because it is returned.
        # Should be the job of the caller to free it.
        i = 0
        for struct in instanced_struct_names:
            if (
                struct.struct_name == result
                and struct.scope == get_current_scope()
            ):
                instanced_struct_names[i].should_be_freed = False
                break
            i += 1

        # Write destructors.
        decrement_scope()
        # Write return itself.
        # We assume we have single return statement.
        if create_temporary:
            LinesCache.append(f"return return_name;\n")
        else:
           LinesCache.append(f"return {result};\n")
    elif check_token(lexer.Token.NAMESPACE):
        if is_inside_name_space:
            RAISE_ERROR(f"Is already inside a namespace(\"{namespace_name}\"). Can't declare a new namespace.")
        parser.consume_token(lexer.Token.NAMESPACE)
        namespace_name = parser.get_token()
        if not is_data_type_struct_object(namespace_name):
            RAISE_ERROR(f"\"{namespace_name}\" isn't a valid namespace name. Namespace name is one of the classes'(struct) name. Namespaces are for implementing the member functions for the provided class(struct).")
        is_inside_name_space = True
    elif check_token(lexer.Token.ENDNAMESPACE):
        parser.consume_token(lexer.Token.ENDNAMESPACE)
        if not is_inside_name_space:
            RAISE_ERROR("Isn't inside a namespace.First, declare a new namespace as \"namespace 'namespace_name'\"")
        namespace_name = ""
        is_inside_name_space = False
    else:
        LinesCache.append(Line)

for i in range(len(LinesCache)):
    if "// STRUCT_DEFINATIONS //" in LinesCache[i]:
        LinesCache[i] = GlobalStructInitCode
    elif "// DESTRUCTOR_CODE //" in LinesCache[i]:
        LinesCache[i] = symbol_table.destructor_code_for_all_remaining_variables()

with open(output_file_name, "w") as outputFile:
    for Line in LinesCache:
        outputFile.writelines(Line)

import subprocess

try:
    subprocess.run(["clang-format", "-i", output_file_name], check=True)
    print(f"Successfully compiled {output_file_name} & formatted using clang-format.")
except subprocess.CalledProcessError as e:
    print(f"Error running clang-format: {e}")
