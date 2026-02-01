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

    def emit_boolean_reassignment(self, variable_name: str, value: bool):
        bool_str = "true" if value else "false"
        self.emit_reassignment(variable_name, bool_str)

    def emit_array_declaration(self, array_type: str, array_name: str, elements: list):
        array_element_count = len(elements)
        array_elements_str = ",".join(elements)

        self.emit(f"{array_type} {array_name}[] = {{ {array_elements_str} }};\n")
        self.emit(f"unsigned int {array_name}_array_size = {array_element_count};\n\n")

    def emit_function_call(self, function_name: str, arguments: list = []):
        args_str = ", ".join(arguments)
        self.emit(f"{function_name}({args_str});\n")

    def emit_return_statement(self, return_value: str = ""):
        if return_value:
            self.emit(f"return {return_value};\n")
        else:
            self.emit("return;\n")
