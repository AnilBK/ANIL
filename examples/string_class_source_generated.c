///*///

#include <ctype.h>
#include <string.h>

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
  String__init__(&str, "Hello World");
  StringprintLn(&str);

  String__reassign__(&str, "Reassign");
  StringprintLn(&str);

  String__reassign__(&str, "");
  StringprintLn(&str);

  struct String str2;
  String__init__(&str2, "Hi n");
  Stringprint(&str2);

  struct String str3 = Stringstrip(&str2);
  StringprintLn(&str3);

  size_t len = Stringlen(&str3);
  printf("Length of the string is : %llu. \n ", len);

  size_t tmp_len_0 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char val = String__getitem__(&str, i);
    printf("%c \n ", val);
  }

  if (String__contains__(&str, "Wor")) {
    printf("Wor is in str. \n ");
  }

  if (String__eq__(&str, "Hello World")) {
    puts("Str is Hello World.");
  }

  String__add__(&str, "New message appended at the end. ");
  StringprintLn(&str);

  // str.clear
  String__reassign__(&str, "");

  size_t len4 = Stringlen(&str);
  printf("Length of the string is : %llu. \n ", len4);
  StringprintLn(&str);

  String__del__(&str3);
  String__del__(&str2);
  String__del__(&str);

  ///*///
  // let str = String{"Hello World"};
  // let str2 = str.strip();
  /*

  Note : Two Templated Structs in a class causes error when writing
  __init__.
  TODO ??

  let string = Vector<char>{10};
  string.push 65
  string.push 66
  string.push 67
  string.print
  */
  return 0;
}