#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// clang-format off

// IMPORTS //

// STRUCT_DEFINATIONS //


///*///

struct Node{int m_data};

namespace Node
c_function __init__(p_data : int)
  this->m_data = p_data;
endc_function

c_function print()
    int data = this->m_data;
    
    @hook_begin("custom_integer_printer" "int" data)
        printf("%d \n", data);
    @hook_end
endc_function
endnamespace
///*///

void CustomIntPrint(int data) {
    printf("[CustomIntegerPrinter] Int = %d \n", data);
}    

int main() {


  ///*/// main()

  let node = Node{10};

  node.print()

  @apply_hook("custom_integer_printer", CustomIntPrint)
  node.print()

  // DESTRUCTOR_CODE //
  ///*///

  // clang-format on

  return 0;
}