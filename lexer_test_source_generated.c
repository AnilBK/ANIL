///*///

#include <ctype.h>
#include <string.h>

///*///

///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

///*///

#include <stdlib.h>

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Vector__char {
  char *arr;
  int size;
  int capacity;
};

// template <char> {
void Vector_char__init__(struct Vector__char *this, int capacity) {
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

void Vector_char__del__(struct Vector__char *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_charpush(struct Vector__char *this, char value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_allocate_n(struct Vector__char *this, int n) {
  this->capacity += n;
  this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed\n");
    exit(EXIT_FAILURE);
  }
}
void Vector_charpush2(struct Vector__char *this, char value) {
  this->arr[this->size++] = value;
}

void Vector_charprint(struct Vector__char *this) {
  // maybe print instanced vec name.
  // instanced name should be passed as fn parameter ig.
  // or add instance_name member silently to the struct itself.
  // printf("@INSTANCE_NAME@\n");
  /*

  void Vector_print(struct Vector* this, const char* p_instance_name){
  printf("%s\n", p_instance_name);
  }
  */

  printf("Dynamic Array (size=%zu, capacity=%zu): [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    const char *type = "char";
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

bool Vector_char__contains__(struct Vector__char *this, char value) {
  // this returns bool.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t Vector_charlen(struct Vector__char *this) { return this->size; }

char Vector_char__getitem__(struct Vector__char *this, int index) {
  return *(this->arr + index);
}

// template <char> }

int main() {

  ///*///

  struct Vector__char character_tokens;
  Vector_char__init__(&character_tokens, 20);

  Vector_charpush(&character_tokens, '=');
  Vector_charpush(&character_tokens, '[');
  Vector_charpush(&character_tokens, ']');
  Vector_charpush(&character_tokens, ';');
  Vector_charpush(&character_tokens, ',');
  Vector_charpush(&character_tokens, '{');
  Vector_charpush(&character_tokens, '}');
  Vector_charpush(&character_tokens, '<');
  Vector_charpush(&character_tokens, '>');
  Vector_charpush(&character_tokens, '+');
  Vector_charpush(&character_tokens, '(');
  Vector_charpush(&character_tokens, ')');
  Vector_charpush(&character_tokens, ':');
  Vector_charpush(&character_tokens, '.');
  Vector_charpush(&character_tokens, '*');
  Vector_charpush(&character_tokens, '-');

  // character_tokens.push "="
  /*
  We need this sort of preprocessor language.
  def push(tokens...){
    character_tokens.allocate_n tokens.len
    for token in tokens{
      character_tokens.push token
    }
  }
  */

  // Optimized code..
  Vector_allocate_n(&character_tokens, 16);
  Vector_charpush2(&character_tokens, '=');
  Vector_charpush2(&character_tokens, '[');
  Vector_charpush2(&character_tokens, ']');
  Vector_charpush2(&character_tokens, ';');
  Vector_charpush2(&character_tokens, ',');
  Vector_charpush2(&character_tokens, '{');
  Vector_charpush2(&character_tokens, '}');
  Vector_charpush2(&character_tokens, '<');
  Vector_charpush2(&character_tokens, '>');
  Vector_charpush2(&character_tokens, '+');
  Vector_charpush2(&character_tokens, '(');
  Vector_charpush2(&character_tokens, ')');
  Vector_charpush2(&character_tokens, ':');
  Vector_charpush2(&character_tokens, '.');
  Vector_charpush2(&character_tokens, '*');
  Vector_charpush2(&character_tokens, '-');

  Vector_char__del__(&character_tokens);
  ///*///

  return 0;
}