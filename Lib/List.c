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
struct ListObject{Variant data};

namespace ListObject

c_function __init__<>(p_value : int)
  this->data.data.int_data = p_value;
  this->data.data_type = INT;
endc_function

c_function __init__<>(p_value : str)
  this->data.data.str_data = strdup(p_value);
  this->data.data_type = STRING;
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

struct List{ListObject *items, int size, int capacity};

namespace List

function len() -> int:
  return this.size
endfunction

c_function __init__()
  this->items = NULL;
  this->size = 0;
  this->capacity = 0;
endc_function

c_function __del__()
  for(int i = 0; i < this->size; i++){
    ListObject__del__(&this->items[i]);
  }
  free(this->items);
  this->items = NULL;
  this->size = 0;
  this->capacity = 0;
endc_function

c_function _ensure_capacity(min_capacity : int)
  if (this->capacity >= min_capacity) {
    return;
  }

  int new_capacity = this->capacity == 0 ? 4 : this->capacity * 2;
  if (new_capacity < min_capacity) {
    new_capacity = min_capacity;
  }

  this->items = realloc(this->items, new_capacity * sizeof(struct ListObject));
  if (this->items == NULL) {
    printf("List: Failed to reallocate array to capacity %d.\n", new_capacity);
    exit(EXIT_FAILURE);
  }
  this->capacity = new_capacity;
endc_function

c_function __getitem__(index : int) -> ListObject:
  if (index < 0){
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }
    
  return ListObject_duplicate(&this->items[index]);
endc_function    

c_function __getitem_ref__(index : int) -> &ListObject:
  if (index < 0){
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }
    
  return this->items[index];
endc_function    

c_function _shift_left_from(index : int)
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).  
  for (int i = index; i < this->size - 1; i++) {
    this->items[i] = this->items[i + 1];
  }
endc_function  

function pop(index : int) -> ListObject:
  # Duplicate the item.
  # We cant return a reference to the popped item, as it will be freed when the list is modified (e.g when a new item is appended or another item is popped).
  let item = this[index]

  this._shift_left_from(index)
  this.size -= 1
  return item
endfunction

function __contains__<>(p_value : int) -> bool:
  for ref item in this{
    if item == p_value{
      return true
    }
  }
  return false
endfunction

function __contains__<>(p_value : str) -> bool:
  for ref item in this{
    if item == p_value{
      return true
    }
  }
  return false
endfunction

c_function print()
  printf("[");
  for (int i = 0; i < this->size; i++) {
    if (this->items[i].data.data_type == STRING) {
      printf("\"%s\"", this->items[i].data.data.str_data);
    } else {
      int data = this->items[i].data.data.int_data;

      @hook_begin("custom_integer_printer" "int" data)
        printf("%d", data);
      @hook_end
    }

    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
endc_function

c_function append_int(p_value : int)
  List_ensure_capacity(this, this->size + 1);
  ListObject__init__OVDint(&this->items[this->size], p_value);
  this->size++;
endc_function

c_function append_str(p_value : str)
  List_ensure_capacity(this, this->size + 1);
  ListObject__init__OVDstr(&this->items[this->size], p_value);
  this->size++;
endc_function

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

function __reassign__(p_list : List)
  this._ensure_capacity(this.size + p_list.size)
  for ref item in p_list{
    this.append(item)
  }
endfunction
endnamespace

///*///


