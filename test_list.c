#include <stdio.h>
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

// Function to create a new node
Node *createNode(Data data) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

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

// Function to insert a new node at the end of the list
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

// Function to print the linked list
void printList(Node *head) {
  Node *current = head;
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("%s ", current->data.str_data);
    } else {
      printf("%d ", current->data.int_data);
    }
    current = current->next;
  }
  printf("\n");
}

// Function to free memory allocated for the linked list
void freeList(Node *head) {
  Node *current = head;
  while (current != NULL) {
    Node *temp = current;
    current = current->next;

    if (temp->data_type == STRING) {
      free(temp->data.str_data);
    }

    free(temp);
  }
}

int main() {
  Node *head = NULL;

  Node *node1 = createIntNode(10);
  Node *node2 = createIntNode(20);
  Node *node3 = createIntNode(30);

  Node *nodeS1 = createStringNode(strdup("hello"));
  Node *nodeS2 = createStringNode(strdup("World"));
  Node *nodeS3 = createStringNode(strdup("Foo"));

  insertEnd(&head, node1);
  insertEnd(&head, node2);
  insertEnd(&head, node3);

  // Inserting some string data
  insertEnd(&head, nodeS1);
  insertEnd(&head, nodeS2);
  insertEnd(&head, nodeS3);

  // Print the linked list
  printList(head);

  // Free the memory allocated for the linked list
  freeList(head);

  return 0;
}
