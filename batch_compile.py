file_names = [
    "examples\\00_Hello_World.c",
    "examples\\00_Hello_World.anil",
    "examples\\01_variables.c",
    "examples\\02_List.c",
    "examples\\03_Dictionary.c",
    "examples\\04_Classes.c",
    "examples\\04_Classes.anil",
    "examples\\04_b_Classes.c",
    "examples\\05_Strings.c",
    "examples\\06_Vector.c",
    "examples\\07_Set.c",
    "examples\\08_Optional.c",
    "examples\\08_Optional.anil",
    "examples\\09_Functions.c",
    "examples\\10_FileIO.c",
    "examples\\10_FileIO.anil",
    "examples\\11_Threads_And_Channels.anil",
    "examples\\Annotations.c",
    "examples\\Compile_Time_Constants.c",
    "examples\\Macro_With_Variadic_Arguments.c",
    "examples\\Reflection.c",
    "examples\\Enumerate.c",
    "examples\\decorators_inside_fn_body.c",
    "examples\\Unique_ptr_example.c",
    "examples\\FunctionPointer.c",
    "examples\\UI\\TodoAppBasic.c",
    "examples\\UI\\TodoAppJSX.c",
    "examples\\UI\\TodoAppWebServer.c",
    "examples\\UI\\IDE\\IDE.anil",
    "examples\\Variables_GUI_Input_Win.c",
    "examples\\WebServer.c",
    "examples\\TestSuites\\Expression_parse_tests.c",
    "examples\\TestSuites\\Return_value_tests.c",
    "examples\\TestSuites\\slicing.c",
    "examples\\raylib\\raylib_example.c",
    "examples\\raylib\\snake.c",
    "Bootstrap\\lexer_test.c",
    "Bootstrap\\Parser.c",
    "Bootstrap\\preprocess_test.c",
    "Bootstrap\\preprocess_test.anil",
]

import subprocess

for file_name in file_names:
    subprocess.run(["python", "preprocess_2.py", "--filename", file_name])

# for file_name in file_names:
#     output_file = file_name.split('.')[0]  
#     subprocess.run(["gcc", "-Wall", "-Wextra", "-o", output_file, output_file + "_generated.c"])