// clang-format off
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
struct CPLObject{CPLObject_Data data, CPLObject_DataType data_type, CPLObject* next};

namespace CPLObject

c_function __init__<>(p_value : int)
  this->data.int_data = p_value;
  this->data_type = INT;
  this->next = NULL;
endc_function

c_function __init__<>(p_value : str)
  this->data.str_data = strdup(p_value);
  this->data_type = STRING;
  this->next = NULL;
endc_function

c_function is_int() -> bool:
  return this->data_type == INT;
endc_function

c_function get_int() -> int:
  return this->data.int_data;
endc_function

c_function is_str() -> bool:
  return this->data_type == STRING;
endc_function

c_function get_str() -> str:
  return this->data.str_data;
endc_function

c_function _clear_str()
  free(this->data.str_data);
endc_function

function __del__()
  if this.is_str(){
    this._clear_str()
  }
endfunction

c_function _duplicate() -> CPLObject:
  // Perform a deep copy.
  CPLObject copy = *this;
  if (this->data_type == STRING) {
    copy.data.str_data = strdup(this->data.str_data);
  }
  return copy;
endc_function

function __eq__<>(p_value : int) -> bool:
  if this.is_int(){
    let val = this.get_int() 
    return val == p_value
  }else{
    return false
  }
endfunction

function __eq__<>(p_value : str) -> bool:
  if this.is_str(){
    let val = this.get_str() 
    return val == p_value
  }else{
    return false
  }
endfunction
endnamespace

struct List{CPLObject *head, CPLObject *tail, int size};

namespace List

c_function len() -> int:
  return this->size;
endc_function

c_function __init__()
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
endc_function

c_function __del__()
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
endc_function

c_function __getitem__(index : int) -> CPLObject:
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
endc_function

c_function pop(index : int) -> CPLObject:
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
  // After copying the *pointer above, popped_node now owns current->data.str_data.
  // This avoids duplicating current->data.str_data into popped_node.
  free(current);
  return popped_node;
endc_function

c_function print()
  CPLObject *current = this->head;
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

c_function _insert_end(new_node : CPLObjectptr) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
endc_function

c_function create_int_node(p_value : int) -> CPLObjectptr:
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if(new_node == NULL){
    printf("List : Failed to allocate a new node of type int for value %d.", p_value);
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDint(new_node, p_value);
  return new_node;
endc_function

c_function create_string_node(p_value : str) -> CPLObjectptr:
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if(new_node == NULL){
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDstr(new_node, p_value);
  return new_node;
endc_function

function append_int(p_value : int)
  let int_node = this.create_int_node(p_value)
  this._insert_end(int_node)
endfunction

function append_str(p_value : str)
  let str_node = this.create_string_node(p_value)
  this._insert_end(str_node)
endfunction

function append<>(p_value : int)
  this.append_int(p_value)
endfunction

function append<>(p_value : str)
  this.append_str(p_value)
endfunction
endnamespace

///*///


