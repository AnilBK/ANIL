///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

///*///

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

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

struct Set {
  struct Vector_String arr;
};

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstructString(struct String *this, struct String text);
void Stringclear(struct String *this);
void Stringprint(struct String *this);
void StringprintLn(struct String *this);
void String__del__(struct String *this);
bool Stringstartswith(struct String *this, char *prefix);
struct String Stringsubstr(struct String *this, int start, int length);
struct String Stringstrip(struct String *this);
struct Vector_String Stringsplit(struct String *this, char delimeter);
bool String__contains__(struct String *this, char *substring);
bool String__eq__(struct String *this, char *pstring);
void String__add__(struct String *this, char *pstring);
void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length);
void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring);
void String__reassign__OVDstr(struct String *this, char *pstring);
void Stringset_to_file_contents(struct String *this, char *pfilename);
struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename);

size_t Setlen(struct Set *this);
struct String Set__getitem__(struct Set *this, int index);
void Set__init__(struct Set *this, int count);
void Set__del__(struct Set *this);
bool Set__contains__(struct Set *this, struct String value);
void Setadd(struct Set *this, struct String value);
void Setprint(struct Set *this);

size_t Vector_Stringlen(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String__del__(struct Vector_String *this);
void Vector_Stringpush(struct Vector_String *this, struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_Stringremove_at(struct Vector_String *this, int index);
bool Vector_String__contains__(struct Vector_String *this, struct String value);
void Vector_Stringprint(struct Vector_String *this);

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

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // NOTE : Because of this function, before import String, we require import
  // Vector.
  struct Vector_String split_result;
  Vector_String__init__(&split_result, 2);

  int index = 0;
  int segment_start = 0;

  size_t tmp_len_0 = Stringlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char character = String__getitem__(this, i);

    if (character == delimeter) {

      if (segment_start < index) {
        struct String segment =
            Stringsubstr(this, segment_start, index - segment_start);
        Vector_Stringpush(&split_result, segment);
        String__del__(&segment);
      }
      segment_start = index + 1;
    }
    index = index + 1;
  }

  if (segment_start < Stringlen(this)) {
    struct String remaining_segment =
        Stringsubstr(this, segment_start, Stringlen(this) - segment_start);
    Vector_Stringpush(&split_result, remaining_segment);
    String__del__(&remaining_segment);
  }

  return split_result;
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

size_t Setlen(struct Set *this) { return Vector_Stringlen(&this->arr); }

struct String Set__getitem__(struct Set *this, int index) {
  return Vector_String__getitem__(&this->arr, index);
}

void Set__init__(struct Set *this, int count) {
  Vector_String__init__(&this->arr, count);
}

void Set__del__(struct Set *this) { Vector_String__del__(&this->arr); }

bool Set__contains__(struct Set *this, struct String value) {
  return Vector_String__contains__(&this->arr, value);
}

void Setadd(struct Set *this, struct String value) {

  if (!Set__contains__(this, value)) {
    Vector_Stringpush(&this->arr, value);
  }
}

void Setprint(struct Set *this) {
  printf("[");
  size_t tmp_len_1 = Setlen(this);
  for (size_t i = 0; i < tmp_len_1; i++) {
    struct String str = Set__getitem__(this, i);
    Stringprint(&str);
    printf(",");
  }
  printf("]\n");
}

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }
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

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
  this->size--;
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_3 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_3; i++) {
    struct String string = Vector_String__getitem__(this, i);

    if (Stringlen(&string) == Stringlen(&value)) {

      if (String__eq__(&string, Stringc_str(&value))) {
        return true;
      }
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

int main() {

  ///*/// main()

  struct String s1;
  String__init__OVDstr(&s1, "apple");
  struct String s2;
  String__init__OVDstr(&s2, "banana");
  struct String s3;
  String__init__OVDstr(&s3, "cherry");

  struct Set s;
  Set__init__(&s, 10);
  Setadd(&s, s1);
  Setadd(&s, s1);
  Setadd(&s, s2);
  Setadd(&s, s2);
  Setadd(&s, s3);
  Setprint(&s);

  size_t tmp_len_2 = Setlen(&s);
  for (size_t i = 0; i < tmp_len_2; i++) {
    struct String str = Set__getitem__(&s, i);
    StringprintLn(&str);
  }

  Set__del__(&s);
  String__del__(&s3);
  String__del__(&s2);
  String__del__(&s1);
  ///*///

  return 0;
}