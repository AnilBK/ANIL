#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String
import Dictionary
import List

struct Parser{List tokens};

namespace Parser
function __init__()
    print("Parser Constructor.\n")
endfunction

function has_tokens_remaining() -> bool:
    // return len(self.tokens) > 0
    return this.tokens.len() > 0
endfunction

function current_token() -> Node:
    // return self.tokens[0]
    return this.tokens[0]
endfunction

function next_token():
    // _ = self.tokens.pop(0)
    let node = this.tokens.pop(0)
endfunction

function get_token() -> Node:
    // return self.tokens.pop(0)
    return this.tokens.pop(0)
endfunction

function check_token(token:int) -> bool:
    // return self.current_token() == token
    // TODO : this.current_token() returns Node, so we cant make direct comparision with token. 
    return false
endfunction
endnamespace
///*///


int main() {

// clang-format off

  ///*///

  let parser = Parser{};

  // TODO : Parser should construct List & free it as well.

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}