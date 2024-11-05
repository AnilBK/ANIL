# C Preprocessor Language(CPL)
## CPL is a statically typed programming language, inspired by Python, that can be embedded within C source files.

![ss1](https://github.com/user-attachments/assets/7ed43e7e-39fd-4ff0-a8e4-a60f43983355)

*Fig: CPL functions written in C and CPL respectively.*

## CPL generates appropriate C code, which is then compiled using a C compiler.

![ss2](https://github.com/user-attachments/assets/38fb545a-cad5-44d6-8892-7e4d07afab95)

*Fig: The generated C code from the second \_\_contains\_\_ method in the first screenshot.*

## Documentation:
You can find example CPL code in the [examples/](examples/) directory.
The examples can be viewed serially to learn various features of the language.


## Requirements:
- C compiler.
- Python.
- clang-format(Recommended).

## Usage
1. Open [preprocess_2.py](preprocess_2.py). 
2. Uncomment one of the `source_file = "FILE_NAME*.c"` line.
3. Run 
```bash 
python preprocess_2.py
```
Alternatively, you can specify the filename directly by running:
```bash
python preprocess_2.py --filename examples\FILE_NAME*.c
```

4. This will generate a FILE_NAME*_generated.c file. Compile the generated c file using a C compiler and execute the generated executable.

## Features:
- Classes: [Example 1](examples/04_Classes.c) [Example 2](examples/04_b_Classes.c)
- Templates & Function Overloading: [Example](Lib/Vector.c)
- Macros: [Example](examples/initializer_list.c)
- Compile-Time Reflection: [Example 1](examples/Reflection.c) 
- Annotations: [Example 1](examples/Annotations.c) [WebSever Example](examples/WebServer.c)
- Mixing C with CPL: See [Lib/](Lib/)

The examples in [examples/](examples/) folder shows all different features of CPL.

## Project Structure
## Directories

- **[Bootstrap](Bootstrap/)**: Implementation of the compiler in CPL itself. Files in this directory uses all (advanced) features available in the language.

- **[examples](examples/)**: Demonstrations of various language features and usage of the standard library.

- **[Lib](Lib/)**: Standard Library.

## Individual Files

- **[preprocess_2.py](preprocess_2.py)**: The actual compiler and code generator.

- **[batch_compile.py](batch_compile.py)**: Compiles all the CPL files one by one. Used to verify if the changes made to the compiler doesn't break existing functionality.

---

> [!NOTE]  
> This is my first ever compiler project.  
> I created this compiler for the following reasons:
> 1. To learn about compiler development.  
> 2. To build a self-compiling compiler.  
> 3. To implement features I find interesting and cool.
