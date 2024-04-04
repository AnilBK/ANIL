#include <stdbool.h>
#include <stdio.h>

#define Option(type)                                                           \
  struct {                                                                     \
    bool is_present;                                                           \
    type value;                                                                \
  }

typedef Option(int) Optional_int;

int main() {

  Optional_int value;
  value.value = 10;

  return 0;
}