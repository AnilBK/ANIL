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
#include <stdlib.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
};

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

size_t Stringlength_of_charptr(struct String *this, char *p_string) {
  // This should be some kind of static method.
  return strlen(p_string);
}

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

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstructString(struct String *this, struct String text) {
  size_t p_text_length = Stringlen(&text);
  String__init__from_charptr(this, Stringc_str(&text), p_text_length);
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

struct String Stringsubstr(struct String *this, int start, int length) {
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;
}

struct String Stringstrip(struct String *this) {
  char *begin = this->arr;
  char *end = begin + Stringlen(this) - 1;

  // Remove leading whitespaces.
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces.
  while (end > begin && isspace(*end)) {
    end--;
  }

  // Length of the substring between 'begin' and 'end' inclusive.
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
size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

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
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }

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
  String__init__OVDstructString(&str, value);

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

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  for (size_t i = 0; i < this->size; ++i) {
    if (strcmp(this->arr[i].arr, value.arr) == 0) {
      return true;
    }
  }
  return false;
}

void Vector_Stringprint(struct Vector_String *this) {
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"", this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
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

      struct String text = Stringsubstr(this, delim_location + 1, length);
      Vector_Stringpush(&result, text);
      String__del__(&text);

      delim_location = i;
    }
  }

  // Add remaining string.
  if (delim_location + 1 < len) {
    char *remaining = &this->arr[delim_location + 1];

    struct String text;
    String__init__OVDstr(&text, remaining);
    Vector_Stringpush(&result, text);
    String__del__(&text);
  }

  return result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = this->length + strlen(pstring) + 1;

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

void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length) {
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
}

void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring) {
  char *src = Stringc_str(&pstring);
  size_t p_text_length = Stringlen(&pstring);
  Stringreassign_internal(this, src, p_text_length);
}

void String__reassign__OVDstr(struct String *this, char *pstring) {
  size_t p_text_length = Stringlength_of_charptr(this, pstring);
  Stringreassign_internal(this, pstring, p_text_length);
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

  char c_str1[18] = "C Style Strings\n";
  // Such strings are very limited in functionality and only looping as below is
  // supported.
  char *c_str1_iterator_0 = c_str1;
  while (*c_str1_iterator_0 != '\0') {
    char c = *c_str1_iterator_0;
    c_str1_iterator_0++;
    printf("%c", c);
  }

  struct String str;
  String__init__OVDstr(&str, "Hello World");
  StringprintLn(&str);

  String__reassign__OVDstr(&str, "Reassign");
  StringprintLn(&str);

  struct String str2;
  String__init__OVDstr(&str2, "Hi \n");
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

  String__add__(&str, "New message appended at the ");
  String__add__(&str, "end.");
  StringprintLn(&str);

  String__reassign__OVDstr(&str, "");

  size_t len4 = Stringlen(&str);
  printf("Length of the string is : %llu. \n", len4);
  StringprintLn(&str);

  struct String str4;
  String__init__OVDstr(&str4, "String constructed from another string. \n");
  struct String str5;
  String__init__OVDstructString(&str5, str4);
  StringprintLn(&str5);

  struct String substr_str = Stringsubstr(&str4, 0, 6);
  StringprintLn(&substr_str);

  printf("Split Test: \n");
  struct String split_str;
  String__init__OVDstr(&split_str, "Splitting.with.dots.");
  StringprintLn(&split_str);
  struct Vector_String dot_split = Stringsplit(&split_str, '.');
  Vector_Stringprint(&dot_split);

  struct String split_str2;
  String__init__OVDstr(&split_str2, "Splitting with Spaces.");
  StringprintLn(&split_str2);
  struct Vector_String space_split = Stringsplit(&split_str2, ' ');
  Vector_Stringprint(&space_split);

  Vector_String__del__(&space_split);
  String__del__(&split_str2);
  Vector_String__del__(&dot_split);
  String__del__(&split_str);
  String__del__(&substr_str);
  String__del__(&str5);
  String__del__(&str4);
  String__del__(&str3);
  String__del__(&str2);
  String__del__(&str);

  ///*///
  return 0;
}