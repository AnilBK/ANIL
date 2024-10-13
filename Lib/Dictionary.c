// clang-format off

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define TABLE_SIZE 101 

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

///*///

struct DictObject<T>{char *key_str,T value,Self *next};

struct Dictionary<T>{DictObject<T> **table};

namespace Dictionary

c_function __init__()
  this->table = (@TYPEOF(table) **)malloc(TABLE_SIZE * sizeof(@TYPEOF(table) *));
  for (int i = 0; i < TABLE_SIZE; i++) {
    this->table[i] = NULL; 
  }
endc_function

c_function __del__()
  for (int i = 0; i < TABLE_SIZE; i++) {
    @TYPEOF(table) *pair = this->table[i]; 
    while (pair != NULL) {
      @TYPEOF(table) *next = pair->next;
      // Do not free key_str since it's not dynamically allocated
      free(pair);
      pair = next;
    }
  }
  free(this->table); // Free the table itself
endc_function

c_function __getitem__(p_key : str) -> int:
  unsigned int index = hash(p_key);
  @TYPEOF(table) *pair = this->table[index];
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
  @TYPEOF(table) *new_pair = (@TYPEOF(table) *)malloc(sizeof(@TYPEOF(table)));
  new_pair->key_str = p_key_str;
  new_pair->value = p_value;
  new_pair->next = this->table[index];
  this->table[index] = new_pair;
endc_function

c_function __contains__(p_key : str) -> bool:
  unsigned int index = hash(p_key);
  @TYPEOF(table) *pair = this->table[index];
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
    @TYPEOF(table) *pair = this->table[i];
    while (pair != NULL) {
      printf("\"%s\" : %d,\n", pair->key_str, pair->value);
      pair = pair->next;
    }
  }
  printf("}\n");
endc_function
endnamespace
///*///

