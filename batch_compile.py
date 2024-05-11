file_names = [
    "examples\\vector_source.c",
    "examples\\unique_ptr_source.c",
    "examples\\string_class_source.c",
    "lexer_test_source.c",
    "examples\\initializer_list.c",
    "examples\\Reflection.c",
    "examples\\List.c",
    "examples\\constexpr_dict.c",
    "examples\\decorators_inside_fn_body.c",
    "examples\\enumerate_source.c",
    "examples\\parser_tests.c",
    "examples\\struct_source.c",
    "examples\\string_source.c",
    "examples\\fileexample.c",
]

import subprocess

for file_name in file_names:
    subprocess.run(["python", "preprocess_2.py", "--filename", file_name])
