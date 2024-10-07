#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()

  import Optional
  
  struct Test{char dummy};
  
  namespace Test
  function return_val() -> Optional<int>:
      let opt = Optional<int>{};
      opt.set_value(5)
      return opt
  endfunction
  
  function return_none() -> Optional<int>:
      let opt = Optional<int>{};
      return opt
  endfunction
  endnamespace

  let test = Test{};

  let optional_int = test.return_val()
  if optional_int.has_value(){
    let val = optional_int.get_value()
    print("Value is {val}")
  }

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}