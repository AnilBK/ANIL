#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct Key {
  char *key_str;
};

struct KeyValuePair {
  struct Key key;
  int value;
};

struct Dictionary {
  unsigned int added_values;
  struct KeyValuePair pairs[10];
};

typedef struct Key Key;
typedef struct Dictionary Dictionary;
typedef struct KeyValuePair KeyValuePair;

void add_key_value(Dictionary *p_dict, char *p_key_str, int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  p_dict->pairs[p_dict->added_values++] = pair;
}

void print_dictionary(Dictionary *p_dict) {
  for (unsigned int i = 0; i < p_dict->added_values; i++) {
    printf("Key : %s. Value : %d\n", p_dict->pairs[i].key,
           p_dict->pairs[i].value);
  }
}

void key_present(Dictionary *p_dict, const char *p_str) {
  for (unsigned int i = 0; i < p_dict->added_values; i++) {
    char *str = p_dict->pairs[i].key.key_str;
    if (strcmp(str, p_str) == 0) {
      printf("Key found at index %d.\n", i);
      return;
    }
  }

  printf("Key doesn't exist.\n");
}

int main() {

  Dictionary d1;
  d1.added_values = 0;

  add_key_value(&d1, "Hello", 10);
  add_key_value(&d1, "World", 20);
  print_dictionary(&d1);

  key_present(&d1, "Hi");
  key_present(&d1, "World");

  return 0;
}