///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

#include <string.h>

typedef struct {
  char *key_str;
} Key;

typedef struct {
  Key key;
  int value;
} KeyValuePair;

// We don't have arrays inside our custom struct datatype,so vvv.
typedef struct {
  KeyValuePair pair[45];
} KeyValuePairs;

///*///

///*///

#include <stdlib.h>
#include <string.h>

typedef union {
  int int_data;
  char *str_data;
} CPLObject_Data;

typedef enum { INT, STRING } CPLObject_DataType;

typedef struct CPLObject CPLObject;
typedef CPLObject *CPLObjectptr;

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
};

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
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
  size_t p_text_length = strlen(text);
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

void String__reassign__(struct String *this, char *pstring) {
  int new_length = strlen(pstring);
  this->arr = (char *)realloc(this->arr, (new_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);

  this->length = new_length;
  this->capacity = new_length + 1;
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

struct Dictionary {
  int added_values;
  KeyValuePairs pairs;
};

void Dictionary__init__(struct Dictionary *this) { this->added_values = 0; }

void Dictionaryadd_key_value(struct Dictionary *this, char *p_key_str,
                             int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

void Dictionaryprint(struct Dictionary *this) {
  printf("{\n");
  for (int i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    printf("\"%s\" : %d,\n", key, value);
  }
  printf("}\n");
}

bool Dictionary__contains__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return true;
    }
  }
  return false;
}

int Dictionaryget_value_from_key(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

int Dictionary__getitem__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

void Dictionary__setitem__(struct Dictionary *this, char *p_key_str,
                           int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

struct CPLObject {
  CPLObject_Data data;
  CPLObject_DataType data_type;
  CPLObject *next;
};

void CPLObject__init__OVDint(struct CPLObject *this, int p_value) {
  this->data.int_data = p_value;
  this->data_type = INT;
  this->next = NULL;
}

void CPLObject__init__OVDstr(struct CPLObject *this, char *p_value) {
  this->data.str_data = strdup(p_value);
  this->data_type = STRING;
  this->next = NULL;
}

bool CPLObjectis_int(struct CPLObject *this) { return this->data_type == INT; }

int CPLObjectget_int(struct CPLObject *this) { return this->data.int_data; }

bool CPLObjectis_str(struct CPLObject *this) {
  return this->data_type == STRING;
}

char *CPLObjectget_str(struct CPLObject *this) { return this->data.str_data; }

void CPLObject_clear_str(struct CPLObject *this) { free(this->data.str_data); }

void CPLObject__del__(struct CPLObject *this) {

  if (CPLObjectis_str(this)) {
    CPLObject_clear_str(this);
  }
}

struct CPLObject CPLObject_duplicate(struct CPLObject *this) {
  // Perform a deep copy.
  CPLObject copy = *this;
  if (this->data_type == STRING) {
    copy.data.str_data = strdup(this->data.str_data);
  }
  return copy;
}

bool CPLObject__eq__OVDint(struct CPLObject *this, int p_value) {

  if (CPLObjectis_int(this)) {
    return CPLObjectget_int(this) == p_value;
  } else {
    return false;
  }
}

bool CPLObject__eq__OVDstr(struct CPLObject *this, char *p_value) {

  if (CPLObjectis_str(this)) {
    return CPLObjectget_str(this) == p_value;
  } else {
    return false;
  }
}

struct List {
  CPLObject *head;
  CPLObject *tail;
  int size;
};

int Listlen(struct List *this) { return this->size; }

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

void List__del__(struct List *this) {
  CPLObject *current = this->head;
  while (current != NULL) {
    CPLObject *temp = current;
    current = current->next;

    CPLObject__del__(temp);
    free(temp);
  }
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

struct CPLObject List__getitem__(struct List *this, int index) {
  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  CPLObject *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  // Duplicate contents of node and return it.
  // If we return a reference, then the calling function will call destructor,
  // which will free the str_data causing free() errors later.
  return CPLObject_duplicate(current);
}

struct CPLObject Listpop(struct List *this, int index) {
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    exit(EXIT_FAILURE);
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  CPLObject *current = this->head;
  CPLObject *previous = NULL;

  for (int i = 0; i < index; i++) {
    previous = current;
    current = current->next;
  }

  if (previous == NULL) {
    // Popping the head.
    this->head = current->next;
    if (this->head == NULL) {
      // The list is now empty.
      this->tail = NULL;
    }
  } else {
    previous->next = current->next;
    if (current->next == NULL) {
      // Popping the tail.
      this->tail = previous;
    }
  }

  this->size--;

  CPLObject popped_node = *current;
  // Don't free current->data.str_data even though current data_type is String.
  // After copying the *pointer above, popped_node now owns
  // current->data.str_data. This avoids duplicating current->data.str_data into
  // popped_node.
  free(current);
  return popped_node;
}

void Listprint(struct List *this) {
  CPLObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\"", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d", data);
      //@hook_end
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

typedef void (*custom_integer_printer)(int);
void Listprint_hooked_custom_integer_printer(
    struct List *this, custom_integer_printer p_custom_integer_printer) {
  CPLObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\"", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //
      p_custom_integer_printer(data);
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

void List_insert_end(struct List *this, CPLObjectptr new_node) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
}

CPLObjectptr Listcreate_int_node(struct List *this, int p_value) {
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type int for value %d.",
           p_value);
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDint(new_node, p_value);
  return new_node;
}

CPLObjectptr Listcreate_string_node(struct List *this, char *p_value) {
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDstr(new_node, p_value);
  return new_node;
}

void Listappend_int(struct List *this, int p_value) {
  CPLObjectptr int_node = Listcreate_int_node(this, p_value);
  List_insert_end(this, int_node);
}

void Listappend_str(struct List *this, char *p_value) {
  CPLObjectptr str_node = Listcreate_string_node(this, p_value);
  List_insert_end(this, str_node);
}

void ListappendOVDint(struct List *this, int p_value) {
  Listappend_int(this, p_value);
}

void ListappendOVDstr(struct List *this, char *p_value) {
  Listappend_str(this, p_value);
}

struct Lexer {
  int dummy;
};

struct List Lexerget_tokens(struct Lexer *this) {

  struct Dictionary CHARACTER_TOKENS;
  Dictionary__init__(&CHARACTER_TOKENS);
  Dictionary__setitem__(&CHARACTER_TOKENS, "=", 1);

  Dictionary__setitem__(&CHARACTER_TOKENS, "[", 2);

  Dictionary__setitem__(&CHARACTER_TOKENS, "]", 3);

  Dictionary__setitem__(&CHARACTER_TOKENS, ";", 4);

  Dictionary__setitem__(&CHARACTER_TOKENS, ",", 5);

  Dictionary__setitem__(&CHARACTER_TOKENS, "%", 6);

  Dictionary__setitem__(&CHARACTER_TOKENS, "{", 7);

  Dictionary__setitem__(&CHARACTER_TOKENS, "}", 8);

  Dictionary__setitem__(&CHARACTER_TOKENS, "<", 15);

  Dictionary__setitem__(&CHARACTER_TOKENS, ">", 16);

  Dictionary__setitem__(&CHARACTER_TOKENS, "\"", 18);

  Dictionary__setitem__(&CHARACTER_TOKENS, "+", 19);

  Dictionary__setitem__(&CHARACTER_TOKENS, "(", 21);

  Dictionary__setitem__(&CHARACTER_TOKENS, ")", 22);

  Dictionary__setitem__(&CHARACTER_TOKENS, ":", 23);

  Dictionary__setitem__(&CHARACTER_TOKENS, ".", 24);

  Dictionary__setitem__(&CHARACTER_TOKENS, "*", 25);

  Dictionary__setitem__(&CHARACTER_TOKENS, "-", 26);

  Dictionary__setitem__(&CHARACTER_TOKENS, "#", 35);

  Dictionary__setitem__(&CHARACTER_TOKENS, "@", 37);

  Dictionary__setitem__(&CHARACTER_TOKENS, "!", 41);

  struct Dictionary KEYWORD_TOKENS;
  Dictionary__init__(&KEYWORD_TOKENS);
  Dictionary__setitem__(&KEYWORD_TOKENS, "let", 0);

  Dictionary__setitem__(&KEYWORD_TOKENS, "struct", 9);

  Dictionary__setitem__(&KEYWORD_TOKENS, "match", 10);

  Dictionary__setitem__(&KEYWORD_TOKENS, "for", 11);

  Dictionary__setitem__(&KEYWORD_TOKENS, "if", 12);

  Dictionary__setitem__(&KEYWORD_TOKENS, "in", 13);

  Dictionary__setitem__(&KEYWORD_TOKENS, "Option", 14);

  Dictionary__setitem__(&KEYWORD_TOKENS, "enumerate", 17);

  Dictionary__setitem__(&KEYWORD_TOKENS, "def", 27);

  Dictionary__setitem__(&KEYWORD_TOKENS, "c_function", 28);

  Dictionary__setitem__(&KEYWORD_TOKENS, "enddef", 29);

  Dictionary__setitem__(&KEYWORD_TOKENS, "endfunc", 30);

  Dictionary__setitem__(&KEYWORD_TOKENS, "else", 31);

  Dictionary__setitem__(&KEYWORD_TOKENS, "True", 32);

  Dictionary__setitem__(&KEYWORD_TOKENS, "False", 33);

  Dictionary__setitem__(&KEYWORD_TOKENS, "constexpr", 34);

  Dictionary__setitem__(&KEYWORD_TOKENS, "include", 36);

  Dictionary__setitem__(&KEYWORD_TOKENS, "apply_hook", 38);

  Dictionary__setitem__(&KEYWORD_TOKENS, "hook_begin", 39);

  Dictionary__setitem__(&KEYWORD_TOKENS, "hook_end", 40);

  struct String line_org;
  String__init__OVDstr(&line_org, "  let arr = [ 1, 2, 3, 4 , 5 ]; } let");
  struct String line = Stringstrip(&line_org);
  size_t length = Stringlen(&line);

  struct String token;
  String__init__OVDstr(&token, "");

  struct List tokens;
  List__init__(&tokens);

  bool inside_string = false;
  bool escape_back_slash = false;

  StringprintLn(&line);

  size_t tmp_len_0 = Stringlen(&line);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char Char = String__getitem__(&line, i);
    // print("{Char}")

    if (escape_back_slash) {

      if (Char == '"') {
        char Char_promoted_0[2] = {Char, '\0'};
        String__add__(&token, Char_promoted_0);
        escape_back_slash = false;
      } else if (Char == '\\') {
        String__add__(&token, "\\\\");
        escape_back_slash = false;
      } else {
        char Char_promoted_1[2] = {Char, '\0'};
        String__add__(&token, Char_promoted_1);
        escape_back_slash = false;
      }
    } else if (Char == '"') {

      if (inside_string) {
        // End of string.
        inside_string = false;

        // Single character tokens like = are tokenized by add_token(),
        // so we use the following method.
        // "=" the inner equals to shouldn't be tokenized.
        Listappend_str(&tokens, Stringc_str(&token));

        String__reassign__(&token, "");
      } else {
        // Start of string.
        inside_string = true;
      }
      ListappendOVDint(&tokens, 18);
    } else if (inside_string) {

      if (Char == '\\') {
        escape_back_slash = true;
        continue;
      }

      char Char_promoted_2[2] = {Char, '\0'};
      String__add__(&token, Char_promoted_2);
    } else if (Char == ' ') {

      if (String__eq__(&token, "")) {
        continue;
      }

      if (Dictionary__contains__(&KEYWORD_TOKENS, Stringc_str(&token))) {
        int ktk = Dictionary__getitem__(&KEYWORD_TOKENS, Stringc_str(&token));
        ListappendOVDint(&tokens, ktk);
      } else if (Dictionary__contains__(&CHARACTER_TOKENS,
                                        Stringc_str(&token))) {
        int ctk = Dictionary__getitem__(&CHARACTER_TOKENS, Stringc_str(&token));
        ListappendOVDint(&tokens, ctk);
      } else {
        Listappend_str(&tokens, Stringc_str(&token));
      }
      String__reassign__(&token, "");
    } else {
      char Char_promoted_3[2] = {Char, '\0'};

      if (Dictionary__contains__(&CHARACTER_TOKENS, Char_promoted_3)) {

        if (!String__eq__(&token, "")) {

          if (Dictionary__contains__(&KEYWORD_TOKENS, Stringc_str(&token))) {
            int ktk =
                Dictionary__getitem__(&KEYWORD_TOKENS, Stringc_str(&token));
            ListappendOVDint(&tokens, ktk);
          } else if (Dictionary__contains__(&CHARACTER_TOKENS,
                                            Stringc_str(&token))) {
            int ctk =
                Dictionary__getitem__(&CHARACTER_TOKENS, Stringc_str(&token));
            ListappendOVDint(&tokens, ctk);
          } else {
            Listappend_str(&tokens, Stringc_str(&token));
          }
        }
        char Char_promoted_4[2] = {Char, '\0'};
        int int_tk = Dictionary__getitem__(&CHARACTER_TOKENS, Char_promoted_4);
        ListappendOVDint(&tokens, int_tk);
        String__reassign__(&token, "");
        continue;
      }

      if (Dictionary__contains__(&CHARACTER_TOKENS, Stringc_str(&token))) {
        int int_tk =
            Dictionary__getitem__(&CHARACTER_TOKENS, Stringc_str(&token));
        ListappendOVDint(&tokens, int_tk);
        String__reassign__(&token, "");
        continue;
      }

      char Char_promoted_5[2] = {Char, '\0'};
      String__add__(&token, Char_promoted_5);
    }
  }

  // Process the last token.

  if (!String__eq__(&token, "")) {

    if (Dictionary__contains__(&KEYWORD_TOKENS, Stringc_str(&token))) {
      int ktk = Dictionary__getitem__(&KEYWORD_TOKENS, Stringc_str(&token));
      ListappendOVDint(&tokens, ktk);
    } else if (Dictionary__contains__(&CHARACTER_TOKENS, Stringc_str(&token))) {
      int ctk = Dictionary__getitem__(&CHARACTER_TOKENS, Stringc_str(&token));
      ListappendOVDint(&tokens, ctk);
    } else {
      Listappend_str(&tokens, Stringc_str(&token));
    }
  }

  String__del__(&token);
  String__del__(&line);
  String__del__(&line_org);
  return tokens;
}

void CustomPrint(int data) {
  // printf("This is a custom print wrapper. [%d]\n", data);

  // The macro is in str, val pair because it is in this format in TOKEN_MAP
  // below.

#define token_case(str, val)                                                   \
  case val:                                                                    \
    msg = str;                                                                 \
    break

  char *msg;

  switch (data) {
    token_case("LET", 0);
    token_case("EQUALS", 1);
    token_case("LEFT_SQUARE_BRACKET", 2);
    token_case("RIGHT_SQUARE_BRACKET", 3);
    token_case("SEMICOLON", 4);
    token_case("COMMA", 5);
    token_case("PERCENT", 6);
    token_case("LEFT_CURLY", 7);
    token_case("RIGHT_CURLY", 8);
    token_case("STRUCT", 9);
    token_case("MATCH", 10);
    token_case("FOR", 11);
    token_case("IF", 12);
    token_case("IN", 13);
    token_case("OPTION", 14);
    token_case("SMALLER_THAN", 15);
    token_case("GREATER_THAN", 16);
    token_case("ENUMERATE", 17);
    token_case("QUOTE", 18);
    token_case("PLUS", 19);
    token_case("LEFT_ROUND_BRACKET", 21);
    token_case("RIGHT_ROUND_BRACKET", 22);
    token_case("COLON", 23);
    token_case("DOT", 24);
    token_case("ASTERISK", 25);
    token_case("MINUS", 26);
    token_case("DEF", 27);
    token_case("c_function", 28);
    token_case("ENDDEF", 29);
    token_case("ENDFN", 30);
    token_case("ELSE", 31);
    token_case("TRUE", 32);
    token_case("FALSE", 33);
    token_case("CONSTEXPR", 34);
    token_case("HASH", 35);
    token_case("INCLUDE", 36);
    token_case("AT", 37);
    token_case("APPLY_HOOK", 38);
    token_case("HOOK_BEGIN", 39);
    token_case("HOOK_END", 40);
    token_case("EXCLAMATION", 41);
  default: {
    msg = "UNDEFINED: ";
    break;
  }
  }

  printf("Token : %s", msg);

#undef token_case
}

///*///

///*///

int main() {

  ///*///

  struct Lexer lexer;
  struct List tokens = Lexerget_tokens(&lexer);

  Listprint_hooked_custom_integer_printer(&tokens, CustomPrint);

  List__del__(&tokens);
  ///*///

  return 0;
}