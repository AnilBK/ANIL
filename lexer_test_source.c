#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

void CustomPrint(int data) {
  // printf("This is a custom print wrapper. [%d]\n", data);

#define print_token(tk_string, tk_value)                                       \
  do {                                                                         \
    if (data == tk_value) {                                                    \
      char *tk = tk_string;                                                    \
      printf("Token: %s", tk);                                                 \
    }                                                                          \
  } while (0)

  print_token("=", 1);
  print_token("[", 2);
  print_token("]", 3);
  print_token(";", 4);
  print_token(",", 5);
  print_token("%", 6);
  print_token("{", 7);
  print_token("}", 8);
  print_token("struct", 9);
  print_token("match", 10);
  print_token("for", 11);
  print_token("if", 12);
  print_token("in", 13);
  print_token("Option", 14);
  print_token("<", 15);
  print_token(">", 16);
  print_token("enumerate", 17);
  print_token("+", 18);
  print_token("fn", 19);
  print_token("(", 20);
  print_token(")", 21);
  print_token(":", 22);
  print_token(".", 23);
  print_token("*", 24);
  print_token("-", 25);

#undef print_token
}

int main() {

// clang-format off

  ///*///
  import String
  import Vector
  import Dictionary
  import List

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

  let line_org = String{"  let arr = [ 1, 2, 3, 4 , 5 ]; } let"};
  let line = line_org.strip
  let length = line.len

  let token = String{""};
  
  let tokens = List{};
  
  let inside_string = False
  let escape_back_slash = False

  def add_token_raw(p_token):
    tokens.append_str p_token
  enddef  

  def add_int_token(p_token):
    tokens.append_int p_token
  enddef  

  def add_string_token(p_token):
    tokens.append_str p_token
  enddef  

  def add_token(p_token):
    if p_token in TOKEN_MAP{
      let token = TOKEN_MAP[p_token]
      add_int_token token
    }else{  
      add_string_token p_token
    }
  enddef

  line.printLn

  for Char in line{
    #print(f "{Char}");

    if escape_back_slash{
      if Char == "\""{
        token += Char
        escape_back_slash = False
      }else if Char == "\\"{
        token += "\\\\"
        escape_back_slash = False
      }else{
        token += Char
        escape_back_slash = False
      }
    }else if Char == "\""{
      if inside_string{
        # End of string.
        inside_string = False

        # Single character tokens like = are tokenized by add_token(),
        # so we use the following method.
        # "=" the inner equals to shouldn't be tokenized.

        let tk1 = token.c_str
        add_token_raw tk1

        token.clear

        if Char in character_tokens{
          let tk2 = TOKEN_MAP[Char]
          tokens.append_int tk2            
        }
      }else{
        # Start of string.
        inside_string = True
        if Char in character_tokens{
          let tk3 = TOKEN_MAP[Char]
          tokens.append_int tk3            
        }
      }
    } else if inside_string{
        if Char == "\\"{
          escape_back_slash = True
          continue;
        }

        token += Char
    } else if Char == " "{
        if token == ""{
          continue;
        }

        let tk4 = token.c_str
        add_token tk4
        token.clear
    }else{
      if Char in character_tokens{
        if token != ""{
          let tk5 = token.c_str
          add_token tk5
        }
        let int_tk = TOKEN_MAP[Char]
        add_int_token int_tk
        token.clear
        continue;
      }

      let is_single_character = token.is_of_length 1

      if is_single_character{
      let tk6 = token[0]
      # 'character_tokens' expect a char but token is char*, so do this hack.

      if tk6 in character_tokens{
        let int_tk = TOKEN_MAP[tk6]
        add_int_token int_tk
        token.clear
        continue;
      }
      }

      token += Char
    }

    
  }    

  # Process the last token.
  if token != ""{
    let tk = token.c_str
    add_token tk
  }

  @apply_hook("custom_integer_printer", CustomPrint)
  tokens.print
  

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}