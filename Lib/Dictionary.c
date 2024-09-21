// clang-format off
#include <stdbool.h>
#include <string.h>

#define TABLE_SIZE 101 

typedef struct KeyValuePair{
  char *key_str;
  int value;
  struct KeyValuePair *next;
}KeyValuePair;

//We don't have arrays inside our custom struct datatype,so vvv.
typedef struct  {
    KeyValuePair *table[TABLE_SIZE];
}KeyValueTable;

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

///*///
struct Dictionary{KeyValueTable table};

namespace Dictionary

c_function __init__()
  for (int i = 0; i < TABLE_SIZE; i++) {
    this->table.table[i] = NULL;
  }
endc_function

c_function __del__()
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      KeyValuePair *next = pair->next;
      // free(pair->key_str);  
      // ^^^^^ This is not dynamically allocated key string, so shouldn't free it.
      free(pair);
      pair = next;
    }
  }  
endc_function

c_function __getitem__(p_key : str) -> int:
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return pair->value;
    }
    pair = pair->next;
  }
  return 0; 
endc_function

c_function __setitem__(p_key_str : str, p_value : int)
  unsigned int index = hash(p_key_str);
  KeyValuePair *new_pair = (KeyValuePair *)malloc(sizeof(KeyValuePair));
  new_pair->key_str = p_key_str;
  new_pair->value = p_value;
  new_pair->next = this->table.table[index];
  this->table.table[index] = new_pair;
endc_function

c_function __contains__(p_key : str) -> bool:
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return true;
    }
    pair = pair->next;
  }
  return false;
endc_function

c_function print()
  printf("{\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      printf("\"%s\" : %d,\n", pair->key_str, pair->value);
      pair = pair->next;
    }
  }
  printf("}\n");
endc_function
endnamespace
///*///
