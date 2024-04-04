#include <stdbool.h>
#include <stdio.h>

int a[] = {5, 10, 25, 100, 500};

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
  int length = 5;

  Iterator iter = create_iterator_from_array(a, length);

  while (can_iterate(iter)) {
    int val = next_iter(&iter);
    printf("%d\n", val);
  }

  printf("\nEnumerator from array: \n");

  Enumerator enumerator;
  enumerator.index = -1;

  while (enumerate(&iter, &enumerator)) {
    printf("i = %d val = %d\n", enumerator.index, enumerator.val);
  }

  return 0;
}