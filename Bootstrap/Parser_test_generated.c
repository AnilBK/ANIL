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

// Define a union for storing int or char*
typedef union {
  int int_data;
  char *str_data;
} Data;

enum DataType { INT, STRING };

// Define a struct for the node
typedef struct Node {
  Data data;
  enum DataType data_type;
  struct Node *next;
} Node;

Node *createIntNode(int p_int) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data.int_data = p_int;
  newNode->data_type = INT;
  newNode->next = NULL;
  return newNode;
}

Node *createStringNode(char *p_str) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data.str_data = p_str;
  newNode->data_type = STRING;
  newNode->next = NULL;
  return newNode;
}

typedef Node *Nodeptr;

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

void String__init__(struct String *this, char *text) {
  size_t p_text_length = strlen(text);
  String__init__from_charptr(this, text, p_text_length);
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

struct String Stringstrip(struct String *this) {
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
}

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

// template Vector<String> {
size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
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
  // Python Version of destructor.
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
  String__init__(&str, value.arr);

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

  return result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = strlen(this->arr) + strlen(pstring) + 1;

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
  this->arr = (char *)realloc(this->arr, (strlen(pstring) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, pstring);

  this->length = strlen(this->arr);
  this->capacity = this->length;
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

struct List {
  Node *head;
  Node *tail;
  int size;
};

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

void List__del__(struct List *this) {
  Node *current = this->head;
  while (current != NULL) {
    Node *temp = current;
    current = current->next;

    if (temp->data_type == STRING) {
      free(temp->data.str_data);
    }

    free(temp);
  }
  this->size = 0;
}

size_t Listlen(struct List *this) { return this->size; }

Node List__getitem__(struct List *this, int index) {
  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(-1);
  }

  Node *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  return *current;
}

Node Listpop(struct List *this, int index) {
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    // return NULL;
    exit(-1);
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(-1);
  }

  Node *current = this->head;
  Node *previous = NULL;

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

  Node popped_node = *current;
  if (current->data_type == STRING) {
    popped_node.data.str_data = strdup(current->data.str_data);
    free(current->data.str_data);
  }
  free(current);

  return popped_node;
}

void Listprint(struct List *this) {
  Node *current = this->head;
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
  Node *current = this->head;
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

void ListinsertEnd(struct List *this, Nodeptr newNode) {
  this->size++;
  if (this->head == NULL) {
    this->head = newNode;
    this->tail = newNode;
    return;
  }

  this->tail->next = newNode;
  this->tail = newNode;
}

void Listappend_int(struct List *this, int p_value) {
  Node *int_node = createIntNode(p_value);
  ListinsertEnd(this, int_node);
}

void Listappend_str(struct List *this, char *p_str) {
  Node *string_node = createStringNode(strdup(p_str));
  ListinsertEnd(this, string_node);
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

void Parser__init__(struct Parser *this) { printf("Parser Constructor.\n"); }

bool Parserhas_tokens_remaining(struct Parser *this) {
  return Listlen(&this->tokens) > 0;
}

Node Parsercurrent_token(struct Parser *this) {
  return List__getitem__(&this->tokens, 0);
}

void Parsernext_token(struct Parser *this) {
  Node node = Listpop(&this->tokens, 0);
}

Node Parserget_token(struct Parser *this) { return Listpop(&this->tokens, 0); }

bool Parsercheck_token(struct Parser *this, int token) {
  // return self.current_token() == token
  // TODO : this.current_token() returns Node, so we cant make direct
  // comparision with token in CPL.
  Node node = Parsercurrent_token(this);
  if (node.data_type == INT) {
    return node.data.int_data == token;
  } else {
    return false;
  }
}

bool Parsermatch_token(struct Parser *this, int token) {

  if (Parsercheck_token(this, token)) {
    return true;
  } else {
    printf("Expected token %d.", token);
  }
}

bool Parserconsume_token(struct Parser *this, int p_token) {
  Parsermatch_token(this, p_token);
  Parsernext_token(this);
}

///*///

///*///

int main() {

  ///*///

  struct Parser parser;
  Parser__init__(&parser);

  // TODO : Parser should construct List & free it as well.

  ///*///

  return 0;
}