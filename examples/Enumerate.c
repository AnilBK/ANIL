#include <stdbool.h>
#include <stdio.h>

struct iterator {
  int *begin;
  int *end;
  int *curr;
};

struct enumerator {
  int index;
  int val;
};

typedef struct iterator Iterator;
typedef struct enumerator Enumerator;

bool can_iterate(Iterator iter) { return iter.curr <= iter.end; }

int curr_iter_value(Iterator iter) { return *iter.curr; }

int next_iter(Iterator *iter) {
  int val = curr_iter_value(*iter);
  iter->curr++;
  return val;
}

bool enumerate(Iterator *iter, Enumerator *enumerator) {
  if (!can_iterate(*iter)) {
    return false;
  }

  enumerator->index++;
  enumerator->val = next_iter(iter);

  return true;
}

Iterator create_iterator_from_array(int *array, int len) {
  Iterator it;
  it.begin = array;
  it.end = array + len - 1;
  it.curr = it.begin;
  return it;
}

int main() {

  // clang-format off

  ///*/// main()

  let arr<int> = [ 1, 2, 3, 4, 5 ];
  for index,value in enumerate arr{
    print("index : {index} value : {value} \n")
  }

  ///*///

  // clang-format on

  return 0;
}