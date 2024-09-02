// clang-format off
///*///
///////////////////////////////////////////
#include<time.h>
#include<stdlib.h>

struct Random{char dummy};

namespace Random

c_function __init__()
  srand(time(0));
endc_function  

c_function randrange(upper_limit : int) -> int:
  return rand() % upper_limit;
endc_function  

endnamespace
///*///

