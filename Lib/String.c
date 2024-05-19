// clang-format off
///*///

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct String{char* arr, int length, int capacity};

impl String __init__from_charptr text : str p_text_length : int
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
endfunc

impl String __init__ text : str
  size_t p_text_length = strlen(text);
  String__init__from_charptr(this, text, p_text_length);
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
  String__init__from_charptr(&text, begin, new_length);
  return text;
endfunc

impl String split -> Vector<String>:
  // TODO: Because of this function, before import String, we require import Vector.
  char delimeter = '.';

  struct Vector__String result;
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

  Vector_Stringprint(&result);

  return result;
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

