///*///

#include <string.h>

///////////////////////////////////////////

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Vector__int {
  int *arr;
  int size;
  int capacity;
};

// template Vector<int> {
void Vector_int__init__(struct Vector__int *this, int capacity) {
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

void Vector_int__del__(struct Vector__int *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_intpush(struct Vector__int *this, int value) {
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

void Vector_intallocate_more(struct Vector__int *this, int n) {
  this->capacity += n;
  this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_intpush_unchecked(struct Vector__int *this, int value) {
  this->arr[this->size++] = value;
}

void Vector_intprint(struct Vector__int *this) {
  // maybe print instanced vec name.
  // instanced name should be passed as fn parameter ig.
  // or add instance_name member silently to the struct itself.
  // printf("@INSTANCE_NAME@\n");
  /*

  void Vector_print(struct Vector* this, const char* p_instance_name){
  printf("%s\n", p_instance_name);
  }
  */

  printf("Dynamic Array (size = %zu, capacity = %zu) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    const char *type = "int";
    if (strcmp(type, "float") == 0) {
      printf("%f", this->arr[i]);
    } else if (strcmp(type, "int") == 0) {
      printf("%d", this->arr[i]);
    } else if (strcmp(type, "char") == 0) {
      printf("%c", this->arr[i]);
    }
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

bool Vector_int__contains__(struct Vector__int *this, int value) {
  // this returns bool.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t Vector_intlen(struct Vector__int *this) { return this->size; }

int Vector_int__getitem__(struct Vector__int *this, int index) {
  return *(this->arr + index);
}

// template Vector<int> }

int main() {

  ///*///

  struct Vector__int values;
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
  ///*///

  return 0;
}