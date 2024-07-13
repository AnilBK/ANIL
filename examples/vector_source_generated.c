///*///

#include <string.h>

///////////////////////////////////////////

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

// template Vector<int> {
size_t Vector_intlen(struct Vector_int *this) { return this->size; }

int Vector_int__getitem__(struct Vector_int *this, int index) {
  return *(this->arr + index);
}

void Vector_int__init__(struct Vector_int *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (int *)malloc(capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_int__del__(struct Vector_int *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_intpush(struct Vector_int *this, int value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_intallocate_more(struct Vector_int *this, int n) {
  this->capacity += n;
  this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_intpush_unchecked(struct Vector_int *this, int value) {
  this->arr[this->size++] = value;
}

bool Vector_int__contains__(struct Vector_int *this, int value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_intprint(struct Vector_int *this) {
  printf("Vector<int> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%d", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

// template Vector<int> }

struct Vector_float {
  float *arr;
  int size;
  int capacity;
};

// template Vector<float> {
size_t Vector_floatlen(struct Vector_float *this) { return this->size; }

float Vector_float__getitem__(struct Vector_float *this, int index) {
  return *(this->arr + index);
}

void Vector_float__init__(struct Vector_float *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (float *)malloc(capacity * sizeof(float));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_float__del__(struct Vector_float *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_floatpush(struct Vector_float *this, float value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (float *)realloc(this->arr, this->capacity * sizeof(float));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_floatallocate_more(struct Vector_float *this, int n) {
  this->capacity += n;
  this->arr = (float *)realloc(this->arr, this->capacity * sizeof(float));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_floatpush_unchecked(struct Vector_float *this, float value) {
  this->arr[this->size++] = value;
}

bool Vector_float__contains__(struct Vector_float *this, float value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_floatprint(struct Vector_float *this) {
  printf("Vector<float> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%f", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

// template Vector<float> }

struct Vector_char {
  char *arr;
  int size;
  int capacity;
};

// template Vector<char> {
size_t Vector_charlen(struct Vector_char *this) { return this->size; }

char Vector_char__getitem__(struct Vector_char *this, int index) {
  return *(this->arr + index);
}

void Vector_char__init__(struct Vector_char *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_char__del__(struct Vector_char *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_charpush(struct Vector_char *this, char value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_charallocate_more(struct Vector_char *this, int n) {
  this->capacity += n;
  this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_charpush_unchecked(struct Vector_char *this, char value) {
  this->arr[this->size++] = value;
}

bool Vector_char__contains__(struct Vector_char *this, char value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_charprint(struct Vector_char *this) {
  printf("Vector<char> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\'%c\'", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

// template Vector<char> }

int main() {

  ///*///

  struct Vector_int a;
  Vector_int__init__(&a, 10);
  Vector_intpush(&a, 10);
  Vector_intpush(&a, 20);
  Vector_intpush(&a, 30);
  Vector_intpush(&a, 40);
  Vector_intpush(&a, 50);
  Vector_intprint(&a);

  if (Vector_int__contains__(&a, 10)) {
    printf("10 is in the vector. \n");
  }

  struct Vector_float b;
  Vector_float__init__(&b, 10);
  Vector_floatpush(&b, 10);
  Vector_floatpush(&b, 40);
  Vector_floatpush(&b, 50);
  Vector_floatprint(&b);

  struct Vector_char string;
  Vector_char__init__(&string, 10);
  Vector_charpush(&string, 'A');
  Vector_charpush(&string, 'N');
  Vector_charpush(&string, 'I');
  Vector_charpush(&string, 'L');
  Vector_charprint(&string);

  Vector_char__del__(&string);
  Vector_float__del__(&b);
  Vector_int__del__(&a);
  ///*///

  return 0;
}