///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct Test {
  char dummy;
};

struct Optional_int {
  bool _has_value;
  int _value;
};

// template Optional<int> {
void Optional_int__init__(struct Optional_int *this) {
  this->_has_value = false;
}

bool Optional_inthas_value(struct Optional_int *this) {
  return this->_has_value;
}

int Optional_intget_value(struct Optional_int *this) { return this->_value; }

void Optional_int_set_value(struct Optional_int *this, int p_value) {
  this->_value = p_value;
}

void Optional_intset_value(struct Optional_int *this, int p_value) {
  this->_has_value = true;
  Optional_int_set_value(this, p_value);
}

// template Optional<int> }

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

int main() {

  ///*///  main()

  struct Test test;

  struct Optional_int optional_int = Testreturn_val(&test);

  if (Optional_inthas_value(&optional_int)) {
    int val = Optional_intget_value(&optional_int);
    printf("Value is %d", val);
  }

  ///*///

  return 0;
}