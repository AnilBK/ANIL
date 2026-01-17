
#include <string.h>

///////////////////////////////////////////

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
void String_allocate_more(struct String *this, int n);
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
struct String Stringfrom(int number);
void Stringformat(struct String *this, char *format, int value);
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

void String_allocate_more(struct String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n + 1;

  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
  if (!new_arr) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  } else {
    this->arr = new_arr;
    this->capacity = new_capacity;
  }
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

struct String Stringfrom(int number) {
  char buf[32];
  int len = snprintf(buf, sizeof(buf), "%d", number);

  struct String text;
  if (len < 0) {
    String__init__from_charptr(&text, "", 0);
    return text;
  }

  if ((size_t)len >= sizeof(buf)) {
    // truncated output
    // either treat as error or clamp
    String__init__from_charptr(&text, buf, sizeof(buf) - 1);
    return text;
  }

  String__init__from_charptr(&text, buf, len);
  return text;
}

void Stringformat(struct String *this, char *format, int value) {
  if (this->is_constexpr) {
    fprintf(stderr, "Cannot modify constexpr string.\n");
    return;
  }

  int needed = snprintf(NULL, 0, format, value);
  if (needed < 0)
    return;

  if (needed + 1 > this->capacity) {
    size_t new_capacity =
        (needed + 1 > this->capacity * 2) ? needed + 1 : this->capacity * 2;
    char *new_arr = realloc(this->arr, new_capacity);
    if (!new_arr) {
      fprintf(stderr, "Memory reallocation failed in String::format.\n");
      exit(EXIT_FAILURE);
    }
    this->arr = new_arr;
    this->capacity = new_capacity;
  }

  snprintf(this->arr, this->capacity, format, value);
  this->length = needed;
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  if (this->is_constexpr) {
    // Probably not necessary, as constexpr strings are compiler generated, but
    // just in case.
    fprintf(stderr, "Error: Attempt to modify a constexpr String object.\n");
    exit(EXIT_FAILURE);
  }

  // Use fopen in binary read mode ("rb") to prevent newline translation.
  FILE *ptr = fopen(pfilename, "rb");
  if (ptr == NULL) {
    fprintf(stderr, "File \"%s\" couldn't be opened.\n", pfilename);
    Stringclear(this);
    return;
  }

  fseek(ptr, 0, SEEK_END);
  long fileSize = ftell(ptr);
  if (fileSize < 0) {
    fprintf(stderr, "Failed to get file size");
    fclose(ptr);
    Stringclear(this);
    return;
  }

  fseek(ptr, 0, SEEK_SET);

  int buffer_capacity = fileSize + 1;
  char *buffer = (char *)malloc(buffer_capacity);
  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed for file content.\n");
    fclose(ptr);
    Stringclear(this);
    return;
  }

  size_t bytesRead = fread(buffer, 1, fileSize, ptr);
  fclose(ptr);

  if (bytesRead != (size_t)fileSize) {
    fprintf(stderr, "Error reading file \"%s\". Expected %ld bytes, got %zu.\n",
            pfilename, fileSize, bytesRead);
    free(buffer);
    Stringclear(this);
    return;
  }

  buffer[fileSize] = '\0';

  if (this->arr != NULL) {
    free(this->arr);
  }

  // Take ownership of the buffer.
  // The buffer should not be freed after this point.
  this->arr = buffer;
  this->length = fileSize;
  this->capacity = buffer_capacity;
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

int main() {

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
  String__init__OVDstrint(&str, "Hello World", 11);
  // The following line is also valid, as the above syntax is shorthand for the
  // statement below. let str = String{"Hello World"};
  StringprintLn(&str);

  String__reassign__OVDstr(&str, "Reassign");
  StringprintLn(&str);

  struct String str2;
  String__init__OVDstrint(&str2, "Hi \n", 4);
  Stringprint(&str2);

  struct String str3 = Stringstrip(&str2);
  StringprintLn(&str3);

  size_t len = Stringlen(&str3);
  printf("Length of the string is : %llu. \n", len);

  size_t tmp_len_1 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_1; i++) {
    char val = String__getitem__(&str, i);
    printf("%c \n", val);
  }

  if (String__contains__(&str, "Wor")) {
    printf("Wor is in str. \n");
  }

  if (String__eq__(&str, "Hello World")) {
    puts("Str is Hello World.");
  }

  String_allocate_more(&str, 28 + 4);
  String__add__(&str, "New message appended at the ");
  String__add__(&str, "end.");
  StringprintLn(&str);

  String__reassign__OVDstr(&str, "");

  size_t len4 = Stringlen(&str);
  printf("Length of the string is : %llu. \n", len4);
  StringprintLn(&str);

  struct String str4;
  String__init__OVDstrint(&str4, "String constructed from another string. \n",
                          41);
  struct String str5;
  String__init__OVDstructString(&str5, str4);
  StringprintLn(&str5);

  struct String str6;
  String__init__OVDstrint(&str6, "String constructed from another string 2.\n",
                          42);
  struct String str7;
  String__init__OVDstructString(&str7, str6);
  StringprintLn(&str7);

  struct String substr_str = Stringsubstr(&str4, 0, 6);
  StringprintLn(&substr_str);

  printf("Split Test: \n");
  struct String split_str;
  String__init__OVDstrint(&split_str, "Splitting.with.dots.", 20);
  StringprintLn(&split_str);
  struct Vector_String dot_split = Stringsplit(&split_str, '.');
  Vector_Stringprint(&dot_split);

  struct String split_str2;
  String__init__OVDstrint(&split_str2, "Splitting with Spaces.", 22);
  StringprintLn(&split_str2);
  struct Vector_String space_split = Stringsplit(&split_str2, ' ');
  Vector_Stringprint(&space_split);

  int score = 69420;
  struct String tmp_string_0 = Stringfrom(score);
  struct String scoreText;
  String__init__OVDstrint(&scoreText, "Score: ", 7);
  String__add__(&scoreText, Stringc_str(&tmp_string_0));
  StringprintLn(&scoreText);

  String__del__(&scoreText);
  String__del__(&tmp_string_0);
  Vector_String__del__(&space_split);
  String__del__(&split_str2);
  Vector_String__del__(&dot_split);
  String__del__(&split_str);
  String__del__(&substr_str);
  String__del__(&str7);
  String__del__(&str6);
  String__del__(&str5);
  String__del__(&str4);
  String__del__(&str3);
  String__del__(&str2);
  String__del__(&str);

  return 0;
}