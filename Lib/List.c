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

struct List{Node *head};

impl List __init__
  this->head = NULL;
endfunc

impl List __del__
  Node *current = this->head;
  while (current != NULL) {
    Node *temp = current;
    current = current->next;

    if (temp->data_type == STRING) {
      free(temp->data.str_data);
    }

    free(temp);
  }
endfunc

impl List print
  Node *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\" ", current->data.str_data);
    } else {
      int data = current->data.int_data;

      @hook_begin("custom_integer_printer" "int" data)
        printf("%d ", data);
      @hook_end

    }
    printf(",");
    current = current->next;
  }
  printf("]\n");
endfunc

impl List append_int p_value:int
  Node *int_node = createIntNode(p_value);
  insertEnd(&this->head, int_node);
endfunc

impl List append_str p_str:str
  Node *string_node = createStringNode(strdup(p_str));
  insertEnd(&this->head, string_node);
endfunc



///*///


