///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

#include <stdlib.h>

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

///*///

///*///

/*
FIXME: The order in which normal C code and our custom code is generated is
different. So this function will be emitted earlier than the actual List struct
causing forward declaration errors. Temporarily we replace the entire function
body at all the needed callsites.

void ListinsertEnd(struct List *this, Node *newNode) {
// Add // to everyline because without it the code generated below,
// all goes to left & isn't formatted.
//  if (this->head == NULL) {
//    this->head = newNode;
//    this->tail = newNode;
//    return;
//  }
//
//  this->tail->next = newNode;
//  this->tail = newNode;
}
*/

///*///

///*///

#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
};

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

void Vector_Stringprint(struct Vector_String *this) {
  for (size_t i = 0; i < this->size; ++i) {
    printf("%s\n", this->arr[i].arr);
  }
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

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  return *(this->arr + index);
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

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

bool Stringis_of_length(struct String *this, int p_len) {
  return strlen(this->arr) == p_len;
}

char *Stringc_str(struct String *this) { return this->arr; }

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

  // Uses the same implementation as Split.
  char delimeter = '\n';

  struct Vector_String result;
  Vector_String__init__(&result, 10);

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

struct List {
  Node *head;
  Node *tail;
};

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
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
}

void Listprint(struct List *this) {
  Node *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\" ", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d ", data);
      //@hook_end
    }
    printf(",");
    current = current->next;
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
      printf("\"%s\" ", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //
      p_custom_integer_printer(data);
    }
    printf(",");
    current = current->next;
  }
  printf("]\n");
}

void Listappend_int(struct List *this, int p_value) {
  Node *int_node = createIntNode(p_value);
  // ListinsertEnd(this, int_node);
  //  TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = int_node;
    this->tail = int_node;
    return;
  }

  this->tail->next = int_node;
  this->tail = int_node;
}

void Listappend_str(struct List *this, char *p_str) {
  Node *string_node = createStringNode(strdup(p_str));
  // ListinsertEnd(this, string_node);
  //  TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = string_node;
    this->tail = string_node;
    return;
  }

  this->tail->next = string_node;
  this->tail = string_node;
}

int main() {

  ///*///

  struct String str;
  String__init__(&str, "Hello");
  StringprintLn(&str);

  Stringset_to_file_contents(&str, "fileexample.c");

  StringprintLn(&str);

  struct String l;
  String__init__(&l, "");
  struct List line;
  List__init__(&line);

  // Our own split by newlines algorithm.

  size_t tmp_len_0 = Stringlen(&str);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char c = String__getitem__(&str, i);

    if (c == '\n') {
      Listappend_str(&line, Stringc_str(&l));
      String__reassign__(&l, "");
      continue;
    }

    char c_promoted_0[2] = {c, '\0'};
    String__add__(&l, c_promoted_0);
  }

  // Add the remaining strings which don't end with \n.

  if (!String__eq__(&l, "")) {
    Listappend_str(&line, Stringc_str(&l));
    String__reassign__(&l, "");
  }

  Listprint(&line);

  List__del__(&line);
  String__del__(&l);
  String__del__(&str);
  ///*///

  return 0;
}