from abc import ABC, abstractmethod


class ASTNode(ABC):
    @abstractmethod
    def codegen(self) -> str:
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


class AssignmentNode(StatementNode):
    def __init__(self, var_name, expression: ExpressionNode):
        self.var_name = var_name
        self.expression = expression

    def codegen(self):
        return f"{self.var_name} = {self.expression.codegen()};"
