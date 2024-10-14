// clang-format off
///*///

// Currently Implemented for Symbol Struct(preprocess_test.c).

struct OrderedDictObject<T>{char *key_str,T value,Self *next};

struct OrderedDict<T>{OrderedDictObject<T> *nodes};

namespace OrderedDict

c_function __init__()
  this->nodes = NULL;
endc_function

c_function __del__()
  @TYPEOF(nodes) *node = this->nodes;
  while (node != NULL) {
    @TYPEOF(nodes) *temp = node;
    node = node->next;
    free(temp);
  }
endc_function

c_function __getitem__(p_key : str) -> Symbol:
  @TYPEOF(nodes) *node = this->nodes;
  while (node != NULL) {
    @TYPEOF(nodes) *temp = node;
    if (strcmp(node->key_str, p_key) == 0) {
      return node->value;
    }
    node = node->next;
    free(temp);
  }

  @TYPEOF(nodes) item;
  return item;
endc_function

c_function __setitem__(p_key_str : str, p_value : int)
  // TODO!
  @TYPEOF(nodes) item;
  return item;
endc_function

c_function __contains__(p_key : str) -> bool:
  @TYPEOF(nodes) *node = this->nodes;
  while (node != NULL) {
    if (strcmp(node->key_str, p_key) == 0) {
      return true;
    }
    node = node->next;
  }
  return false;
endc_function

function get(key : str) -> Optional<Symbol>:
  let res = Optional<Symbol>{};
  if key in this{
    let value = this.__getitem__(key);
    res.set_value(value);
  }
  return res
endfunction

c_function push(symbol : Symbol)
    // TODO !
endc_function

endnamespace
///*///

