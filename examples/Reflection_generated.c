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

  struct List vector_class_members;
  List__init__(&vector_class_members);
  struct List vector_class_member_functions;
  List__init__(&vector_class_member_functions);
  struct List list_instances;
  List__init__(&list_instances);

  Listappend_str(&vector_class_members, "arr");
  Listappend_str(&vector_class_members, "size");
  Listappend_str(&vector_class_members, "capacity");

  Listappend_str(&vector_class_member_functions, "__init__");
  Listappend_str(&vector_class_member_functions, "__del__");
  Listappend_str(&vector_class_member_functions, "push");
  Listappend_str(&vector_class_member_functions, "push");
  Listappend_str(&vector_class_member_functions, "allocate_more");
  Listappend_str(&vector_class_member_functions, "push_unchecked");
  Listappend_str(&vector_class_member_functions, "print");
  Listappend_str(&vector_class_member_functions, "print");
  Listappend_str(&vector_class_member_functions, "__contains__");
  Listappend_str(&vector_class_member_functions, "__contains__");
  Listappend_str(&vector_class_member_functions, "len");
  Listappend_str(&vector_class_member_functions, "__getitem__");

  Listappend_str(&list_instances, "vector_class_members");
  Listappend_str(&list_instances, "vector_class_member_functions");
  Listappend_str(&list_instances, "list_instances");

  printf("We print the different instances of class List using Reflection as "
         "follows: \n");

  // 'instances_of_class' returns name of the instances in "string" form,
  // so, we unquote that using UNQUOTE in macro as belows.
  Listprint(&vector_class_members);
  Listprint(&vector_class_member_functions);
  Listprint(&list_instances);

  List__del__(&list_instances);
  List__del__(&vector_class_member_functions);
  List__del__(&vector_class_members);
  ///*///

  return 0;
}