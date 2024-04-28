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
};

void String__init__(struct String *this, char *text) {
  this->arr = (char *)malloc((strlen(text) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, text);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, 1);
  this->arr[0] = '\0';
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

size_t Stringlen(struct String *this) { return strlen(this->arr); }

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

  this->arr = (char *)realloc(this->arr, new_length);
  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
}

void String__reassign__(struct String *this, char *pstring) {
  this->arr = (char *)realloc(this->arr, (strlen(pstring) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);
}

int main() {

  ///*///

  struct String str;
  String__init__(&str, "Hello");
  StringprintLn(&str);

  size_t tmp_len_0 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char value = String__getitem__(&str, i);
    printf("%c.\n ", value);
  }

  if (String__eq__(&str, "Hello")) {
    printf("Str is hello.\n ");
  }

  String__add__(&str, " World");
  StringprintLn(&str);

  String__add__(&str, " Its me Anil");
  StringprintLn(&str);

  if (String__contains__(&str, "Anil")) {
    printf("Anil is in the given string.\n ");
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
    printf("10 is in arr. \n ");
  }

  for (unsigned int i = 0; i < arr_array_size; i++) {
    int value2 = arr[i];
    printf("%d \n ", value2);
  }

  float arr2[] = {1, 2, 3, 4, 5, 10};
  unsigned int arr2_array_size = 6;

  for (unsigned int i = 0; i < arr2_array_size; i++) {
    float value3 = arr2[i];
    printf("%f \n ", value3);
  }

  String__del__(&str);
  ///*///

  return 0;
}