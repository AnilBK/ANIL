// clang-format off
///*///

struct UniquePtr<T>{T *arr};

namespace UniquePtr

c_function __init__(capacity : int)
  this->arr = (@TEMPLATED_DATA_TYPE@ *)malloc(capacity * sizeof(@TEMPLATED_DATA_TYPE@));

  if (this->arr == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }
endc_function

c_function __del__()
  free(this->arr);
endc_function
endnamespace

///*///
