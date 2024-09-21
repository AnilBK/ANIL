#include <stdbool.h>
#include <string.h>

#define TABLE_SIZE 101

typedef struct KeyValuePair {
  char *key_str;
  int value;
  struct KeyValuePair *next;
} KeyValuePair;

// We don't have arrays inside our custom struct datatype,so vvv.
typedef struct {
  KeyValuePair *table[TABLE_SIZE];
} KeyValueTable;

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

///*///

///*///
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Dictionary {
  KeyValueTable table;
};

void Dictionary__init__(struct Dictionary *this) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    this->table.table[i] = NULL;
  }
}

void Dictionary__del__(struct Dictionary *this) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      KeyValuePair *next = pair->next;
      // free(pair->key_str);
      // ^^^^^ This is not dynamically allocated key string, so shouldn't free
      // it.
      free(pair);
      pair = next;
    }
  }
}

int Dictionary__getitem__(struct Dictionary *this, char *p_key) {
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return pair->value;
    }
    pair = pair->next;
  }
  return 0;
}

void Dictionary__setitem__(struct Dictionary *this, char *p_key_str,
                           int p_value) {
  unsigned int index = hash(p_key_str);
  KeyValuePair *new_pair = (KeyValuePair *)malloc(sizeof(KeyValuePair));
  new_pair->key_str = p_key_str;
  new_pair->value = p_value;
  new_pair->next = this->table.table[index];
  this->table.table[index] = new_pair;
}

bool Dictionary__contains__(struct Dictionary *this, char *p_key) {
  unsigned int index = hash(p_key);
  KeyValuePair *pair = this->table.table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return true;
    }
    pair = pair->next;
  }
  return false;
}

void Dictionaryprint(struct Dictionary *this) {
  printf("{\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    KeyValuePair *pair = this->table.table[i];
    while (pair != NULL) {
      printf("\"%s\" : %d,\n", pair->key_str, pair->value);
      pair = pair->next;
    }
  }
  printf("}\n");
}

int main() {

  ///*///  main()

  // Compile time dictionary. See constexpr_dict.c
  // Also See, Bootstrap/lexer_test.c.

  struct Dictionary dict;
  Dictionary__init__(&dict);
  // The following line is also valid, as the above syntax is shorthand for the
  // statement below. let dict = Dictionary{};
  Dictionary__setitem__(&dict, "One", 1);
  Dictionary__setitem__(&dict, "Two", 2);
  Dictionary__setitem__(&dict, "Three", 3);

  Dictionaryprint(&dict);

  Dictionary__del__(&dict);
  ///*///

  return 0;
}