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

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

struct Vector_float {
  float *arr;
  int size;
  int capacity;
};

struct Vector_char {
  char *arr;
  int size;
  int capacity;
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
size_t Vector_intlen(struct Vector_int *this);
void Vector_int__init__(struct Vector_int *this, int capacity);
void Vector_int_call_destructor_for_element(struct Vector_int *this, int index);
void Vector_int_call_destructor_for_all_elements(struct Vector_int *this);
void Vector_int_reset(struct Vector_int *this);
void Vector_int__del__(struct Vector_int *this);
int Vector_int__getitem__(struct Vector_int *this, int index);
void Vector_int_set_ith_item(struct Vector_int *this, int index, int value);
void Vector_intpush_unchecked(struct Vector_int *this, int value);
void Vector_intvalidate_index(struct Vector_int *this, int index);
void Vector_int_set(struct Vector_int *this, int index, int value);
void Vector_int__setitem__(struct Vector_int *this, int index, int value);
void Vector_intallocate_more(struct Vector_int *this, int n);
void Vector_int_grow_if_required(struct Vector_int *this);
void Vector_int_push(struct Vector_int *this, int value);
void Vector_intpush(struct Vector_int *this, int value);
int Vector_intpop(struct Vector_int *this);
void Vector_int_shift_left_from(struct Vector_int *this, int index);
void Vector_intremove_at(struct Vector_int *this, int index);
void Vector_int_clear(struct Vector_int *this);
void Vector_intclear(struct Vector_int *this);
bool Vector_int__contains__(struct Vector_int *this, int value);
void Vector_intprint(struct Vector_int *this);
size_t Vector_floatlen(struct Vector_float *this);
void Vector_float__init__(struct Vector_float *this, int capacity);
void Vector_float_call_destructor_for_element(struct Vector_float *this,
                                              int index);
void Vector_float_call_destructor_for_all_elements(struct Vector_float *this);
void Vector_float_reset(struct Vector_float *this);
void Vector_float__del__(struct Vector_float *this);
float Vector_float__getitem__(struct Vector_float *this, int index);
void Vector_float_set_ith_item(struct Vector_float *this, int index,
                               float value);
void Vector_floatpush_unchecked(struct Vector_float *this, float value);
void Vector_floatvalidate_index(struct Vector_float *this, int index);
void Vector_float_set(struct Vector_float *this, int index, float value);
void Vector_float__setitem__(struct Vector_float *this, int index, float value);
void Vector_floatallocate_more(struct Vector_float *this, int n);
void Vector_float_grow_if_required(struct Vector_float *this);
void Vector_float_push(struct Vector_float *this, float value);
void Vector_floatpush(struct Vector_float *this, float value);
float Vector_floatpop(struct Vector_float *this);
void Vector_float_shift_left_from(struct Vector_float *this, int index);
void Vector_floatremove_at(struct Vector_float *this, int index);
void Vector_float_clear(struct Vector_float *this);
void Vector_floatclear(struct Vector_float *this);
bool Vector_float__contains__(struct Vector_float *this, float value);
void Vector_floatprint(struct Vector_float *this);
size_t Vector_charlen(struct Vector_char *this);
void Vector_char__init__(struct Vector_char *this, int capacity);
void Vector_char_call_destructor_for_element(struct Vector_char *this,
                                             int index);
void Vector_char_call_destructor_for_all_elements(struct Vector_char *this);
void Vector_char_reset(struct Vector_char *this);
void Vector_char__del__(struct Vector_char *this);
char Vector_char__getitem__(struct Vector_char *this, int index);
void Vector_char_set_ith_item(struct Vector_char *this, int index, char value);
void Vector_charpush_unchecked(struct Vector_char *this, char value);
void Vector_charvalidate_index(struct Vector_char *this, int index);
void Vector_char_set(struct Vector_char *this, int index, char value);
void Vector_char__setitem__(struct Vector_char *this, int index, char value);
void Vector_charallocate_more(struct Vector_char *this, int n);
void Vector_char_grow_if_required(struct Vector_char *this);
void Vector_char_push(struct Vector_char *this, char value);
void Vector_charpush(struct Vector_char *this, char value);
char Vector_charpop(struct Vector_char *this);
void Vector_char_shift_left_from(struct Vector_char *this, int index);
void Vector_charremove_at(struct Vector_char *this, int index);
void Vector_char_clear(struct Vector_char *this);
void Vector_charclear(struct Vector_char *this);
bool Vector_char__contains__(struct Vector_char *this, char value);
void Vector_charprint(struct Vector_char *this);

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
  size_t tmp_len_2 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_2; i++) {
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

size_t Vector_intlen(struct Vector_int *this) { return this->size; }

void Vector_int__init__(struct Vector_int *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (int *)malloc(capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_int_call_destructor_for_element(struct Vector_int *this,
                                            int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_int_call_destructor_for_all_elements(struct Vector_int *this) {
  for (size_t i = 0; i < Vector_intlen(this); i++) {
    Vector_int_call_destructor_for_element(this, i);
  }
}

void Vector_int_reset(struct Vector_int *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_int__del__(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_reset(this);
}

int Vector_int__getitem__(struct Vector_int *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_int_set_ith_item(struct Vector_int *this, int index, int value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_intpush_unchecked(struct Vector_int *this, int value) {
  this->arr[this->size++] = value;
}

void Vector_intvalidate_index(struct Vector_int *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_int_set(struct Vector_int *this, int index, int value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_set_ith_item(this, index, value);
}

void Vector_int__setitem__(struct Vector_int *this, int index, int value) {
  Vector_int_set(this, index, value);
}

void Vector_intallocate_more(struct Vector_int *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  int *new_arr = (int *)realloc(this->arr, new_capacity * sizeof(int));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_int_grow_if_required(struct Vector_int *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_intallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_intallocate_more(this, 1);
    }
  }
}

void Vector_int_push(struct Vector_int *this, int value) {
  Vector_int_grow_if_required(this);
  Vector_intpush_unchecked(this, value);
}

void Vector_intpush(struct Vector_int *this, int value) {
  Vector_int_push(this, value);
}

int Vector_intpop(struct Vector_int *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_int_shift_left_from(struct Vector_int *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_intremove_at(struct Vector_int *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_int_clear(struct Vector_int *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (int *)malloc(this->capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_intclear(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_clear(this);
}

bool Vector_int__contains__(struct Vector_int *this, int value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_intprint(struct Vector_int *this) {
  printf("Vector<int> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%d", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

size_t Vector_floatlen(struct Vector_float *this) { return this->size; }

void Vector_float__init__(struct Vector_float *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (float *)malloc(capacity * sizeof(float));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_float_call_destructor_for_element(struct Vector_float *this,
                                              int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_float_call_destructor_for_all_elements(struct Vector_float *this) {
  for (size_t i = 0; i < Vector_floatlen(this); i++) {
    Vector_float_call_destructor_for_element(this, i);
  }
}

void Vector_float_reset(struct Vector_float *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_float__del__(struct Vector_float *this) {
  Vector_float_call_destructor_for_all_elements(this);
  Vector_float_reset(this);
}

float Vector_float__getitem__(struct Vector_float *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_float_set_ith_item(struct Vector_float *this, int index,
                               float value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_floatpush_unchecked(struct Vector_float *this, float value) {
  this->arr[this->size++] = value;
}

void Vector_floatvalidate_index(struct Vector_float *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_float_set(struct Vector_float *this, int index, float value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_floatvalidate_index(this, index);
  Vector_float_call_destructor_for_element(this, index);
  Vector_float_set_ith_item(this, index, value);
}

void Vector_float__setitem__(struct Vector_float *this, int index,
                             float value) {
  Vector_float_set(this, index, value);
}

void Vector_floatallocate_more(struct Vector_float *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  float *new_arr = (float *)realloc(this->arr, new_capacity * sizeof(float));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_float_grow_if_required(struct Vector_float *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_floatallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_floatallocate_more(this, 1);
    }
  }
}

void Vector_float_push(struct Vector_float *this, float value) {
  Vector_float_grow_if_required(this);
  Vector_floatpush_unchecked(this, value);
}

void Vector_floatpush(struct Vector_float *this, float value) {
  Vector_float_push(this, value);
}

float Vector_floatpop(struct Vector_float *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_float_shift_left_from(struct Vector_float *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_floatremove_at(struct Vector_float *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_floatvalidate_index(this, index);
  Vector_float_call_destructor_for_element(this, index);
  Vector_float_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_float_clear(struct Vector_float *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (float *)malloc(this->capacity * sizeof(float));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_floatclear(struct Vector_float *this) {
  Vector_float_call_destructor_for_all_elements(this);
  Vector_float_clear(this);
}

bool Vector_float__contains__(struct Vector_float *this, float value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_floatprint(struct Vector_float *this) {
  printf("Vector<float> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%f", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

size_t Vector_charlen(struct Vector_char *this) { return this->size; }

void Vector_char__init__(struct Vector_char *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_char_call_destructor_for_element(struct Vector_char *this,
                                             int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_char_call_destructor_for_all_elements(struct Vector_char *this) {
  for (size_t i = 0; i < Vector_charlen(this); i++) {
    Vector_char_call_destructor_for_element(this, i);
  }
}

void Vector_char_reset(struct Vector_char *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_char__del__(struct Vector_char *this) {
  Vector_char_call_destructor_for_all_elements(this);
  Vector_char_reset(this);
}

char Vector_char__getitem__(struct Vector_char *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_char_set_ith_item(struct Vector_char *this, int index, char value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_charpush_unchecked(struct Vector_char *this, char value) {
  this->arr[this->size++] = value;
}

void Vector_charvalidate_index(struct Vector_char *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_char_set(struct Vector_char *this, int index, char value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_charvalidate_index(this, index);
  Vector_char_call_destructor_for_element(this, index);
  Vector_char_set_ith_item(this, index, value);
}

void Vector_char__setitem__(struct Vector_char *this, int index, char value) {
  Vector_char_set(this, index, value);
}

void Vector_charallocate_more(struct Vector_char *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_char_grow_if_required(struct Vector_char *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_charallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_charallocate_more(this, 1);
    }
  }
}

void Vector_char_push(struct Vector_char *this, char value) {
  Vector_char_grow_if_required(this);
  Vector_charpush_unchecked(this, value);
}

void Vector_charpush(struct Vector_char *this, char value) {
  Vector_char_push(this, value);
}

char Vector_charpop(struct Vector_char *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_char_shift_left_from(struct Vector_char *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_charremove_at(struct Vector_char *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_charvalidate_index(this, index);
  Vector_char_call_destructor_for_element(this, index);
  Vector_char_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_char_clear(struct Vector_char *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (char *)malloc(this->capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_charclear(struct Vector_char *this) {
  Vector_char_call_destructor_for_all_elements(this);
  Vector_char_clear(this);
}

bool Vector_char__contains__(struct Vector_char *this, char value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_charprint(struct Vector_char *this) {
  printf("Vector<char> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\'%c\'", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

int main() {

  ///*/// main()

  struct Vector_int a;
  Vector_int__init__(&a, 10);
  Vector_intpush(&a, 10);
  Vector_intpush(&a, 20);
  Vector_intpush(&a, 30);
  Vector_intpush(&a, 40);
  Vector_intpush(&a, 50);
  Vector_intprint(&a);

  if (Vector_int__contains__(&a, 10)) {
    printf("10 is in the vector. \n");
  }

  printf("Removing index 2 item.\n");
  Vector_intremove_at(&a, 2);

  Vector_intprint(&a);
  printf("\n");

  struct Vector_float b;
  Vector_float__init__(&b, 10);
  Vector_floatpush(&b, 10);
  Vector_floatpush(&b, 40);
  Vector_floatpush(&b, 50);
  Vector_floatprint(&b);

  struct Vector_char string;
  Vector_char__init__(&string, 10);
  Vector_charpush(&string, 'A');
  Vector_charpush(&string, 'N');
  Vector_charpush(&string, 'I');
  Vector_charpush(&string, 'L');
  Vector_charprint(&string);

  struct String str;
  String__init__OVDstrint(&str, "Hello", 5);
  struct String str2;
  String__init__OVDstrint(&str2, "World", 5);
  struct String str3;
  String__init__OVDstrint(&str3, "Honey", 5);
  struct String str4;
  String__init__OVDstrint(&str4, "Bunny", 5);

  struct Vector_String test;
  Vector_String__init__(&test, 5);
  // Push Strings to Vector<String>
  Vector_Stringpush(&test, str);
  Vector_Stringpush(&test, str2);
  Vector_Stringpush(&test, str3);
  Vector_Stringpush(&test, str4);
  Vector_Stringprint(&test);

  size_t tmp_len_1 = Vector_Stringlen(&test);
  tmp_len_1 -= 1;
  for (size_t i = tmp_len_1; i != (size_t)-1; i += -1) {
    struct String tst = Vector_String__getitem__(&test, i);
    Stringprint(&tst);
  }

  printf("\nClearing Vector<String>:\n");
  Vector_Stringclear(&test);
  Vector_Stringprint(&test);

  printf("\nPushing String to Vector<String>:\n");
  Vector_Stringpush(&test, str);
  Vector_Stringpush(&test, str2);
  Vector_Stringprint(&test);

  printf("\nReplacing with the above vector[1] with str which is Hello:\n");
  Vector_String__setitem__(&test, 1, str);
  Vector_Stringprint(&test);

  Vector_String__del__(&test);
  String__del__(&str4);
  String__del__(&str3);
  String__del__(&str2);
  String__del__(&str);
  Vector_char__del__(&string);
  Vector_float__del__(&b);
  Vector_int__del__(&a);
  ///*///

  return 0;
}