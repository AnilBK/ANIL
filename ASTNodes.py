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

    def codegen(self) -> str:
        # Generate the C code: "int x = 10;"
        code = f"{self.var_type} {self.var_name}"
        if self.initializer:
            code += f" = {self.initializer.codegen()}"
        code += ";"
        return code


class VariableNode(ExpressionNode):
    def __init__(self, var_name, var_type=None):
        self.var_name = var_name
        self.var_type = var_type

    def codegen(self) -> str:
        return self.var_name


class LiteralNode(ExpressionNode):
    def __init__(self, value, value_type):
        self.value = value
        self.value_type = value_type

    def codegen(self) -> str:
        if self.value_type == "char":
            return f"'{self.value}'"
        if self.value_type == "str":
            return f'"{self.value}"'
        if self.value_type == "bool":
            if isinstance(self.value, str):
                l = self.value.lower()
                if l in ("true", "false"):
                    return l
            elif isinstance(self.value, bool):
                return "true" if self.value else "false"
            return self.value
        return (
            str(self.value).lower() if isinstance(self.value, bool) else str(self.value)
        )


class AssignmentNode(StatementNode):
    def __init__(self, var_name, expression: ExpressionNode):
        self.var_name = var_name
        self.expression = expression

    def codegen(self) -> str:
        return f"{self.var_name} = {self.expression.codegen()};"


class CommentNode(StatementNode):
    def __init__(self, comment):
        self.comment = comment

    def codegen(self) -> str:
        return f"// {self.comment}\n"


class ReturnNode(StatementNode):
    """
    def __init__(self, expression: ExpressionNode):
        self.expression = expression
    Currently we dont have support for proper expressions.
    So we will allow string as expression for now.
    """

    def __init__(self, expression):
        self.expression = expression

    def codegen(self) -> str:
        if isinstance(self.expression, str):
            return f"return {self.expression};"
        elif isinstance(self.expression, ExpressionNode):
            return f"return {self.expression.codegen()};"
        else:
            raise ValueError("Unsupported return expression type.")
