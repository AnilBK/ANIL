

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Test {
  char dummy;
};

struct Optional_int {
  bool _has_value;
  int _value;
};

struct Optional_int Testreturn_val(struct Test *this);
struct Optional_int Testreturn_none(struct Test *this);

int main();
void Optional_int__init__(struct Optional_int *this);
bool Optional_inthas_value(struct Optional_int *this);
int Optional_intget_value(struct Optional_int *this);
void Optional_intset_value(struct Optional_int *this, int p_value);

struct Optional_int Testreturn_val(struct Test *this) {
  struct Optional_int opt;
  Optional_int__init__(&opt);
  Optional_intset_value(&opt, 5);
  return opt;
}

struct Optional_int Testreturn_none(struct Test *this) {
  struct Optional_int opt;
  Optional_int__init__(&opt);
  return opt;
}

void Optional_int__init__(struct Optional_int *this) {
  this->_has_value = false;
}

bool Optional_inthas_value(struct Optional_int *this) {
  return this->_has_value;
}

int Optional_intget_value(struct Optional_int *this) { return this->_value; }

void Optional_intset_value(struct Optional_int *this, int p_value) {
  this->_has_value = true;
  this->_value = p_value;
}

int main() {
  struct Test test;

  struct Optional_int optional_int = Testreturn_val(&test);

  if (Optional_inthas_value(&optional_int)) {
    int val = Optional_intget_value(&optional_int);
    printf("Value is %d", val);
  }

  struct Optional_int opt;
  Optional_int__init__(&opt);
  Optional_intset_value(&opt, 5);

  return 0;
}
