// clang-format off
///*///

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct String{char* arr, int length, int capacity};

impl String __init__ text : str
  size_t p_text_length = strlen(text);
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }

  strcpy(this->arr, text);

  this->length = p_text_length;
  this->capacity = p_text_length;
endfunc


impl String clear
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
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
  return this->length;
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

  if(new_length > this->capacity){
    size_t new_capacity;
    if(this->capacity == 0){
      new_capacity = new_length * 2;
    }else{
      new_capacity = this->capacity;
      while(new_capacity <= new_length){
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
endfunc

impl String __reassign__ pstring : str
  this->arr = (char *)realloc(this->arr, (strlen(pstring) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);

  this->length = strlen(this->arr);
  this->capacity = this->length;
endfunc

impl String set_to_file_contents pfilename : str
  // Read from the file & store the contents to this string.

  // TODO: Use CPL to generate this, because the function below is a mangled
  // function name.
  Stringclear(this);

  FILE* ptr;
 
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
endfunc
///*///

