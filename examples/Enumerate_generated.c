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

  int arr[] = {1, 2, 3, 4, 5};
  unsigned int arr_array_size = 5;

  Iterator arr_iter = create_iterator_from_array(arr, arr_array_size);
  Enumerator arr_enumerator;
  arr_enumerator.index = -1;

  while (enumerate(&arr_iter, &arr_enumerator)) {
    int index = arr_enumerator.index;
    int value = arr_enumerator.val;
    printf("index : %d value : %d \n", index, value);
  }

  return 0;
}