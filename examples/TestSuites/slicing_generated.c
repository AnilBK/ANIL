///*///

#include <string.h>

///////////////////////////////////////////

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Vector_char {
  char *arr;
  int size;
  int capacity;
};

size_t Vector_charlen(struct Vector_char *this);
void Vector_char__init__(struct Vector_char *this, int capacity);
void Vector_char_call_destructor_for_element(struct Vector_char *this,
                                             int index);
void Vector_char_call_destructor_for_all_elements(struct Vector_char *this);
void Vector_char_reset(struct Vector_char *this);
void Vector_char__del__(struct Vector_char *this);
char Vector_char__getitem__(struct Vector_char *this, int index);
void Vector_char_set_ith_item(struct Vector_char *this, int index, char value);
void Vector_charpush_unchecked(struct Vector_char *this, char value);
void Vector_charvalidate_index(struct Vector_char *this, int index);
void Vector_char_set(struct Vector_char *this, int index, char value);
void Vector_char__setitem__(struct Vector_char *this, int index, char value);
void Vector_charallocate_more(struct Vector_char *this, int n);
void Vector_char_grow_if_required(struct Vector_char *this);
void Vector_char_push(struct Vector_char *this, char value);
void Vector_charpush(struct Vector_char *this, char value);
char Vector_charpop(struct Vector_char *this);
void Vector_char_shift_left_from(struct Vector_char *this, int index);
void Vector_char_dec_size(struct Vector_char *this);
void Vector_charremove_at(struct Vector_char *this, int index);
void Vector_char_clear(struct Vector_char *this);
void Vector_charclear(struct Vector_char *this);
bool Vector_char__contains__(struct Vector_char *this, char value);
void Vector_charprint(struct Vector_char *this);

size_t Vector_charlen(struct Vector_char *this) { return this->size; }

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

void Vector_char_call_destructor_for_element(struct Vector_char *this,
                                             int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_char_call_destructor_for_all_elements(struct Vector_char *this) {
  for (size_t i = 0; i < Vector_charlen(this); i++) {
    Vector_char_call_destructor_for_element(this, i);
  }
}

void Vector_char_reset(struct Vector_char *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_char__del__(struct Vector_char *this) {
  Vector_char_call_destructor_for_all_elements(this);
  Vector_char_reset(this);
}

char Vector_char__getitem__(struct Vector_char *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_char_set_ith_item(struct Vector_char *this, int index, char value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_charpush_unchecked(struct Vector_char *this, char value) {
  this->arr[this->size++] = value;
}

void Vector_charvalidate_index(struct Vector_char *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_char_set(struct Vector_char *this, int index, char value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_charvalidate_index(this, index);
  Vector_char_call_destructor_for_element(this, index);
  Vector_char_set_ith_item(this, index, value);
}

void Vector_char__setitem__(struct Vector_char *this, int index, char value) {
  Vector_char_set(this, index, value);
}

void Vector_charallocate_more(struct Vector_char *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_char_grow_if_required(struct Vector_char *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_charallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_charallocate_more(this, 1);
    }
  }
}

void Vector_char_push(struct Vector_char *this, char value) {
  Vector_char_grow_if_required(this);
  Vector_charpush_unchecked(this, value);
}

void Vector_charpush(struct Vector_char *this, char value) {
  Vector_char_push(this, value);
}

char Vector_charpop(struct Vector_char *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_char_shift_left_from(struct Vector_char *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_char_dec_size(struct Vector_char *this) { this->size--; }

void Vector_charremove_at(struct Vector_char *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_charvalidate_index(this, index);
  Vector_char_call_destructor_for_element(this, index);
  Vector_char_shift_left_from(this, index);
  Vector_char_dec_size(this);
  // this.size = this.size - 1 : FIXME: Not supported yet.
}

void Vector_char_clear(struct Vector_char *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (char *)malloc(this->capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_charclear(struct Vector_char *this) {
  Vector_char_call_destructor_for_all_elements(this);
  Vector_char_clear(this);
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

int main() {

  ///*/// main()

  struct Vector_char a;
  Vector_char__init__(&a, 10);
  Vector_charpush(&a, '1');
  Vector_charpush(&a, '2');
  Vector_charpush(&a, '3');
  Vector_charpush(&a, '4');
  Vector_charpush(&a, '5');

  printf("Printing in normal order(step 1): 1,2,3,4,5 \n");
  size_t tmp_len_0 = Vector_charlen(&a);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char a1 = Vector_char__getitem__(&a, i);
    printf("%c ", a1);
  }

  printf("\nPrinting in normal order(step 1) in between (2,..): 3,4,5 \n");
  size_t tmp_len_1 = Vector_charlen(&a);
  for (size_t i = 2; i < tmp_len_1; i++) {
    char a1 = Vector_char__getitem__(&a, i);
    printf("%c ", a1);
  }

  printf("\nPrinting in normal order(step 1) in between (2,4): 3,4 \n");
  int tmp_len_2 = 4;
  for (size_t i = 2; i < tmp_len_2; i++) {
    char a1 = Vector_char__getitem__(&a, i);
    printf("%c ", a1);
  }

  printf("\nPrinting in normal order(step 2): 1,3,5 \n");
  size_t tmp_len_3 = Vector_charlen(&a);
  for (size_t i = 0; i < tmp_len_3; i += 2) {
    char a1 = Vector_char__getitem__(&a, i);
    printf("%c ", a1);
  }

  printf("\nPrinting in reverse order: 5,4,3,2,1 \n");
  size_t tmp_len_4 = Vector_charlen(&a);
  tmp_len_4 -= 1;
  for (size_t i = tmp_len_4; i != (size_t)-1; i += -1) {
    char a1 = Vector_char__getitem__(&a, i);
    printf("%c ", a1);
  }

  Vector_char__del__(&a);
  ///*///

  return 0;
}