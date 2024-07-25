class ErrorHandler:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.current_line_code = ""
            cls._instance.source_file = ""
        return cls._instance

    def register_source_file(self, source_file: str) -> None:
        self.source_file = source_file

    def register_current_line_code(self, current_line_code: str) -> None:
        """
        Register the line which we are currently parsing.
        This line is printed along with the error message.
        """
        self.current_line_code = current_line_code

    def raise_error(self, error):
        line = self.current_line_code.strip()
        error_msg = (
            f"Error in compiling '{self.source_file}' "
            f'at line "{line}" :\n'
            f"{error}"
        )
        raise Exception(f"{error_msg}")
