

#include <string.h>

typedef struct {
  char *key_str;
} Key;

typedef struct {
  Key key;
  int value;
} KeyValuePair;

// We don't have arrays inside our custom struct datatype,so vvv.
typedef struct {
  KeyValuePair pair[45];
} KeyValuePairs;

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Dictionary {
  int added_values;
  KeyValuePairs pairs;
};

void Dictionary__init__(struct Dictionary *this) { this->added_values = 0; }

void Dictionaryadd_key_value(struct Dictionary *this, char *p_key_str,
                             int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

void Dictionaryprint(struct Dictionary *this) {
  printf("{\n");
  for (int i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    printf("\"%s\" : %d,\n", key, value);
  }
  printf("}\n");
}

bool Dictionary__contains__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return true;
    }
  }
  return false;
}

int Dictionaryget_value_from_key(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

int Dictionary__getitem__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

void Dictionary__setitem__(struct Dictionary *this, char *p_key_str,
                           int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

int main() {

  ///*///

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

  ///*///

  return 0;
}