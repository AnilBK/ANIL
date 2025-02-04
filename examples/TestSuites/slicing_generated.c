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
char Vector_char__getitem__(struct Vector_char *this, int index);
void Vector_char__setitem__(struct Vector_char *this, int index, char value);
void Vector_char__init__(struct Vector_char *this, int capacity);
void Vector_char__del__(struct Vector_char *this);
void Vector_charpush(struct Vector_char *this, char value);
void Vector_charallocate_more(struct Vector_char *this, int n);
void Vector_charpush_unchecked(struct Vector_char *this, char value);
char Vector_charpop(struct Vector_char *this);
void Vector_charremove_at(struct Vector_char *this, int index);
void Vector_charclear(struct Vector_char *this);
bool Vector_char__contains__(struct Vector_char *this, char value);
void Vector_charprint(struct Vector_char *this);

size_t Vector_charlen(struct Vector_char *this) { return this->size; }

char Vector_char__getitem__(struct Vector_char *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_char__setitem__(struct Vector_char *this, int index, char value) {
  if (index < 0) {
    index += this->size;
  }
  // FIXME: If previous value is a struct with destructor, then that destructor
  // should be called. This is fixed in the next overloaded function for String
  // class.
  this->arr[index] = value;
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

char Vector_charpop(struct Vector_char *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_charremove_at(struct Vector_char *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
  this->size--;
}

void Vector_charclear(struct Vector_char *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (char *)malloc(this->capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
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