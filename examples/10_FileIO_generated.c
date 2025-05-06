///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

///*///

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
  bool is_constexpr;
};

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

struct File {
  FILE *file_ptr;
};

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void Stringinit__STATIC__(struct String *this, char *text, int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length);
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

void File__init__(struct File *this, char *p_file_name);
void Filewriteline(struct File *this, char *p_content);
void File__del__(struct File *this);
size_t Vector_Stringlen(struct Vector_String *this);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index);
void Vector_String_call_destructor_for_all_elements(struct Vector_String *this);
void Vector_String_reset(struct Vector_String *this);
void Vector_String__del__(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
void Vector_Stringvalidate_index(struct Vector_String *this, int index);
struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s);
void Vector_String_set(struct Vector_String *this, int index,
                       struct String value);
void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_String_grow_if_required(struct Vector_String *this);
void Vector_String_push(struct Vector_String *this, struct String value);
void Vector_Stringpush(struct Vector_String *this, struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_String_shift_left_from(struct Vector_String *this, int index);
void Vector_Stringremove_at(struct Vector_String *this, int index);
void Vector_String_clear(struct Vector_String *this);
void Vector_Stringclear(struct Vector_String *this);
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
  this->is_constexpr = false;
}

void Stringinit__STATIC__(struct String *this, char *text, int p_text_length) {
  // WARNING: Only the compiler should write a call to this function.
  // The compiler uses this initialization function to create a temporary String
  // object when a string literal is passed to a function that expects a String
  // object.
  this->arr = text;
  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = true;
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length) {
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

void String__del__(struct String *this) {
  if (!this->is_constexpr) {
    free(this->arr);
  }
}

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

  // TODO: Implement this function in ANIL itself, because the function below is
  // a mangled function name.
  Stringclear(this);

  FILE *ptr = fopen(pfilename, "r");
  if (ptr == NULL) {
    printf("File \"%s\" couldn't be opened.\n", pfilename);
    return;
  }

  char myString[256];
  bool has_data = false;

  while (fgets(myString, sizeof(myString), ptr)) {
    String__add__(this, myString);
    has_data = true;
  }

  fclose(ptr);

  if (!has_data) {
    // Double-clear just in case
    Stringclear(this);
  }
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

void File__init__(struct File *this, char *p_file_name) {
  this->file_ptr = fopen(p_file_name, "w");
  if (this->file_ptr == NULL) {
    printf("Failed to open file %s.\n", p_file_name);
    exit(0);
  }
}

void Filewriteline(struct File *this, char *p_content) {
  // Write a line to the file with terminating newline.
  fprintf(this->file_ptr, "%s\n", p_content);
}

void File__del__(struct File *this) { fclose(this->file_ptr); }

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

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

void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  String__del__(&this->arr[index]);
}

void Vector_String_call_destructor_for_all_elements(
    struct Vector_String *this) {
  for (size_t i = 0; i < Vector_Stringlen(this); i++) {
    Vector_String_call_destructor_for_element(this, i);
  }
}

void Vector_String_reset(struct Vector_String *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_String__del__(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_reset(this);
}

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

void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringvalidate_index(struct Vector_String *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s) {
  struct String string_copy;
  String__init__OVDstructString(&string_copy, s);
  return string_copy;
}

void Vector_String_set(struct Vector_String *this, int index,
                       struct String value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_set_ith_item(this, index, value);
}

void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_set(this, index, Vector_String_copy_string(this, value));
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct String *new_arr =
      (struct String *)realloc(this->arr, new_capacity * sizeof(struct String));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_String_grow_if_required(struct Vector_String *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_Stringallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_Stringallocate_more(this, 1);
    }
  }
}

void Vector_String_push(struct Vector_String *this, struct String value) {
  Vector_String_grow_if_required(this);
  Vector_Stringpush_unchecked(this, value);
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_push(this, Vector_String_copy_string(this, value));
}

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_String_shift_left_from(struct Vector_String *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_String_clear(struct Vector_String *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct String *)malloc(this->capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_Stringclear(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_clear(this);
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_1 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_1; i++) {
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

  printf("Writing to file hello.txt:\n");
  struct File outputFile;
  File__init__(&outputFile, "hello.txt");
  Filewriteline(&outputFile, "Hello World :)");

  printf("Reading from a file(10_FileIO.c) to a string:\n");
  struct String input_str;
  String__init__OVDstrint(&input_str, "", 0);
  Stringset_to_file_contents(&input_str, "10_FileIO.c");
  StringprintLn(&input_str);

  String__del__(&input_str);
  File__del__(&outputFile);
  ///*///

  return 0;
}