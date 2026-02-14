from CompilerContext import CompilerContext


class CCodeGenerator:
    def __init__(self, compiler_context: CompilerContext):
        self.compiler_context = compiler_context

    def emit(self, code: str):
        self.compiler_context.emit(code)

    def emit_comment(self, comment: str):
        self.emit(f"// {comment}\n")

    def emit_reassignment(self, variable_name: str, value: str):
        self.emit(f"{variable_name} = {value};\n")

    def emit_variable_declaration(
        self, variable_type: str, variable_name: str, initialization_value: str = ""
    ):
        if initialization_value:
            self.emit(f"{variable_type} {variable_name} = {initialization_value};\n")
        else:
            self.emit(f"{variable_type} {variable_name};\n")

    def emit_string_literal_declaration(self, string_name: str, string: str):
        self.emit(f'char {string_name}[{len(string)+1}] = "{string}";\n')

    def emit_char_to_str(self, str_var_name: str, char_content: str):
        self.emit(f"char {str_var_name}[2] = {{ {char_content}, '\\0'}};\n")

    def emit_string_object_from_literal(self, var_name: str, string_literal: str):
        self.emit(f"struct String {var_name}; \n")
        self.emit(
            f'String__init__from_charptr(&{var_name}, "{string_literal}", {len(string_literal)}); \n'
        )

    def emit_boolean_reassignment(self, variable_name: str, value: bool):
        bool_str = "true" if value else "false"
        self.emit_reassignment(variable_name, bool_str)

    def emit_function_call(self, function_name: str, arguments: list = []):
        args_str = ", ".join(arguments)
        self.emit(f"{function_name}({args_str});\n")

    def emit_return_statement(self, return_value: str = ""):
        if return_value:
            self.emit(f"return {return_value};\n")
        else:
            self.emit("return;\n")

    def get_array_size_variable_name(self, array_name: str) -> str:
        return f"{array_name}_array_size"

    def get_array_element(self, array_name: str, index: str) -> str:
        return f"{array_name}[{index}]"

    def emit_array_declaration(self, array_type: str, array_name: str, elements: list):
        array_element_count = len(elements)
        array_elements_str = ",".join(elements)

        self.emit(f"{array_type} {array_name}[] = {{ {array_elements_str} }};\n")
        self.emit(
            f"unsigned int {self.get_array_size_variable_name(array_name)} = {array_element_count};\n\n"
        )

    def emit_normal_array_iterator(
        self, array_name: str, array_type: str, iterator_name: str
    ):
        self.emit(
            f"for(unsigned int i = 0; i < {self.get_array_size_variable_name(array_name)}; i++) {{\n"
        )
        self.emit(
            f"    {array_type} {iterator_name} = {self.get_array_element(array_name, 'i')};\n"
        )

    def emit_array_contains_check(
        self, var_to_check_against: str, var_to_check: str, search_variable: str
    ):
        self.emit(
            f"bool {search_variable} = false;\n"
            f"for (unsigned int i = 0; i < {self.get_array_size_variable_name(var_to_check_against)}; i++) {{\n"
            f"  if ({var_to_check_against}[i] == {var_to_check}) {{\n"
            f"      {search_variable} = true;\n"
            f"      break;\n"
            f"  }}\n"
            f" }}\n"
        )
