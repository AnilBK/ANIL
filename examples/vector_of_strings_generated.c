///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

///*///

#include <string.h>

///////////////////////////////////////////

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
};

void String__init__(struct String *this, char *text) {
  size_t p_text_length = strlen(text);
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, text);

  this->length = p_text_length;
  this->capacity = p_text_length;
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) { free(this->arr); }

struct String Stringstrip(struct String *this) {
  //  char *str = "  Hello ";
  char *str = this->arr;

  char *begin = str;
  // Remove leading whitespaces
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces
  char *end = str + strlen(str) - 1;
  while (end > begin && isspace(*end)) {
    end--;
  }

  int new_length = end - begin + 1;

  struct String text;
  text.arr = (char *)malloc((new_length + 1) * sizeof(char));
  text.length = new_length;
  text.capacity = new_length;

  if (text.arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  // Copy the substring to the new variable
  strncpy(text.arr, begin, new_length);

  // Null-terminate the new string
  text.arr[new_length] = '\0';

  return text;
}

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

bool Stringis_of_length(struct String *this, int p_len) {
  return strlen(this->arr) == p_len;
}

char *Stringc_str(struct String *this) { return this->arr; }

void String__add__(struct String *this, char *pstring) {
  size_t new_length = strlen(this->arr) + strlen(pstring) + 1;

  if (new_length > this->capacity) {
    size_t new_capacity;
    if (this->capacity == 0) {
      new_capacity = new_length * 2;
    } else {
      new_capacity = this->capacity;
      while (new_capacity <= new_length) {
        new_capacity *= 2;
      }
    }
    this->arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
    this->capacity = new_capacity;
  }

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
  this->length = new_length;
}

void String__reassign__(struct String *this, char *pstring) {
  this->arr = (char *)realloc(this->arr, (strlen(pstring) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);

  this->length = strlen(this->arr);
  this->capacity = this->length;
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  // Read from the file & store the contents to this string.

  // TODO: Use CPL to generate this, because the function below is a mangled
  // function name.
  Stringclear(this);

  FILE *ptr;

  ptr = fopen(pfilename, "r");

  if (ptr == NULL) {
    printf("File can't be opened.\n");
    exit(0);
  }

  char myString[256];
  while (fgets(myString, 256, ptr)) {
    String__add__(this, myString);
  }

  fclose(ptr);
}

struct Vector__char {
  char *arr;
  int size;
  int capacity;
};

// template Vector<char> {
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
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_charallocate_more(struct Vector__char *this, int n) {
  this->capacity += n;
  this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_charpush_unchecked(struct Vector__char *this, char value) {
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

  printf("Dynamic Array (size = %zu, capacity = %zu) : [", this->size,
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

// template Vector<char> }

struct Vector__String {
  struct String *arr;
  int size;
  int capacity;
};

// template Vector<String> {
void Vector_String__init__(struct Vector__String *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct String *)malloc(capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_String__del__(struct Vector__String *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_Stringpush(struct Vector__String *this, struct String value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct String *)realloc(this->arr, this->capacity *
                                                        sizeof(struct String));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_Stringallocate_more(struct Vector__String *this, int n) {
  this->capacity += n;
  this->arr = (struct String *)realloc(this->arr,
                                       this->capacity * sizeof(struct String));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_Stringpush_unchecked(struct Vector__String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringprint(struct Vector__String *this) {
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
    const char *type = "struct String";
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

bool Vector_String__contains__(struct Vector__String *this,
                               struct String value) {
  // this returns bool.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t Vector_Stringlen(struct Vector__String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector__String *this, int index) {
  return *(this->arr + index);
}

// template Vector<String> }

int main() {

  ///*///

  struct Vector__char string;
  Vector_char__init__(&string, 10);
  Vector_charpush(&string, 'A');
  Vector_charpush(&string, 'N');
  Vector_charpush(&string, 'I');
  Vector_charpush(&string, 'L');
  Vector_charprint(&string);

  struct Vector__String test;
  Vector_String__init__(&test, 5);

  // let split = string.split(",");
  //  TODO : implement this

  Vector_String__del__(&test);
  Vector_char__del__(&string);
  ///*///
  ;

  return 0;
}