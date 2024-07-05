// clang-format off


#include <string.h>

typedef struct  {
  char *key_str;
} Key;

typedef struct  {
  Key key;
  int value;
}KeyValuePair;

//We don't have arrays inside our custom struct datatype,so vvv.
typedef struct  {
    KeyValuePair pair[45];
}KeyValuePairs;


///*///
struct Dictionary{int added_values,KeyValuePairs pairs};

namespace Dictionary

c_function __init__()
  this->added_values = 0;
endc_function

c_function add_key_value(p_key_str : str, p_value : int)
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
endc_function

c_function print()
  printf("{\n");
  for(int i = 0; i < this->added_values; i++){
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    printf("\"%s\" : %d,\n", key, value);
  }
  printf("}\n");
endc_function

c_function __contains__(p_key : str) -> bool:
  for (size_t i = 0; i < this->added_values; i++) {
      char *key = this->pairs.pair[i].key.key_str;
      int value = this->pairs.pair[i].value;
      if(strcmp(key,p_key) == 0){
        return true;
      }
  }    
  return false;  
endc_function


c_function get_value_from_key(p_key : str) -> int:
  for (size_t i = 0; i < this->added_values; i++) {
      char *key = this->pairs.pair[i].key.key_str;
      int value = this->pairs.pair[i].value;
      if(strcmp(key,p_key) == 0){
        return value;
      }
  }    
  return 0;  
endc_function

c_function __getitem__(p_key : str) -> int:
  for (size_t i = 0; i < this->added_values; i++) {
      char *key = this->pairs.pair[i].key.key_str;
      int value = this->pairs.pair[i].value;
      if(strcmp(key,p_key) == 0){
        return value;
      }
  }    
  return 0;  
endc_function

c_function __setitem__(p_key_str : str, p_value : int)
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
endc_function
endnamespace

///*///


