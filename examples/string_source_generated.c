///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
};

void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length) {
  // p_text_length : Length of the string without the null terminator.
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strncpy(this->arr, text, p_text_length);
  this->arr[p_text_length] = '\0';

  this->length = p_text_length;
  this->capacity = p_text_length + 1;
}

void String__init__(struct String *this, char *text) {
  size_t p_text_length = strlen(text);
  String__init__from_charptr(this, text, p_text_length);
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

bool Stringstartswith(struct String *this, char *prefix) {
  return strncmp(this->arr, prefix, strlen(prefix)) == 0;
}

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
  String__init__from_charptr(&text, begin, new_length);
  return text;
}

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

// template Vector<String> {
void Vector_String__init__(struct Vector_String *this, int capacity) {
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

void Vector_String__del__(struct Vector_String *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  struct String str;
  String__init__(&str, value.arr);

  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct String *)realloc(this->arr, this->capacity *
                                                        sizeof(struct String));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = str;
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  this->capacity += n;
  this->arr = (struct String *)realloc(this->arr,
                                       this->capacity * sizeof(struct String));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringprint(struct Vector_String *this) {
  for (size_t i = 0; i < this->size; ++i) {
    printf("%s\n", this->arr[i].arr);
  }
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  for (size_t i = 0; i < this->size; ++i) {
    if (strcmp(this->arr[i].arr, value.arr) == 0) {
      return true;
    }
  }
  return false;
}

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  return *(this->arr + index);
}

// template Vector<String> }

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // TODO: Because of this function, before import String, we require import
  // Vector.
  struct Vector_String result;
  Vector_String__init__(&result, 2);

  int delim_location = -1;

  int len = this->length;
  for (int i = 0; i < len; i++) {
    if (this->arr[i] == delimeter) {
      int length = i - (delim_location + 1);

      struct String text;
      String__init__from_charptr(&text, &this->arr[delim_location + 1], length);
      Vector_Stringpush(&result, text);

      delim_location = i;
    }
  }

  // Add remaining string.
  if (delim_location + 1 < len) {
    char *remaining = &this->arr[delim_location + 1];

    struct String text;
    String__init__(&text, remaining);
    Vector_Stringpush(&result, text);
  }

  return result;
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

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

int main() {

  ///*///

  struct String str;
  String__init__(&str, "Hello");
  StringprintLn(&str);

  size_t tmp_len_0 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char value = String__getitem__(&str, i);
    printf("%c.\n", value);
  }

  if (String__eq__(&str, "Hello")) {
    printf("Str is hello.\n");
  }

  String__add__(&str, " World");
  StringprintLn(&str);

  String__add__(&str, " Its me");
  String__add__(&str, " Anil ");
  StringprintLn(&str);

  if (String__contains__(&str, "Anil")) {
    printf("Anil is in the given string.\n");
  }

  int arr[] = {1, 2, 3, 4, 5, 10};
  unsigned int arr_array_size = 6;

  bool arr__contains__10_0 = false;
  for (unsigned int i = 0; i < arr_array_size; i++) {
    if (arr[i] == 10) {
      arr__contains__10_0 = true;
      break;
    }
  }

  if (arr__contains__10_0) {
    printf("10 is in arr. \n");
  }

  for (unsigned int i = 0; i < arr_array_size; i++) {
    int value2 = arr[i];
    printf("%d \n", value2);
  }

  float arr2[] = {1, 2, 3, 4, 5, 10};
  unsigned int arr2_array_size = 6;

  for (unsigned int i = 0; i < arr2_array_size; i++) {
    float value3 = arr2[i];
    printf("%f \n", value3);
  }

  String__del__(&str);
  ///*///

  return 0;
}