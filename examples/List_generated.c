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
  if (this->head == NULL) {
    this->head = newNode;
    this->tail = newNode;
    return;
  }

  this->tail->next = newNode;
  this->tail = newNode;
}
*/

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

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

  struct List test_list;
  List__init__(&test_list);
  Listappend_str(&test_list, "Hello");
  Listappend_str(&test_list, "World");
  Listappend_str(&test_list, "Puppy");

  Listappend_int(&test_list, 10);
  Listappend_int(&test_list, 20);
  Listappend_int(&test_list, 30);
  Listappend_int(&test_list, 40);
  Listappend_int(&test_list, 50);

  Listprint(&test_list);

  List__del__(&test_list);
  ///*///

  return 0;
}