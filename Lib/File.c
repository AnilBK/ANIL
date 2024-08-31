// clang-format off
///*///
struct File{FILE* file_ptr};

namespace File
c_function __init__(p_file_name : str)
  this->file_ptr = fopen(p_file_name, "w");
  if(this->file_ptr == NULL) {
    printf("Failed to open file %s.\n", p_file_name);
    exit(0);
  }
endc_function  

c_function writeline(p_content : str)
  // Write a line to the file with terminating newline.
  fprintf(this->file_ptr, "%s\n", p_content);
endc_function  

c_function __del__()
  fclose(this->file_ptr);
endc_function
endnamespace
///*///

