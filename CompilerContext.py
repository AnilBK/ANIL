class CompilerContext:
    def __init__(self):
        self.generated_lines = []

        # This is like Git but for generated code lines.
        # While speculative parsing of functions, we may generate some intermediate codes.
        # If speculative parsing fails, we need to undo these changes.
        # These changes are marked using these trackers.
        self.generated_lines_checkpoints = []

        self.counters = {}

    def get_next_temp_id(self, prefix: str) -> int:
        if prefix not in self.counters:
            self.counters[prefix] = 0

        current_id = self.counters[prefix]
        self.counters[prefix] += 1
        return current_id

    def emit(self, code: str):
        self.generated_lines.append(code)

    def get_generated_lines_count(self) -> int:
        return len(self.generated_lines)

    def extend_generated_lines(self, new_lines):
        self.generated_lines.extend(new_lines)

    def get_generated_lines_slice(self, start: int, end: int):
        return self.generated_lines[start:end]

    def remove_generated_lines(self, start: int, end: int):
        del self.generated_lines[start:end]

    def get_generated_line(self, index: int):
        return self.generated_lines[index]

    def set_generated_line(self, index: int, value):
        self.generated_lines[index] = value

    def push_generated_lines_checkpoint(self):
        self.generated_lines_checkpoints.append(self.get_generated_lines_count())

    def commit_generated_lines_checkpoint(self):
        if self.generated_lines_checkpoints:
            del self.generated_lines_checkpoints[-1]
        else:
            print("Generated lines weren't tracking.")

    def revert_generated_lines_checkpoint(self):
        if not self.generated_lines_checkpoints:
            print("Generated lines were not tracking. Cant revert.")
            return

        checkpoint = self.generated_lines_checkpoints[-1]
        if self.get_generated_lines_count() != checkpoint:
            del self.generated_lines[checkpoint:]
        del self.generated_lines_checkpoints[-1]

    def write_final_code(self, p_output_file_name: str):
        with open(p_output_file_name, "w") as output_file:
            for line in self.generated_lines:
                output_file.writelines(line)
