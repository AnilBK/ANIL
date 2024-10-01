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
import File

constexpr Token = {"LET" : 0, "EQUALS" : 1, "LEFT_SQUARE_BRACKET" : 2, "RIGHT_SQUARE_BRACKET" : 3, "SEMICOLON" : 4, "COMMA" : 5, "PERCENT" : 6, "LEFT_CURLY" : 7, "RIGHT_CURLY" : 8, "STRUCT" : 9, "MATCH" : 10, "FOR" : 11, "IF" : 12, "IN" : 13, "OPTION" : 14, "SMALLER_THAN" : 15, "GREATER_THAN" : 16, "ENUMERATE" : 17, "QUOTE" : 18, "PLUS" : 19, "LEFT_ROUND_BRACKET" : 21, "RIGHT_ROUND_BRACKET" : 22, "COLON" : 23, "DOT" : 24, "ASTERISK" : 25, "MINUS" : 26, "DEF" : 27, "CFUNCTION" : 28, "ENDDEF" : 29, "ENDFN" : 30, "ELSE" : 31, "TRUE" : 32, "FALSE" : 33, "CONSTEXPR" : 34, "HASH" : 35, "INCLUDE" : 36, "AT" : 37, "APPLY_HOOK" : 38, "HOOK_BEGIN" : 39, "HOOK_END" : 40, "EXCLAMATION" : 41}

struct Parser{List tokens};

namespace Parser
function __init__()
  print("Parser Constructor.\n")
    
  this.tokens.__init__()
  this.tokens.append("print")
  this.tokens.append(Token["LEFT_ROUND_BRACKET"])
  this.tokens.append(Token["QUOTE"])
  this.tokens.append("Hello World")
  this.tokens.append(Token["QUOTE"])
  this.tokens.append(Token["RIGHT_ROUND_BRACKET"])
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
    if this.match_token(p_token){
    }
    this.next_token()
endfunction

function extract_string_literal() -> String:
    /*
    Extract the string defined inside the quotes "...".
    And, advances the parser to the next token.
    It expects the following Tokens: Token.QUOTE, Actual string, Token.QUOTE.
    */
    this.consume_token(Token["QUOTE"])

    let string_tk = this.get_token()

    this.match_token(Token["QUOTE"])
    this.next_token()

    let string = "";
    if string_tk.is_str(){
        let str = string_tk.get_str()
        // TODO: Reassign bug(reassign expressions not implemented). 
        // So do above workaround.
        // What we want is string = string_tk.get_str()
        string = str
    }else{
        print("Expected a string literal.")
        exit(EXIT_FAILURE);
    }

    return string
endfunction

endnamespace
///*///


int main() {

// clang-format off

  ///*/// main()

  let parser = Parser{};

  let LinesCache = []
  LinesCache.append("#include<stdio.h>")
  LinesCache.append("int main(){")

  if parser.check_token("print"){
    parser.next_token()

    parser.consume_token(Token["LEFT_ROUND_BRACKET"])
    let actual_str = parser.extract_string_literal()
    parser.consume_token(Token["RIGHT_ROUND_BRACKET"])

    let str_to_write = "printf(";
    str_to_write += "\"" + actual_str + "\");"
    let cstr = str_to_write.c_str()
    LinesCache.append(cstr)
  }
  
  LinesCache.append("return 0;")
  LinesCache.append("}")

  LinesCache.print()

  let outputFile = File{"Parser_test_output1.c"};
  for line in LinesCache{
    if line.is_str(){
        outputFile.writeline(line.get_str())
    }
  }


  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}