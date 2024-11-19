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
import Lexer

struct Parser{List tokens};

namespace Parser

function __init__(line : String)
  let lexer = Lexer{};
  let tokens = lexer.get_tokens(line)

  this.tokens.__init__()
  this.tokens = tokens
endfunction

function __del__()
    // TODO : This should be performed automatically.
    this.tokens.__del__()
endfunction

function has_tokens_remaining() -> bool:
    return this.tokens.len() > 0
endfunction

function current_token() -> ListObject:
    return this.tokens[0]
endfunction

function next_token()
    let node = this.tokens.pop(0)
endfunction

function get_token() -> ListObject:
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

  let Line = "print("
  Line += "\"Hello World!\");"

  let parser = Parser{Line};

  let GeneratedLines = []
  GeneratedLines.append("#include<stdio.h>")
  GeneratedLines.append("int main(){")

  if parser.check_token("print"){
    parser.next_token()

    parser.consume_token(Token["LEFT_ROUND_BRACKET"])
    let actual_str = parser.extract_string_literal()
    parser.consume_token(Token["RIGHT_ROUND_BRACKET"])

    let str_to_write = "printf("
    str_to_write += "\"" + actual_str + "\");"
    let cstr = str_to_write.c_str()
    GeneratedLines.append(cstr)
  }
  
  GeneratedLines.append("return 0;")
  GeneratedLines.append("}")

  GeneratedLines.print()

  let outputFile = File{"Parser_test_output1.c"};
  for line in GeneratedLines{
    if line.is_str(){
        outputFile.writeline(line.get_str())
    }
  }


  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}