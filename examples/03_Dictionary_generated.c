
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 101

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct DictObject_int {
  char *key_str;
  int value;
  struct DictObject_int *next;
};

struct Dictionary_int {
  struct DictObject_int **table;
};

void Dictionary_int__init__(struct Dictionary_int *this);
void Dictionary_int__del__(struct Dictionary_int *this);
int Dictionary_int__getitem__(struct Dictionary_int *this, char *p_key);
void Dictionary_int__setitem__(struct Dictionary_int *this, char *p_key_str,
                               int p_value);
bool Dictionary_int__contains__(struct Dictionary_int *this, char *p_key);
void Dictionary_intprint(struct Dictionary_int *this);

void Dictionary_int__init__(struct Dictionary_int *this) {
  this->table = (struct DictObject_int **)malloc(
      TABLE_SIZE * sizeof(struct DictObject_int *));
  for (int i = 0; i < TABLE_SIZE; i++) {
    this->table[i] = NULL;
  }
}

void Dictionary_int__del__(struct Dictionary_int *this) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    struct DictObject_int *pair = this->table[i];
    while (pair != NULL) {
      struct DictObject_int *next = pair->next;
      // Do not free key_str since it's not dynamically allocated
      free(pair);
      pair = next;
    }
  }
  free(this->table); // Free the table itself
}

int Dictionary_int__getitem__(struct Dictionary_int *this, char *p_key) {
  unsigned int index = hash(p_key);
  struct DictObject_int *pair = this->table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return pair->value;
    }
    pair = pair->next;
  }
  return 0;
}

void Dictionary_int__setitem__(struct Dictionary_int *this, char *p_key_str,
                               int p_value) {
  unsigned int index = hash(p_key_str);
  struct DictObject_int *new_pair =
      (struct DictObject_int *)malloc(sizeof(struct DictObject_int));
  new_pair->key_str = p_key_str;
  new_pair->value = p_value;
  new_pair->next = this->table[index];
  this->table[index] = new_pair;
}

bool Dictionary_int__contains__(struct Dictionary_int *this, char *p_key) {
  unsigned int index = hash(p_key);
  struct DictObject_int *pair = this->table[index];
  while (pair != NULL) {
    if (strcmp(pair->key_str, p_key) == 0) {
      return true;
    }
    pair = pair->next;
  }
  return false;
}

void Dictionary_intprint(struct Dictionary_int *this) {
  printf("{\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    struct DictObject_int *pair = this->table[i];
    while (pair != NULL) {
      printf("\"%s\" : %d,\n", pair->key_str, pair->value);
      pair = pair->next;
    }
  }
  printf("}\n");
}

int main() {

  // Compile time dictionary. See constexpr_dict.c
  // Also See, Bootstrap/lexer_test.c.

  struct Dictionary_int dict;
  Dictionary_int__init__(&dict);
  // The following line is also valid, as the above syntax is shorthand for the
  // statement below. let dict = Dictionary<int>{};
  Dictionary_int__setitem__(&dict, "One", 1);
  Dictionary_int__setitem__(&dict, "Two", 2);
  Dictionary_int__setitem__(&dict, "Three", 3);

  Dictionary_intprint(&dict);

  Dictionary_int__del__(&dict);

  return 0;
}