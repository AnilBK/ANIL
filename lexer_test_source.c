#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

// OPTIONAL_INIT_CODE //

// STRUCT_DEFINATIONS //

void CustomPrint(int data) {
  // printf("This is a custom print wrapper. [%d]\n", data);

  // The macro is in str, val pair because it is in this format in TOKEN_MAP
  // below.
  // clang-format off

#define token_case(str, val)                                                   \
  case val: msg = str; break                                                               \

  char *msg;

  switch (data) {
    token_case("LET", 0); 
    token_case("EQUALS", 1); 
    token_case("LEFT_SQUARE_BRACKET", 2); 
    token_case("RIGHT_SQUARE_BRACKET", 3); 
    token_case("SEMICOLON", 4); 
    token_case("COMMA", 5); 
    token_case("PERCENT", 6); 
    token_case("LEFT_CURLY", 7); 
    token_case("RIGHT_CURLY", 8); 
    token_case("STRUCT", 9); 
    token_case("MATCH", 10); 
    token_case("FOR", 11); 
    token_case("IF", 12); 
    token_case("IN", 13); 
    token_case("OPTION", 14); 
    token_case("SMALLER_THAN", 15); 
    token_case("GREATER_THAN", 16); 
    token_case("ENUMERATE", 17); 
    token_case("QUOTE", 18); 
    token_case("PLUS", 19); 
    token_case("LEFT_ROUND_BRACKET", 21); 
    token_case("RIGHT_ROUND_BRACKET", 22); 
    token_case("COLON", 23); 
    token_case("DOT", 24); 
    token_case("ASTERISK", 25); 
    token_case("MINUS", 26); 
    token_case("DEF", 27); 
    token_case("IMPL", 28); 
    token_case("ENDDEF", 29); 
    token_case("ENDFN", 30); 
    token_case("ELSE", 31); 
    token_case("TRUE", 32); 
    token_case("FALSE", 33); 
    token_case("CONSTEXPR", 34); 
    token_case("HASH", 35); 
    token_case("INCLUDE", 36); 
    token_case("AT", 37); 
    token_case("APPLY_HOOK", 38); 
    token_case("HOOK_BEGIN", 39); 
    token_case("HOOK_END", 40); 
    token_case("EXCLAMATION", 41); 
    default: {
      msg = "UNDEFINED: ";
      break;
    }
  }

  // clang-format on
  printf("Token : %s", msg);

#undef token_case
}

int main() {

// clang-format off

  ///*///
  import String
  import Vector
  import Dictionary
  import List

  constexpr Token = {"LET" : 0, "EQUALS" : 1, "LEFT_SQUARE_BRACKET" : 2, "RIGHT_SQUARE_BRACKET" : 3, "SEMICOLON" : 4, "COMMA" : 5, "PERCENT" : 6, "LEFT_CURLY" : 7, "RIGHT_CURLY" : 8, "STRUCT" : 9, "MATCH" : 10, "FOR" : 11, "IF" : 12, "IN" : 13, "OPTION" : 14, "SMALLER_THAN" : 15, "GREATER_THAN" : 16, "ENUMERATE" : 17, "QUOTE" : 18, "PLUS" : 19, "LEFT_ROUND_BRACKET" : 21, "RIGHT_ROUND_BRACKET" : 22, "COLON" : 23, "DOT" : 24, "ASTERISK" : 25, "MINUS" : 26, "DEF" : 27, "IMPL" : 28, "ENDDEF" : 29, "ENDFN" : 30, "ELSE" : 31, "TRUE" : 32, "FALSE" : 33, "CONSTEXPR" : 34, "HASH" : 35, "INCLUDE" : 36, "AT" : 37, "APPLY_HOOK" : 38, "HOOK_BEGIN" : 39, "HOOK_END" : 40, "EXCLAMATION" : 41}

  let TOKEN_MAP = Dictionary{};

  // TOKEN_MAP.add_key_value "let" 0
  TOKEN_MAP["let"] = Token["LET"]
  TOKEN_MAP["="] = Token["EQUALS"]
  TOKEN_MAP["["] = Token["LEFT_SQUARE_BRACKET"]
  TOKEN_MAP["]"] = Token["RIGHT_SQUARE_BRACKET"]
  TOKEN_MAP[";"] = Token["SEMICOLON"]
  TOKEN_MAP[","] = Token["COMMA"]
  TOKEN_MAP["%"] = Token["PERCENT"]
  TOKEN_MAP["{"] = Token["LEFT_CURLY"]
  TOKEN_MAP["}"] = Token["RIGHT_CURLY"]
  TOKEN_MAP["struct"] = Token["STRUCT"]
  TOKEN_MAP["match"] = Token["MATCH"]
  TOKEN_MAP["for"] = Token["FOR"]
  TOKEN_MAP["if"] = Token["IF"]
  TOKEN_MAP["in"] = Token["IN"]
  TOKEN_MAP["Option"] = Token["OPTION"]
  TOKEN_MAP["<"] = Token["SMALLER_THAN"]
  TOKEN_MAP[">"] = Token["GREATER_THAN"]
  TOKEN_MAP["enumerate"] = Token["ENUMERATE"]
  TOKEN_MAP["+"] = Token["QUOTE"]
  TOKEN_MAP["fn"] = Token["PLUS"]
  TOKEN_MAP["("] = Token["LEFT_ROUND_BRACKET"]
  TOKEN_MAP[")"] = Token["RIGHT_ROUND_BRACKET"]
  TOKEN_MAP[":"] = Token["COLON"]
  TOKEN_MAP["."] = Token["DOT"]
  TOKEN_MAP["*"] = Token["ASTERISK"]
  TOKEN_MAP["-"] = Token["MINUS"]
  TOKEN_MAP["def"] = Token["DEF"]
  TOKEN_MAP["impl"] = Token["IMPL"]
  TOKEN_MAP["enddef"] = Token["ENDDEF"]
  TOKEN_MAP["endfunc"] = Token["ENDFN"]
  TOKEN_MAP["else"] = Token["ELSE"]
  TOKEN_MAP["True"] = Token["TRUE"]
  TOKEN_MAP["False"] = Token["FALSE"]
  TOKEN_MAP["constexpr"] = Token["CONSTEXPR"]
  TOKEN_MAP["#"] = Token["HASH"]
  TOKEN_MAP["include"] = Token["INCLUDE"]
  TOKEN_MAP["@"] = Token["AT"]
  TOKEN_MAP["apply_hook"] = Token["APPLY_HOOK"]
  TOKEN_MAP["hook_begin"] = Token["HOOK_BEGIN"]
  TOKEN_MAP["hook_end"] = Token["HOOK_END"]
  TOKEN_MAP["!"] = Token["EXCLAMATION"]

  let character_tokens = Vector<char>{1};
  character_tokens.pushn "=" "[" "]" ";" "," "{" "}" "<" ">" "+" "(" ")" ":" "." "*" "-" "#" "@" "!"

  let line_org = String{"  let arr = [ 1, 2, 3, 4 , 5 ]; } let"};
  let line = line_org.strip()
  let length = line.len()

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

        let tk1 = token.c_str()
        add_token_raw tk1

        token = ""

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

        let tk4 = token.c_str()
        add_token tk4
        token = ""
    }else{
      if Char in character_tokens{
        if token != ""{
          let tk5 = token.c_str()
          add_token tk5
        }
        let int_tk = TOKEN_MAP[Char]
        add_int_token int_tk
        token = ""
        continue;
      }

      let is_single_character = token.is_of_length(1)

      if is_single_character{
      let tk6 = token[0]
      # 'character_tokens' expect a char but token is char*, so do this hack.

      if tk6 in character_tokens{
        let int_tk = TOKEN_MAP[tk6]
        add_int_token int_tk
        token = ""
        continue;
      }
      }

      token += Char
    }

    
  }    

  # Process the last token.
  if token != ""{
    let tk = token.c_str()
    add_token tk
  }

  @apply_hook("custom_integer_printer", CustomPrint)
  tokens.print
  

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}