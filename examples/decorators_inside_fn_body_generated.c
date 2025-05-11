#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Node {
  int m_data;
};

void Node__init__(struct Node *this, int p_data);
void Nodeprint(struct Node *this);

void Node__init__(struct Node *this, int p_data) { this->m_data = p_data; }

void Nodeprint(struct Node *this) {
  int data = this->m_data;

  //@hook_begin("custom_integer_printer" "int" data)
  printf("%d \n", data);
  //@hook_end
}

typedef void (*custom_integer_printer)(int);
void Nodeprint_hooked_custom_integer_printer(
    struct Node *this, custom_integer_printer p_custom_integer_printer) {
  int data = this->m_data;

  //
  p_custom_integer_printer(data);
}

void CustomIntPrint(int data) {
  printf("[CustomIntegerPrinter] Int = %d \n", data);
}

int main() {

  struct Node node;
  Node__init__(&node, 10);

  Nodeprint(&node);

  Nodeprint_hooked_custom_integer_printer(&node, CustomIntPrint);

  return 0;
}