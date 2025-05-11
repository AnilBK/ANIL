
#include <string.h>

///////////////////////////////////////////

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

size_t Vector_intlen(struct Vector_int *this);
void Vector_int__init__(struct Vector_int *this, int capacity);
void Vector_int_call_destructor_for_element(struct Vector_int *this, int index);
void Vector_int_call_destructor_for_all_elements(struct Vector_int *this);
void Vector_int_reset(struct Vector_int *this);
void Vector_int__del__(struct Vector_int *this);
int Vector_int__getitem__(struct Vector_int *this, int index);
void Vector_int_set_ith_item(struct Vector_int *this, int index, int value);
void Vector_intpush_unchecked(struct Vector_int *this, int value);
void Vector_intvalidate_index(struct Vector_int *this, int index);
void Vector_int_set(struct Vector_int *this, int index, int value);
void Vector_int__setitem__(struct Vector_int *this, int index, int value);
void Vector_intallocate_more(struct Vector_int *this, int n);
void Vector_int_grow_if_required(struct Vector_int *this);
void Vector_int_push(struct Vector_int *this, int value);
void Vector_intpush(struct Vector_int *this, int value);
int Vector_intpop(struct Vector_int *this);
void Vector_int_shift_left_from(struct Vector_int *this, int index);
void Vector_intremove_at(struct Vector_int *this, int index);
void Vector_int_clear(struct Vector_int *this);
void Vector_intclear(struct Vector_int *this);
bool Vector_int__contains__(struct Vector_int *this, int value);
void Vector_intprint(struct Vector_int *this);

size_t Vector_intlen(struct Vector_int *this) { return this->size; }

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

void Vector_int_call_destructor_for_element(struct Vector_int *this,
                                            int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_int_call_destructor_for_all_elements(struct Vector_int *this) {
  for (size_t i = 0; i < Vector_intlen(this); i++) {
    Vector_int_call_destructor_for_element(this, i);
  }
}

void Vector_int_reset(struct Vector_int *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_int__del__(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_reset(this);
}

int Vector_int__getitem__(struct Vector_int *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_int_set_ith_item(struct Vector_int *this, int index, int value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_intpush_unchecked(struct Vector_int *this, int value) {
  this->arr[this->size++] = value;
}

void Vector_intvalidate_index(struct Vector_int *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_int_set(struct Vector_int *this, int index, int value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_set_ith_item(this, index, value);
}

void Vector_int__setitem__(struct Vector_int *this, int index, int value) {
  Vector_int_set(this, index, value);
}

void Vector_intallocate_more(struct Vector_int *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  int *new_arr = (int *)realloc(this->arr, new_capacity * sizeof(int));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_int_grow_if_required(struct Vector_int *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_intallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_intallocate_more(this, 1);
    }
  }
}

void Vector_int_push(struct Vector_int *this, int value) {
  Vector_int_grow_if_required(this);
  Vector_intpush_unchecked(this, value);
}

void Vector_intpush(struct Vector_int *this, int value) {
  Vector_int_push(this, value);
}

int Vector_intpop(struct Vector_int *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_int_shift_left_from(struct Vector_int *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_intremove_at(struct Vector_int *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_int_clear(struct Vector_int *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (int *)malloc(this->capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_intclear(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_clear(this);
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

int main() {

  struct Vector_int values;
  Vector_int__init__(&values, 8);
  Vector_intpush(&values, 1);
  Vector_intpush(&values, 2);
  Vector_intpush(&values, 3);
  Vector_intpush(&values, 4);
  Vector_intpush(&values, 5);
  Vector_intpush(&values, 6);
  Vector_intpush(&values, 7);
  Vector_intpush(&values, 8);

  Vector_intallocate_more(&values, 8);
  Vector_intpush_unchecked(&values, 10);
  Vector_intpush_unchecked(&values, 20);
  Vector_intpush_unchecked(&values, 30);
  Vector_intpush_unchecked(&values, 40);
  Vector_intpush_unchecked(&values, 50);
  Vector_intpush_unchecked(&values, 60);
  Vector_intpush_unchecked(&values, 70);
  Vector_intpush_unchecked(&values, 80);

  Vector_int__del__(&values);

  return 0;
}