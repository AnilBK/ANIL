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
    return this.tokens.len() > 0
endfunction

function current_token() -> Node:
    return this.tokens[0]
endfunction

function next_token():
    let node = this.tokens.pop(0)
endfunction

function get_token() -> Node:
    return this.tokens.pop(0)
endfunction

c_function check_token(token : int) -> bool:
    // return self.current_token() == token
    // TODO : this.current_token() returns Node, so we cant make direct comparision with token in CPL. 
    Node node = Parsercurrent_token(this);
    if(node.data_type == INT){
        return node.data.int_data == token;
    }else{
        return false;
    }
endc_function

function match_token(token : int) -> bool:
    if this.check_token(token){
        return true
    }else{
        print("Expected token {token}.")
    }
endfunction

function consume_token(p_token : int) -> bool:
    this.match_token(p_token)
    this.next_token()
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