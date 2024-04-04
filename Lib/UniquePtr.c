// clang-format off
///*///

struct UniquePtr<T>{T *arr};

impl UniquePtr __init__ capacity
  this->arr = (@TEMPLATED_DATA_TYPE@ *)malloc(capacity * sizeof(@TEMPLATED_DATA_TYPE@));

  if (this->arr == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }
endfunc

impl UniquePtr __del__
  free(this->arr);
endfunc

///*///
