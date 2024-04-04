#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///
  import String
  import Vector
  import Dictionary
  import List

  let line_org = String{"  let arr = [ 1, 2, 3, 4 , 5 ]; } let"};
  let line = line_org.strip
  let length = line.len

  let token = String{""};
  
  let tokens = List{};

  let inside_string = False
  let escape_back_slash = False

  for Char in line{
    print(f "{Char}");

    if escape_back_slash{
      if Char == "\""{
        token += Char
        escape_back_slash = False
      }
    }else if Char == "\""{
      if inside_string{
        # End of string.
        inside_string = False

        
      }else{
        inside_string = False
      }
    }

    
  }    

  
  # Process the last token.

  let TOKEN_MAP = Dictionary{};
  TOKEN_MAP.add_key_value "let" 0

  TOKEN_MAP["="] = 1
  TOKEN_MAP["["] = 2
  TOKEN_MAP["]"] = 3
  TOKEN_MAP[";"] = 4
  TOKEN_MAP[","] = 5
  TOKEN_MAP["%"] = 6
  TOKEN_MAP["{"] = 7
  TOKEN_MAP["}"] = 8
  TOKEN_MAP["struct"] = 9
  TOKEN_MAP["match"] = 10 
  TOKEN_MAP["for"] = 11
  TOKEN_MAP["if"] = 12
  TOKEN_MAP["in"] = 13
  TOKEN_MAP["Option"] = 14
  TOKEN_MAP["<"] = 15
  TOKEN_MAP[">"] = 16
  TOKEN_MAP["enumerate"] = 17
  TOKEN_MAP["+"] = 18
  TOKEN_MAP["fn"] = 19
  TOKEN_MAP["("] = 20
  TOKEN_MAP[")"] = 21
  TOKEN_MAP[":"] = 22
  TOKEN_MAP["."] = 23
  TOKEN_MAP["*"] = 24
  TOKEN_MAP["-"] = 25
  
  TOKEN_MAP.print

  if "enumerate" in TOKEN_MAP{
    print(f "Option is in the Dictionary.\n");
    let token = TOKEN_MAP["Option"]
    print(f "Value of Option token is {token}. \n");
  }else{
    print(f "enumerate is not in the Dictionary.\n");
  }

  let character_tokens = Vector<char>{20};
  character_tokens.push "="
  character_tokens.push "["
  character_tokens.push "]"
  character_tokens.push ";"
  character_tokens.push ","
  character_tokens.push "{"
  character_tokens.push "}"
  character_tokens.push "<"
  character_tokens.push ">"
  character_tokens.push "+"
  character_tokens.push "("
  character_tokens.push ")"
  character_tokens.push ":"
  character_tokens.push "."
  character_tokens.push "*"
  character_tokens.push "-"
  character_tokens.print

  for vec in character_tokens{
    print(f "{vec} \n");
  }

  let let_token = String{"Let"};

  def add_token_raw(p_token):
    tokens2.push p_token
  enddef  

  def add_token(p_token):
    if p_token in TOKEN_MAP{
      let token = TOKEN_MAP[p_token]
      add_token_raw token
    }else{  
      add_token_raw p_token
    }
  enddef


  let tokens2 = Vector<char>{20};
  tokens2.push "="
  tokens2.push "["
  tokens2.push "]"
  tokens2.push ";"
  tokens2.push ","
  tokens2.push "{"
  tokens2.push "}"

  add_token_raw "="

  add_token "="


  let test_list = List{};
  test_list.append_str "Hello"
  test_list.append_str "World"
  test_list.append_str "Puppy"

  test_list.append_int 10
  test_list.append_int 20
  test_list.append_int 30
  test_list.append_int 40
  test_list.append_int 50

  test_list.print


  def add_int_token(p_token):
    test_token_list.append_int p_token
  enddef  

  def add_string_token(p_token):
    test_token_list.append_str p_token
  enddef  

  def add_test_token(p_token):
    if p_token in TOKEN_MAP{
      let token = TOKEN_MAP[p_token]
      add_int_token token
    }else{  
      add_string_token p_token
    }
  enddef

  let test_token_list = List{};

  add_test_token "10"
  add_test_token ":"
  add_test_token "in"
  add_test_token "20"

  test_token_list.print

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}