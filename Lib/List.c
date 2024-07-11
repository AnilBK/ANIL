// clang-format off
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

struct List{Node *head,Node *tail, int size};

namespace List

c_function __init__()
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
endc_function

c_function __del__()
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
endc_function

c_function len() -> size_t:
  return this->size;
endc_function

c_function __getitem__(index : int) -> Node:
  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(-1);
  }
    
  Node *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }
    
  return *current;
endc_function

c_function pop(index : int) -> Node:
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
endc_function

c_function print()
  Node *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\"", current->data.str_data);
    } else {
      int data = current->data.int_data;

      @hook_begin("custom_integer_printer" "int" data)
        printf("%d", data);
      @hook_end

    }
    
    current = current->next;
    if(current != NULL){
      printf(", ");
    }
  }
  printf("]\n");
endc_function
///*///

/* 
FIXME: The order in which normal C code and our custom code is generated is different.
So this function will be emitted earlier than the actual List struct causing forward declaration errors.
Temporarily we replace the entire function body at all the needed callsites.

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
c_function append_int(p_value : int)
  Node *int_node = createIntNode(p_value);
  //ListinsertEnd(this, int_node);
  // TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = int_node;
    this->tail = int_node;
    this->size++;
    return;
  }

  this->tail->next = int_node;
  this->tail = int_node;
  this->size++;
endc_function

c_function append_str(p_str : str)
  Node *string_node = createStringNode(strdup(p_str));
  //ListinsertEnd(this, string_node);
  // TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = string_node;
    this->tail = string_node;
    this->size++;
    return;
  }

  this->tail->next = string_node;
  this->tail = string_node;
  this->size++;
endc_function
endnamespace

///*///


