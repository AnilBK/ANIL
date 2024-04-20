// clang-format off
///*///

#include <string.h>
#include <ctype.h>

struct String{char* arr};

impl String __init__ text : str
  this->arr = (char *)malloc((strlen(text) + 1) * sizeof(char));

  if (this->arr == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }

  strcpy(this->arr, text);
endfunc


impl String clear
  this->arr = (char *)realloc(this->arr, 1);
  this->arr[0] = '\0';
endfunc

impl String print
  printf("%s", this->arr);
endfunc

impl String printLn
  printf("%s\n", this->arr);
endfunc


impl String __del__
  free(this->arr);
endfunc

impl String strip -> String:
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
endfunc

impl String len -> size_t:
  return strlen(this->arr);
endfunc

impl String __getitem__ index : int -> char
  return *(this->arr + index);
endfunc

impl String __contains__ substring : str -> bool
  return strstr(this->arr, substring) != NULL;
endfunc

impl String __eq__ pstring : str -> bool
  return strcmp(this->arr, pstring) == 0;
endfunc

impl String is_of_length p_len : int -> bool
  return strlen(this->arr) == p_len;
endfunc

impl String c_str -> str:
  return this->arr;
endfunc

impl String __add__ pstring : str
  size_t new_length = strlen(this->arr) + strlen(pstring) + 1;

  this->arr = (char *)realloc(this->arr, new_length);
  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
endfunc

impl String __reassign__ pstring : str
  this->arr = (char *)realloc(this->arr, (strlen(pstring) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);
endfunc

///*///

