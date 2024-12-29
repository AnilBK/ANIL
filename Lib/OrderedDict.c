// clang-format off
///*///

struct OrderedDictObject<T>{char *key_str, T value, Self *next};

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

c_function __getitem__(p_key : str) -> T:
  @TYPEOF(nodes) *node = this->nodes;
  while (node != NULL) {
    @TYPEOF(nodes) *temp = node;
    if (strcmp(node->key_str, p_key) == 0) {
      return node->value;
    }
    node = node->next;
    free(temp);
  }

  @TEMPLATED_DATA_TYPE@ item;
  return item;
endc_function

c_function __setitem__(p_key : str, p_value : T)
  if (!p_key || p_key[0] == '\0') {
    printf("OrderedDict: Key cannot be NULL or empty.\n");
    exit(EXIT_FAILURE);
  }

  @TYPEOF(nodes) *node = this->nodes;
  if (node == NULL) {
    // No items in the dict yet, add the new item as the first node.
    @TYPEOF(nodes) *new_node = (@TYPEOF(nodes)*)malloc(sizeof(@TYPEOF(nodes)));
    new_node->key_str = strdup(p_key);
    new_node->value = p_value;
    new_node->next = NULL;
    this->nodes = new_node;
  } else {
    @TYPEOF(nodes) *prev = NULL;
    while (node != NULL) {
      if (strcmp(node->key_str, p_key) == 0) {
        free(node->key_str);  // Free the old key string
        node->key_str = strdup(p_key);
        if (!node->key_str) {
          printf("OrderedDict: Memory allocation for key failed.\n");
          exit(EXIT_FAILURE);
          return;
        }
        node->value = p_value;
        return;
      }
      prev = node;
      node = node->next;
    }

    // Add a new node since the key was not found.
    @TYPEOF(nodes) *new_node = (@TYPEOF(nodes)*)malloc(sizeof(@TYPEOF(nodes)));
    new_node->key_str = strdup(p_key);
    new_node->value = p_value;
    new_node->next = NULL;
    prev->next = new_node;
  }
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

function get(key : str) -> Optional<T>:
  let res = Optional<T>{};
  if key in this{
    let value = this[key]
    res.set_value(value)
  }
  return res
endfunction

function push(p_key : str, p_value : T)
  this[p_key] = p_value
endfunction

endnamespace
///*///

