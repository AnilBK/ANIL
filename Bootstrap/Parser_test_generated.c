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
    return strcmp(p_value, CPLObjectget_str(this)) == 0;
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

struct Parser {
  struct List tokens;
};

void Parser__init__(struct Parser *this) {
  printf("Parser Constructor.\n");

  List__init__(&this->tokens);
  ListappendOVDstr(&this->tokens, "print");
  ListappendOVDint(&this->tokens, 0);
  ListappendOVDstr(&this->tokens, "arr");
  ListappendOVDint(&this->tokens, 1);
  ListappendOVDint(&this->tokens, 2);
  ListappendOVDstr(&this->tokens, "1");
  ListappendOVDint(&this->tokens, 5);
  ListappendOVDstr(&this->tokens, "2");
  ListappendOVDint(&this->tokens, 5);
  ListappendOVDstr(&this->tokens, "3");
  ListappendOVDint(&this->tokens, 5);
  ListappendOVDstr(&this->tokens, "4");
  ListappendOVDint(&this->tokens, 5);
  ListappendOVDstr(&this->tokens, "5");
  ListappendOVDint(&this->tokens, 3);
  ListappendOVDint(&this->tokens, 4);
  ListappendOVDint(&this->tokens, 8);
  ListappendOVDint(&this->tokens, 0);
}

void Parser__del__(struct Parser *this) {
  // TODO : This should be performed automatically.
  List__del__(&this->tokens);
}

bool Parserhas_tokens_remaining(struct Parser *this) {
  return Listlen(&this->tokens) > 0;
}

struct CPLObject Parsercurrent_token(struct Parser *this) {
  return List__getitem__(&this->tokens, 0);
}

void Parsernext_token(struct Parser *this) {
  struct CPLObject node = Listpop(&this->tokens, 0);
  CPLObject__del__(&node);
}

struct CPLObject Parserget_token(struct Parser *this) {
  return Listpop(&this->tokens, 0);
}

bool Parsercheck_tokenOVDint(struct Parser *this, int token) {
  struct CPLObject node = Parsercurrent_token(this);
  bool return_name = CPLObject__eq__OVDint(&node, token);
  CPLObject__del__(&node);
  return return_name;
}

bool Parsercheck_tokenOVDstr(struct Parser *this, char *token) {
  struct CPLObject node = Parsercurrent_token(this);
  bool return_name = CPLObject__eq__OVDstr(&node, token);
  CPLObject__del__(&node);
  return return_name;
}

bool Parsermatch_token(struct Parser *this, int token) {

  if (Parsercheck_tokenOVDint(this, token)) {
    return true;
  } else {
    printf("Expected token %d.", token);
    exit(EXIT_FAILURE);
  }
}

bool Parserconsume_token(struct Parser *this, int p_token) {

  if (Parsermatch_token(this, p_token)) {
  }
  Parsernext_token(this);
}

///*///

///*///

int main() {

  ///*///

  struct Parser parser;
  Parser__init__(&parser);

  struct List LinesCache;
  List__init__(&LinesCache);

  if (Parsercheck_tokenOVDstr(&parser, "print")) {
    struct String str_to_write;
    String__init__OVDstr(&str_to_write, "printf();");
    char *cstr = Stringc_str(&str_to_write);
    ListappendOVDstr(&LinesCache, cstr);
    String__del__(&str_to_write);
  }

  Listprint(&LinesCache);

  List__del__(&LinesCache);
  Parser__del__(&parser);
  ///*///

  return 0;
}