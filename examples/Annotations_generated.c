#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef void (*fn_ptr)();

typedef struct {
  char *key_str;
} Key;

typedef struct {
  Key key;
  fn_ptr value;
} KeyValuePair;

typedef struct {
  KeyValuePair pair[45];
} KeyValuePairs;

// IMPORTS //

struct FunctionDictionary {
  int added_values;
  KeyValuePairs pairs;
};

void FunctionDictionary__init__(struct FunctionDictionary *this) {
  this->added_values = 0;
}

void FunctionDictionaryadd_key_value(struct FunctionDictionary *this,
                                     char *p_key_str, fn_ptr p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

void FunctionDictionaryHandleRequest(struct FunctionDictionary *this,
                                     char *p_str) {
  for (int i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    if (strcmp(key, p_str) == 0) {
      fn_ptr value = this->pairs.pair[i].value;
      value();
      return;
    }
  }
  printf("%s is not a valid route.\n", p_str);
}

///*///

void Home() { printf("Hello World\n"); }

void About() { printf("Anil BK\n"); }

///*///

int main() {

  ///*/// main()

  struct FunctionDictionary router;
  FunctionDictionary__init__(&router);

  // Add all the annotated functions using a macro.
  // The macro performs text replace operations so, "annotation_argument_value"
  // in the macro below is not a string. It will be replaced by actual value of
  // 'annotation_argument_value'.

  FunctionDictionaryadd_key_value(&router, "/Home", Home);
  FunctionDictionaryadd_key_value(&router, "/About", About);

  FunctionDictionaryadd_key_value(&router, "home", Home);

  FunctionDictionaryHandleRequest(&router, "/Home");
  FunctionDictionaryHandleRequest(&router, "/About");
  FunctionDictionaryHandleRequest(&router, "/Home/1");

  ///*///

  return 0;
}