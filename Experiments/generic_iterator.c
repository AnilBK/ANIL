#include <stdbool.h>
#include <stdio.h>

// int a[] = {5, 10, 25, 100, 500};

char a[12] = "Hello World";

#define Iterator(T)                                                            \
  struct T##_iterator {                                                        \
    T *begin;                                                                  \
    T *end;                                                                    \
    T *curr;                                                                   \
  };                                                                           \
                                                                               \
  typedef struct T##_iterator T##Iterator;                                     \
  bool can_iterate_##T(T##Iterator iter) { return iter.curr <= iter.end; }     \
                                                                               \
  T curr_iter_value_##T(T##Iterator iter) { return *iter.curr; }               \
                                                                               \
  T next_iter_##T(T##Iterator *iter) {                                         \
    T val = curr_iter_value_##T(*iter);                                        \
    iter->curr++;                                                              \
    return val;                                                                \
  }                                                                            \
                                                                               \
  T##Iterator create_##T##iterator_from_array(T *array, int len) {             \
    T##Iterator it;                                                            \
    it.begin = array;                                                          \
    it.end = array + len - 1;                                                  \
    it.curr = it.begin;                                                        \
    return it;                                                                 \
  }

Iterator(char);

int main() {
  int length = 11;

  charIterator iter = create_chariterator_from_array(a, length);

  while (can_iterate_char(iter)) {
    int val = next_iter_char(&iter);
    printf("%c", val);
  }

  return 0;
}