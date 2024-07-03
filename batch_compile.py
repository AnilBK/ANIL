file_names = [
    "examples\\01_variables.c",
    "examples\\vector_source.c",
    "examples\\unique_ptr_source.c",
    "examples\\string_class_source.c",
    "examples\\initializer_list.c",
    "examples\\Reflection.c",
    "examples\\02_List.c",
    "examples\\constexpr_dict.c",
    "examples\\decorators_inside_fn_body.c",
    "examples\\enumerate_source.c",
    "examples\\parser_tests.c",
    "examples\\struct_source.c",
    "examples\\string_source.c",
    "examples\\fileexample.c",
    "examples\\vector_of_strings.c",
    "examples\\function_example.c",
    "Bootstrap\\lexer_test.c",
    "Bootstrap\\preprocess_test.c",
]

import subprocess

for file_name in file_names:
    subprocess.run(["python", "preprocess_2.py", "--filename", file_name])

# for file_name in file_names:
#     output_file = file_name.split('.')[0]  
#     subprocess.run(["gcc", "-Wall", "-Wextra", "-o", output_file, output_file + "_generated.c"])