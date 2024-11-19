// clang-format off
#include <stdlib.h>
#include <string.h>

typedef struct Variant{
  enum { 
    INT, 
    STRING 
  } data_type;

  union {
    int int_data;
    char *str_data;
  } data;
} Variant;

typedef struct Variant Variant;
typedef struct ListObject *ListObjectptr;

///*///
struct ListObject{Variant data, Self* next};

namespace ListObject

c_function __init__<>(p_value : int)
  this->data.data.int_data = p_value;
  this->data.data_type = INT;
  this->next = NULL;
endc_function

c_function __init__<>(p_value : str)
  this->data.data.str_data = strdup(p_value);
  this->data.data_type = STRING;
  this->next = NULL;
endc_function

c_function is_int() -> bool:
  return this->data.data_type == INT;
endc_function

c_function get_int() -> int:
  return this->data.data.int_data;
endc_function

c_function is_str() -> bool:
  return this->data.data_type == STRING;
endc_function

c_function get_str() -> str:
  return this->data.data.str_data;
endc_function

c_function _clear_str()
  free(this->data.data.str_data);
endc_function

function __del__()
  if this.is_str(){
    this._clear_str()
  }
endfunction

c_function _duplicate() -> ListObject:
  // Perform a deep copy.
  struct ListObject copy = *this;
  if (this->data.data_type == STRING) {
    copy.data.data.str_data = strdup(this->data.data.str_data);
  }
  return copy;
endc_function

function __eq__<>(p_value : int) -> bool:
  if this.is_int(){
    return this.get_int() == p_value
  }else{
    return false
  }
endfunction

function __eq__<>(p_value : str) -> bool:
  if this.is_str(){
    return this.get_str() == p_value
  }else{
    return false
  }
endfunction
endnamespace

struct List{ListObject *head, ListObject *tail, int size};

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
  struct ListObject *current = this->head;
  while (current != NULL) {
    struct ListObject *temp = current;
    current = current->next;

    ListObject__del__(temp);
    free(temp);
  }
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
endc_function

c_function __getitem__(index : int) -> ListObject:
  if (index < 0){
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }
    
  struct ListObject *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }
    
  // Duplicate contents of node and return it.
  // If we return a reference, then the calling function will call destructor,
  // which will free the str_data causing free() errors later.  
  return ListObject_duplicate(current);
endc_function

c_function pop(index : int) -> ListObject:
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    exit(EXIT_FAILURE);
  }
  
  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  struct ListObject *current = this->head;
  struct ListObject *previous = NULL;

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

  struct ListObject popped_node = *current;
  // Don't free current->data.str_data even though current data_type is String.
  // After copying the *pointer above, popped_node now owns current->data.str_data.
  // This avoids duplicating current->data.str_data into popped_node.
  free(current);
  return popped_node;
endc_function

c_function __contains__<>(p_value : int) -> bool:
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == INT) {
      int data = current->data.data.int_data;

      if (data == p_value){
        return true;
      }

    }
    current = current->next;
  }
  return false;
endc_function

c_function __contains__<>(p_value : str) -> bool:
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      char* data = current->data.data.str_data;

      if (strcmp(data,p_value) == 0){
        return true;
      }

    }
    current = current->next;
  }
  return false;
endc_function

c_function print()
  struct ListObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      printf("\"%s\"", current->data.data.str_data);
    } else {
      int data = current->data.data.int_data;

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

c_function _insert_end(new_node : ListObjectptr) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
endc_function

c_function create_int_node(p_value : int) -> ListObjectptr:
  struct ListObject *new_node = (struct ListObject *)malloc(sizeof(struct ListObject));
  if(new_node == NULL){
    printf("List : Failed to allocate a new node of type int for value %d.", p_value);
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDint(new_node, p_value);
  return new_node;
endc_function

c_function create_string_node(p_value : str) -> ListObjectptr:
  struct ListObject *new_node = (struct ListObject *)malloc(sizeof(struct ListObject));
  if(new_node == NULL){
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDstr(new_node, p_value);
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

function append<>(p_value : ListObject)
  if p_value.is_int(){
    this.append_int(p_value.get_int())
  }else if p_value.is_str(){
    this.append_str(p_value.get_str())
  }
endfunction

function __reassign__(p_list: List)
  for item in p_list{
    this.append(item)
  }
endfunction

endnamespace

///*///


