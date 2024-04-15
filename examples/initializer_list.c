#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///
  import Vector

  let values = Vector<int>{8};
  values.push 1
  values.push 2
  values.push 3
  values.push 4
  values.push 5
  values.push 6
  values.push 7
  values.push 8

  def push(X...):
    values.allocate_more X.size
  	forall x: values.push_unchecked x 
  enddef  

  push 10 20 30 40 50 60 70 80

  // DESTRUCTOR_CODE //
  ///*///
                    

  return 0;
}