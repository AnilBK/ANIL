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

function __call_destructor_if_available__()
  // If arr has a destructor, then emit 'arr.__del__()'.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  ~arr
endfunction

c_function __free__ptr()
  free(this->arr);
endc_function

function __del__()
  this.__call_destructor_if_available__()
  this.__free__ptr()
endfunction
endnamespace

///*///
