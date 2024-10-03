#include <stdlib.h>

///*///

struct ErrorHandler { char dummy};

namespace ErrorHandler
  c_function RAISE_ERROR(p_error_msg : str)
    fprintf(stderr, p_error_msg);
    exit(EXIT_FAILURE);
  endc_function
endnamespace

///*///
