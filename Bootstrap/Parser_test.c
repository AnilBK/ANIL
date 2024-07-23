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
  constexpr Token = {"LET" : 0, "EQUALS" : 1, "LEFT_SQUARE_BRACKET" : 2, "RIGHT_SQUARE_BRACKET" : 3, "SEMICOLON" : 4, "COMMA" : 5, "PERCENT" : 6, "LEFT_CURLY" : 7, "RIGHT_CURLY" : 8, "STRUCT" : 9, "MATCH" : 10, "FOR" : 11, "IF" : 12, "IN" : 13, "OPTION" : 14, "SMALLER_THAN" : 15, "GREATER_THAN" : 16, "ENUMERATE" : 17, "QUOTE" : 18, "PLUS" : 19, "LEFT_ROUND_BRACKET" : 21, "RIGHT_ROUND_BRACKET" : 22, "COLON" : 23, "DOT" : 24, "ASTERISK" : 25, "MINUS" : 26, "DEF" : 27, "CFUNCTION" : 28, "ENDDEF" : 29, "ENDFN" : 30, "ELSE" : 31, "TRUE" : 32, "FALSE" : 33, "CONSTEXPR" : 34, "HASH" : 35, "INCLUDE" : 36, "AT" : 37, "APPLY_HOOK" : 38, "HOOK_BEGIN" : 39, "HOOK_END" : 40, "EXCLAMATION" : 41}
    
  this.tokens.__init__()
  this.tokens.append(Token["LET"])
  this.tokens.append("arr")
  this.tokens.append(Token["EQUALS"])
  this.tokens.append(Token["LEFT_SQUARE_BRACKET"])
  this.tokens.append("1")
  this.tokens.append(Token["COMMA"])
  this.tokens.append("2")
  this.tokens.append(Token["COMMA"])
  this.tokens.append("3")
  this.tokens.append(Token["COMMA"])
  this.tokens.append("4")
  this.tokens.append(Token["COMMA"])
  this.tokens.append("5")
  this.tokens.append(Token["RIGHT_SQUARE_BRACKET"])
  this.tokens.append(Token["SEMICOLON"])
  this.tokens.append(Token["RIGHT_CURLY"])
  this.tokens.append(Token["LET"])
endfunction

function __del__()
    // TODO : This should be performed automatically.
    this.tokens.__del__()
endfunction

function has_tokens_remaining() -> bool:
    return this.tokens.len() > 0
endfunction

function current_token() -> CPLObject:
    return this.tokens[0]
endfunction

function next_token():
    let node = this.tokens.pop(0)
endfunction

function get_token() -> CPLObject:
    return this.tokens.pop(0)
endfunction

function check_token<>(token : int) -> bool:
    let node = this.current_token()
    return node == token
endfunction

function check_token<>(token : str) -> bool:
    let node = this.current_token()
    return node == token
endfunction

function match_token(token : int) -> bool:
    if this.check_token(token){
        return true
    }else{
        print("Expected token {token}.")
        exit(EXIT_FAILURE);
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

  if parser.check_token(0){
    print("Found let.\n")
    parser.next_token()
    if parser.check_token("arr"){
        print("Found arr.\n")
    }
  }

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}