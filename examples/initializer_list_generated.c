///*///

#include <string.h>

///////////////////////////////////////////

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

void insertEnd(Node **head, Node *newNode) {
  if (*head == NULL) {
    *head = newNode;
    return;
  }

  Node *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = newNode;
}

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct List {
  Node *head;
};

void List__init__(struct List *this) { this->head = NULL; }

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
  insertEnd(&this->head, int_node);
}

void Listappend_str(struct List *this, char *p_str) {
  Node *string_node = createStringNode(strdup(p_str));
  insertEnd(&this->head, string_node);
}

struct Vector__int {
  int *arr;
  int size;
  int capacity;
};

// template <int> {
void Vector_int__init__(struct Vector__int *this, int capacity) {
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

void Vector_int__del__(struct Vector__int *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_intpush(struct Vector__int *this, int value) {
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

void Vector_intallocate_more(struct Vector__int *this, int n) {
  this->capacity += n;
  this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_intpush_unchecked(struct Vector__int *this, int value) {
  this->arr[this->size++] = value;
}

void Vector_intprint(struct Vector__int *this) {
  // maybe print instanced vec name.
  // instanced name should be passed as fn parameter ig.
  // or add instance_name member silently to the struct itself.
  // printf("@INSTANCE_NAME@\n");
  /*

  void Vector_print(struct Vector* this, const char* p_instance_name){
  printf("%s\n", p_instance_name);
  }
  */

  printf("Dynamic Array (size = %zu, capacity = %zu) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    const char *type = "int";
    if (strcmp(type, "float") == 0) {
      printf("%f", this->arr[i]);
    } else if (strcmp(type, "int") == 0) {
      printf("%d", this->arr[i]);
    } else if (strcmp(type, "char") == 0) {
      printf("%c", this->arr[i]);
    }
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

bool Vector_int__contains__(struct Vector__int *this, int value) {
  // this returns bool.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t Vector_intlen(struct Vector__int *this) { return this->size; }

int Vector_int__getitem__(struct Vector__int *this, int index) {
  return *(this->arr + index);
}

// template <int> }

int main() {

  ///*///

  struct Vector__int values;
  Vector_int__init__(&values, 8);
  Vector_intpush(&values, 1);

  Vector_intpush(&values, 2);

  Vector_intpush(&values, 3);

  Vector_intpush(&values, 4);

  Vector_intpush(&values, 5);

  Vector_intpush(&values, 6);

  Vector_intpush(&values, 7);

  Vector_intpush(&values, 8);

  Vector_intallocate_more(&values, 8);

  Vector_intpush_unchecked(&values, 10);

  Vector_intpush_unchecked(&values, 20);

  Vector_intpush_unchecked(&values, 30);

  Vector_intpush_unchecked(&values, 40);

  Vector_intpush_unchecked(&values, 50);

  Vector_intpush_unchecked(&values, 60);

  Vector_intpush_unchecked(&values, 70);

  Vector_intpush_unchecked(&values, 80);

  struct List class_values;
  List__init__(&class_values);

  Listappend_str(&class_values, "arr");

  Listappend_str(&class_values, "size");

  Listappend_str(&class_values, "capacity");

  Listappend_str(&class_values, "__init__");

  Listappend_str(&class_values, "__del__");

  Listappend_str(&class_values, "push");

  Listappend_str(&class_values, "allocate_more");

  Listappend_str(&class_values, "push_unchecked");

  Listappend_str(&class_values, "print");

  Listappend_str(&class_values, "__contains__");

  Listappend_str(&class_values, "len");

  Listappend_str(&class_values, "__getitem__");

  Listprint(&class_values);

  List__del__(&class_values);
  Vector_int__del__(&values);
  ///*///

  return 0;
}