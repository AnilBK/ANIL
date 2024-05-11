///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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

int main() {

  ///*///

  struct String str;
  String__init__(&str, "Hello World");
  StringprintLn(&str);

  String__reassign__(&str, "Reassign");
  StringprintLn(&str);

  struct String str2;
  String__init__(&str2, "Hi \n");
  Stringprint(&str2);

  struct String str3 = Stringstrip(&str2);
  StringprintLn(&str3);

  size_t len = Stringlen(&str3);
  printf("Length of the string is : %llu. \n", len);

  size_t tmp_len_0 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char val = String__getitem__(&str, i);
    printf("%c \n", val);
  }

  if (String__contains__(&str, "Wor")) {
    printf("Wor is in str. \n");
  }

  if (String__eq__(&str, "Hello World")) {
    puts("Str is Hello World.");
  }

  String__add__(&str, "New message appended at the end. ");
  StringprintLn(&str);

  String__reassign__(&str, "");

  size_t len4 = Stringlen(&str);
  printf("Length of the string is : %llu. \n", len4);
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