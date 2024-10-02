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
#include <string.h>

#define TABLE_SIZE 101

typedef struct KeyValuePair {
  char *key_str;
  int value;
  struct KeyValuePair *next;
} KeyValuePair;

// We don't have arrays inside our custom struct datatype,so vvv.
typedef struct {
  KeyValuePair *table[TABLE_SIZE];
} KeyValueTable;

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

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

///*///
///////////////////////////////////////////
#include <stdlib.h>
#include <time.h>

///*///

///*///
// Dictionary of <int, list<String>>
// This is just for Symbol Table in preprocess_test as we dont have
// Dictionary<int, String> as of now.

// Ordered Dictionary of key(int) and value(list of string).

///*///

#include <stdbool.h>
#include <stdio.h>

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

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i].length == value.length) {
      if (strcmp(this->arr[i].arr, value.arr) == 0) {
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

struct Set {
  struct Vector_String arr;
};

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
  size_t tmp_len_0 = Setlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    struct String str = Set__getitem__(this, i);
    Stringprint(&str);
    printf(",");
  }
  printf("]\n");
}

struct Dictionary {
  KeyValueTable table;
};

void Dictionary__init__(struct Dictionary *this) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    this->table.table[i] = NULL;
  }
}

void Dictionary__del__(struct Dictionary *this) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      KeyValuePair *next = pair->next;
      // free(pair->key_str);
      // ^^^^^ This is not dynamically allocated key string, so shouldn't free
      // it.
      free(pair);
      pair = next;
    }
  }
}

int Dictionary__getitem__(struct Dictionary *this, char *p_key) {
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return pair->value;
    }
    pair = pair->next;
  }
  return 0;
}

void Dictionary__setitem__(struct Dictionary *this, char *p_key_str,
                           int p_value) {
  unsigned int index = hash(p_key_str);
  KeyValuePair *new_pair = (KeyValuePair *)malloc(sizeof(KeyValuePair));
  new_pair->key_str = p_key_str;
  new_pair->value = p_value;
  new_pair->next = this->table.table[index];
  this->table.table[index] = new_pair;
}

bool Dictionary__contains__(struct Dictionary *this, char *p_key) {
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return true;
    }
    pair = pair->next;
  }
  return false;
}

void Dictionaryprint(struct Dictionary *this) {
  printf("{\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      printf("\"%s\" : %d,\n", pair->key_str, pair->value);
      pair = pair->next;
    }
  }
  printf("}\n");
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
  if (index < 0) {
    index += this->size;
  }

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

bool List__contains__OVDint(struct List *this, int p_value) {
  CPLObject *current = this->head;
  while (current != NULL) {
    if (current->data_type == INT) {
      int data = current->data.int_data;

      if (data == p_value) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

bool List__contains__OVDstr(struct List *this, char *p_value) {
  CPLObject *current = this->head;
  while (current != NULL) {
    if (current->data_type == STRING) {
      char *data = current->data.str_data;

      if (strcmp(data, p_value) == 0) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
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

void ListappendOVDstructCPLObject(struct List *this, struct CPLObject p_value) {

  if (CPLObjectis_int(&p_value)) {
    Listappend_int(this, CPLObjectget_int(&p_value));
  } else if (CPLObjectis_str(&p_value)) {
    Listappend_str(this, CPLObjectget_str(&p_value));
  }
}

void List__reassign__(struct List *this, struct List p_list) {
  int tmp_len_1 = Listlen(&p_list);
  for (size_t i = 0; i < tmp_len_1; i++) {
    struct CPLObject item = List__getitem__(&p_list, i);
    ListappendOVDstructCPLObject(this, item);
    CPLObject__del__(&item);
  }
}

struct Random {
  char dummy;
};

void Random__init__(struct Random *this) { srand(time(0)); }

int Randomrandrange(struct Random *this, int upper_limit) {
  return rand() % upper_limit;
}

struct int_str_list {
  int key;
  struct Vector_String value;
};

void int_str_list__init__(struct int_str_list *this, int p_key) {
  this->key = p_key;
  Vector_String__init__(&this->value, 5);
}

void int_str_list__del__(struct int_str_list *this) {
  Vector_String__del__(&this->value);
}

struct Vector_int_str_list {
  struct int_str_list *arr;
  int size;
  int capacity;
};

// template Vector<int_str_list> {
size_t Vector_int_str_listlen(struct Vector_int_str_list *this) {
  return this->size;
}

struct int_str_list
Vector_int_str_list__getitem__(struct Vector_int_str_list *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_int_str_list__init__(struct Vector_int_str_list *this,
                                 int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr =
      (struct int_str_list *)malloc(capacity * sizeof(struct int_str_list));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_int_str_list__del__(struct Vector_int_str_list *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_int_str_listpush(struct Vector_int_str_list *this,
                             struct int_str_list value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct int_str_list *)realloc(
        this->arr, this->capacity * sizeof(struct int_str_list));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_int_str_listallocate_more(struct Vector_int_str_list *this, int n) {
  this->capacity += n;
  this->arr = (struct int_str_list *)realloc(
      this->arr, this->capacity * sizeof(struct int_str_list));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_int_str_listpush_unchecked(struct Vector_int_str_list *this,
                                       struct int_str_list value) {
  this->arr[this->size++] = value;
}

struct int_str_list Vector_int_str_listpop(struct Vector_int_str_list *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

bool Vector_int_str_list__contains__(struct Vector_int_str_list *this,
                                     struct int_str_list value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_int_str_listprint(struct Vector_int_str_list *this) {
  // Default overload.
  printf("Dynamic Array (size = %d, capacity = %d) : [ ]", this->size,
         this->capacity);
  // struct int_str_list will be replaced by the actual templated data type.
  printf("Unknown Format Specifier for type struct int_str_list.\n");
}

// template Vector<int_str_list> }

struct Dict_int_string {
  struct Vector_int_str_list pairs;
};

void Dict_int_string__init__(struct Dict_int_string *this) {
  Vector_int_str_list__init__(&this->pairs, 5);
}

bool Dict_int_string__contains__(struct Dict_int_string *this, int p_key) {
  bool found = false;
  size_t tmp_len_2 = Vector_int_str_listlen(&this->pairs);
  for (size_t i = 0; i < tmp_len_2; i++) {
    struct int_str_list pair = Vector_int_str_list__getitem__(&this->pairs, i);

    if (&pair.key == p_key) {
      found = true;
      // no break because the pair should be destructed at the end of the scope.
      // break doesn't do that as of now.
    }
    int_str_list__del__(&pair);
  }
  return found;
}

void Dict_int_stringkey_exists_quit(struct Dict_int_string *this) {
  fprintf(stderr, "Pop from empty Vector.\n");
  exit(EXIT_FAILURE);
}

void Dict_int_stringadd_key(struct Dict_int_string *this, int p_key) {

  if (Dict_int_string__contains__(this, p_key)) {
    Dict_int_stringkey_exists_quit(this);
  } else {
    struct int_str_list s;
    int_str_list__init__(&s, p_key);
    Vector_int_str_listpush(&this->pairs, s);
    int_str_list__del__(&s);
  }
}

void Dict_int_string__del__(struct Dict_int_string *this) {
  Vector_int_str_list__del__(&this->pairs);
}

struct StructInstance {
  struct String struct_type;
  struct String struct_name;
  bool is_templated;
  struct String templated_data_type;
  int scope;
  bool should_be_freed;
  bool is_pointer_type;
};

void StructInstance__init__(struct StructInstance *this,
                            struct String p_struct_type,
                            struct String p_struct_name, bool p_is_templated,
                            struct String p_templated_data_type, int p_scope) {
  String__init__OVDstructString(&this->struct_type, p_struct_type);
  String__init__OVDstructString(&this->struct_name, p_struct_name);
  String__init__OVDstructString(&this->templated_data_type,
                                p_templated_data_type);

  this->is_templated = p_is_templated;

  this->scope = p_scope;

  this->should_be_freed = true;

  this->is_pointer_type = false;
}

bool StructInstanceis_templated_instance(struct StructInstance *this) {
  return this->is_templated;
}

bool StructInstanceshould_struct_be_freed(struct StructInstance *this) {
  return this->should_be_freed;
}

void StructInstance__del__(struct StructInstance *this) {
  String__del__(&this->struct_type);
  String__del__(&this->struct_name);
  String__del__(&this->templated_data_type);
}

struct Symbol {
  struct String name;
  struct String data_type;
};

void Symbol__init__(struct Symbol *this, struct String p_name,
                    struct String p_data_type) {
  String__init__OVDstructString(&this->name, p_name);

  String__init__OVDstr(&this->data_type, "");
  // don't initialize data_type directly from p_data_type, so we can see,
  // reassign parsing is working as expected.
  String__reassign__OVDstructString(&this->data_type, p_data_type);
}

void Symbol__del__(struct Symbol *this) {
  String__del__(&this->name);
  String__del__(&this->data_type);
}

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

// template Vector<int> {
size_t Vector_intlen(struct Vector_int *this) { return this->size; }

int Vector_int__getitem__(struct Vector_int *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

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

void Vector_int__del__(struct Vector_int *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_intpush(struct Vector_int *this, int value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_intallocate_more(struct Vector_int *this, int n) {
  this->capacity += n;
  this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_intpush_unchecked(struct Vector_int *this, int value) {
  this->arr[this->size++] = value;
}

int Vector_intpop(struct Vector_int *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
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

// template Vector<int> }

struct SymbolTable {
  struct Dict_int_string symbols;
  struct Vector_int scope_stack;
};

void SymbolTable__init__(struct SymbolTable *this) {
  Dict_int_string__init__(&this->symbols);
  Vector_int__init__(&this->scope_stack, 5);
}

int SymbolTablecurrent_scope(struct SymbolTable *this) {
  return Vector_int__getitem__(&this->scope_stack, -1);
}

int SymbolTablenew_unique_scope_id(struct SymbolTable *this) {

  if (Vector_intlen(&this->scope_stack) == 0) {
    // return 0
  }

  int latest_scope = SymbolTablecurrent_scope(this);
  int new_scope = latest_scope + 1;

  while (Vector_int__contains__(&this->scope_stack, new_scope)) {
    new_scope = Randomrandrange(&random, 100000);
  }

  return new_scope;
}

void SymbolTableenter_scope(struct SymbolTable *this) {
  int new_scope_id = SymbolTablenew_unique_scope_id(this);
  Vector_intpush(&this->scope_stack, new_scope_id);
  // this.symbols[new_scope_id] = OrderedDict()
  Dict_int_stringadd_key(&this->symbols, new_scope_id);
}

struct String
SymbolTabledestructor_for_all_variables_in_scope(struct SymbolTable *this,
                                                 int scope_id) {
  // Return the destructor code for all variables in the provided scope
  // And, free(unregister) those variables as well.
  struct String des_code;
  String__init__OVDstr(&des_code, "");

  if (Dict_int_string__contains__(&this->symbols, scope_id)) {
    // for variable in reversed(this.symbols[scope_id]):
    //     # Call the destructor for the variable
    //     # print(f"Destroying variable '{variable}' in scope {scope_id}")
    //     code = get_destructor_for_struct(variable)
    //     if code != None:
    //         # print(f"~() = {code}")
    //         des_code += code
    //     remove_struct_instance(variable)
    // del this.symbols[scope_id]
  }
  return des_code;
}

void SymbolTableexit_scope(struct SymbolTable *this) {

  if (Vector_intlen(&this->scope_stack) > 0) {
    int exiting_scope_id = Vector_intpop(&this->scope_stack);
    struct String destructor_code =
        SymbolTabledestructor_for_all_variables_in_scope(this,
                                                         exiting_scope_id);

    if (!(String__eq__(&destructor_code, ""))) {
      // LinesCache.append(destructor_code)
    }
    String__del__(&destructor_code);
  }

  // No more scopes remaining.
  // Create one so current_scope() wont return list index out of range, because
  // scope_stack is empty.

  if (Vector_intlen(&this->scope_stack) == 0) {
    SymbolTableenter_scope(this);
  }
}

void SymbolTableprint_symbol_table(struct SymbolTable *this) {
  printf("-------------------Symbol Table------------------");
  printf("Unimplemented");
  printf("-------------------------------------------------");
}

void SymbolTabledeclare_variable(struct SymbolTable *this, struct String name,
                                 struct String p_type) {
  int current_scope = SymbolTablecurrent_scope(this);

  // if name in this.symbols[current_scope]:
  //     this.print_symbol_table()
  //     RAISE_ERROR(f"Variable '{name}' already declared in this scope.")

  size_t tmp_len_3 = Vector_intlen(&this->scope_stack);
  for (size_t i = 0; i < tmp_len_3; i++) {
    int scope = Vector_int__getitem__(&this->scope_stack, i);
    //     if name in this.symbols[scope]:
    //         this.print_symbol_table()
    //         RAISE_ERROR(
    //             f"Variable '{name}' already declared in previous scope
    //             {scope}."
    //         )
  }

  // this.symbols[current_scope][name] = Symbol(name, p_type)
}

void SymbolTablefind_variable(struct SymbolTable *this, struct String name) {
  // for scope in this.scope_stack[::-1]{
  //     if name in this.symbols[scope]:
  //         return this.symbols[scope][name]
  // }
  // return None
}

struct String SymbolTabledestructor_code_for_all_remaining_variables(
    struct SymbolTable *this) {
  struct String destructor_code;
  String__init__OVDstr(&destructor_code, "");

  while (true) {

    if (Vector_intlen(&this->scope_stack) > 0) {
      int exiting_scope_id = Vector_intpop(&this->scope_stack);
      struct String des_code = SymbolTabledestructor_for_all_variables_in_scope(
          this, exiting_scope_id);

      if (!(String__eq__(&des_code, ""))) {
        String__add__(&destructor_code, Stringc_str(&des_code));
      }
      String__del__(&des_code);
    } else {
      break;
    }
  }
  return destructor_code;
}

void SymbolTable__del__(struct SymbolTable *this) {
  Vector_int__del__(&this->scope_stack);
}

///*///

// Insert a string at a given index in another string.
struct String insert_string(struct String original_string, int p_index,
                            struct String string_to_insert) {
  // return original_string[:index] + string_to_insert + original_string[index:]
  struct String left_part = Stringsubstr(&original_string, 0, p_index);
  struct String tmp_string_0 = Stringsubstr(
      &original_string, p_index, Stringlen(&original_string) - p_index);
  String__add__(&left_part, Stringc_str(&string_to_insert));
  String__add__(&left_part, Stringc_str(&tmp_string_0));
  String__del__(&tmp_string_0);
  return left_part;
}

struct String escape_quotes(struct String s) {
  // Add \ in front of any " in the string.
  // if we find \", then we don't add \ in front of ".
  struct String result;
  String__init__OVDstr(&result, "");
  size_t len = Stringlen(&s);

  for (size_t i = 0; i < len; i++) {
    char c = String__getitem__(&s, i);

    if (c == '\"') {

      if (i == 0) {
        String__add__(&result, "\\");
      } else {

        if (!(String__getitem__(&s, i - 1) == '\\')) {
          String__add__(&result, "\\");
        }
      }
    }
    char c_promoted_0[2] = {c, '\0'};
    String__add__(&result, c_promoted_0);
  }
  return result;
}

struct String get_format_specifier(struct String p_type) {
  struct String return_type_str;
  String__init__OVDstr(&return_type_str, "d");

  if (String__eq__(&p_type, "char")) {
    String__reassign__OVDstr(&return_type_str, "c");
  } else if (String__eq__(&p_type, "int")) {
    String__reassign__OVDstr(&return_type_str, "d");
  } else if (String__eq__(&p_type, "float")) {
    String__reassign__OVDstr(&return_type_str, "f");
  } else if (String__eq__(&p_type, "size_t")) {
    String__reassign__OVDstr(&return_type_str, "llu");
  }
  return return_type_str;
}

struct String get_mangled_fn_name(struct String p_struct_type,
                                  struct String p_fn_name) {
  struct String s;
  String__init__OVDstructString(&s, p_struct_type);
  String__add__(&s, Stringc_str(&p_fn_name));
  return s;
}

struct String
get_templated_mangled_fn_name(struct String p_struct_type1,
                              struct String p_fn_name1,
                              struct String p_templated_data_type1) {
  struct String s1;
  String__init__OVDstructString(&s1, p_struct_type1);
  String__add__(&s1, "_");
  String__add__(&s1, Stringc_str(&p_templated_data_type1));
  String__add__(&s1, Stringc_str(&p_fn_name1));
  return s1;
}

// Cached Items.
// If any structs have __init__ method, then we register them here.
// This could be stored in StructDefination.
struct Set structs_with_constructors;

bool has_constructors(struct String p_struct_type) {
  return Set__contains__(&structs_with_constructors, p_struct_type);
}

struct Random random;

struct SymbolTable symbol_table;

///*///

int main() {

  ///*/// main()
  // Global Variables Initialization.
  Set__init__(&structs_with_constructors, 5);
  Random__init__(&random);
  SymbolTable__init__(&symbol_table);

  struct String source_file;
  String__init__OVDstr(&source_file, "../examples/01_variables.c");
  // source_file.printLn()

  // output_file_name = source_file.split(".")[0] + "_generated.c"
  // let _split = source_file.split(" ")
  // let output_file_name = _split[0]
  // output_file_name += "_generated.c"
  // output_file_name.print()

  struct String file;
  String__init__OVDstr(&file, "");
  StringprintLn(&file);

  struct Vector_String Lines =
      StringreadlinesFrom(&file, Stringc_str(&source_file));
  Vector_Stringprint(&Lines);

  struct Vector_String imported_modules;
  Vector_String__init__(&imported_modules, 5);

  size_t tmp_len_4 = Vector_Stringlen(&Lines);
  for (size_t i = 0; i < tmp_len_4; i++) {
    struct String line = Vector_String__getitem__(&Lines, i);
    struct String Line = Stringstrip(&line);

    if (Stringstartswith(&Line, "import")) {
      struct Vector_String import_split = Stringsplit(&Line, ' ');
      // let module_name = import_split[1]
      // module_name.printLn()

      Vector_Stringpush(&imported_modules,
                        Vector_String__getitem__(&import_split, 1));

      StringprintLn(&Line);
      Vector_String__del__(&import_split);
    }
    String__del__(&Line);
  }

  if (Vector_Stringlen(&imported_modules) > 0) {
    struct Vector_String ImportedCodeLines;
    Vector_String__init__(&ImportedCodeLines, 50);

    size_t tmp_len_5 = Vector_Stringlen(&imported_modules);
    for (size_t i = 0; i < tmp_len_5; i++) {
      struct String module_name =
          Vector_String__getitem__(&imported_modules, i);
      struct String relative_path;
      String__init__OVDstr(&relative_path, "../Lib/");
      String__add__(&relative_path, Stringc_str(&module_name));
      String__add__(&relative_path, ".c");

      StringprintLn(&relative_path);

      struct String module_file;
      String__init__OVDstr(&module_file, "");
      struct Vector_String lines =
          StringreadlinesFrom(&module_file, Stringc_str(&relative_path));
      // lines.print()

      // ImportedCodeLines += lines
      size_t tmp_len_6 = Vector_Stringlen(&lines);
      for (size_t j = 0; j < tmp_len_6; j++) {
        struct String line = Vector_String__getitem__(&lines, j);
        Vector_Stringpush(&ImportedCodeLines, line);
      }
      Vector_String__del__(&lines);
      String__del__(&module_file);
      String__del__(&relative_path);
    }
    Vector_String__del__(&ImportedCodeLines);
  }

  struct String s1;
  String__init__OVDstr(&s1, "Hello World");
  struct String insert;
  String__init__OVDstr(&insert, "virus");
  int index = 2;

  struct String new_string = insert_string(s1, index, insert);
  Stringprint(&new_string);

  String__del__(&new_string);
  String__del__(&insert);
  String__del__(&s1);
  Vector_String__del__(&imported_modules);
  Vector_String__del__(&Lines);
  String__del__(&file);
  String__del__(&source_file);
  SymbolTable__del__(&symbol_table);
  Set__del__(&structs_with_constructors);
  ///*///

  return 0;
}