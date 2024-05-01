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
  import Dictionary
  import List

  constexpr Token = {"LET" : 0, "EQUALS" : 1, "LEFT_SQUARE_BRACKET" : 2, "RIGHT_SQUARE_BRACKET" : 3, "SEMICOLON" : 4, "COMMA" : 5, "PERCENT" : 6, "LEFT_CURLY" : 7, "RIGHT_CURLY" : 8, "STRUCT" : 9, "MATCH" : 10, "FOR" : 11, "IF" : 12, "IN" : 13, "OPTION" : 14, "SMALLER_THAN" : 15, "GREATER_THAN" : 16, "ENUMERATE" : 17, "QUOTE" : 18, "PLUS" : 19, "LEFT_ROUND_BRACKET" : 21, "RIGHT_ROUND_BRACKET" : 22, "COLON" : 23, "DOT" : 24, "ASTERISK" : 25, "MINUS" : 26, "DEF" : 27, "IMPL" : 28, "ENDDEF" : 29, "ENDFN" : 30, "ELSE" : 31, "TRUE" : 32, "FALSE" : 33, "CONSTEXPR" : 34, "HASH" : 35, "INCLUDE" : 36, "AT" : 37, "APPLY_HOOK" : 38, "HOOK_BEGIN" : 39, "HOOK_END" : 40, "EXCLAMATION" : 41}

  let CHARACTER_TOKENS = Dictionary{};
  CHARACTER_TOKENS["="] = Token["EQUALS"]
  CHARACTER_TOKENS["["] = Token["LEFT_SQUARE_BRACKET"]
  CHARACTER_TOKENS["]"] = Token["RIGHT_SQUARE_BRACKET"]
  CHARACTER_TOKENS[";"] = Token["SEMICOLON"]
  CHARACTER_TOKENS[","] = Token["COMMA"]
  CHARACTER_TOKENS["%"] = Token["PERCENT"]
  CHARACTER_TOKENS["{"] = Token["LEFT_CURLY"]
  CHARACTER_TOKENS["}"] = Token["RIGHT_CURLY"]
  CHARACTER_TOKENS["<"] = Token["SMALLER_THAN"]
  CHARACTER_TOKENS[">"] = Token["GREATER_THAN"]
  CHARACTER_TOKENS["\""] = Token["QUOTE"]
  CHARACTER_TOKENS["+"] = Token["PLUS"]
  CHARACTER_TOKENS["("] = Token["LEFT_ROUND_BRACKET"]
  CHARACTER_TOKENS[")"] = Token["RIGHT_ROUND_BRACKET"]
  CHARACTER_TOKENS[":"] = Token["COLON"]
  CHARACTER_TOKENS["."] = Token["DOT"]
  CHARACTER_TOKENS["*"] = Token["ASTERISK"]
  CHARACTER_TOKENS["-"] = Token["MINUS"]
  CHARACTER_TOKENS["#"] = Token["HASH"]
  CHARACTER_TOKENS["@"] = Token["AT"]
  CHARACTER_TOKENS["!"] = Token["EXCLAMATION"]

  let KEYWORD_TOKENS = Dictionary{};
  KEYWORD_TOKENS["let"] = Token["LET"]
  KEYWORD_TOKENS["struct"] = Token["STRUCT"]
  KEYWORD_TOKENS["match"] = Token["MATCH"]
  KEYWORD_TOKENS["for"] = Token["FOR"]
  KEYWORD_TOKENS["if"] = Token["IF"]
  KEYWORD_TOKENS["in"] = Token["IN"]
  KEYWORD_TOKENS["Option"] = Token["OPTION"]
  KEYWORD_TOKENS["enumerate"] = Token["ENUMERATE"]
  KEYWORD_TOKENS["def"] = Token["DEF"]
  KEYWORD_TOKENS["impl"] = Token["IMPL"]
  KEYWORD_TOKENS["enddef"] = Token["ENDDEF"]
  KEYWORD_TOKENS["endfunc"] = Token["ENDFN"]
  KEYWORD_TOKENS["else"] = Token["ELSE"]
  KEYWORD_TOKENS["True"] = Token["TRUE"]
  KEYWORD_TOKENS["False"] = Token["FALSE"]
  KEYWORD_TOKENS["constexpr"] = Token["CONSTEXPR"]
  KEYWORD_TOKENS["include"] = Token["INCLUDE"]
  KEYWORD_TOKENS["apply_hook"] = Token["APPLY_HOOK"]
  KEYWORD_TOKENS["hook_begin"] = Token["HOOK_BEGIN"]
  KEYWORD_TOKENS["hook_end"] = Token["HOOK_END"]

  let line_org = String{"  let arr = [ 1, 2, 3, 4 , 5 ]; } let"};
  let line = line_org.strip()
  let length = line.len()

  let token = String{""};
  
  let tokens = List{};
  
  let inside_string = False
  let escape_back_slash = False

  def add_token_raw(p_token):
    tokens.append_str(p_token)
  enddef  

  def add_int_token(p_token):
    tokens.append_int(p_token)
  enddef  

  def add_string_token(p_token):
    tokens.append_str(p_token)
  enddef  

  def add_token(p_token):
    if p_token in KEYWORD_TOKENS{
      let ktk = KEYWORD_TOKENS[p_token]
      add_int_token ktk
    }else if p_token in CHARACTER_TOKENS{
      let ctk = CHARACTER_TOKENS[p_token]
      add_int_token ctk
    }else{  
      add_token_raw p_token
    }
  enddef

  line.printLn()

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
        let tk = token.c_str()
        add_token_raw tk

        token = ""
      }else{
        # Start of string.
        inside_string = True
      }
      tokens.append_int(Token["QUOTE"])
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

        let tk = token.c_str()
        add_token tk
        token = ""
    }else{
      if Char in CHARACTER_TOKENS{
        if token != ""{
          let tk = token.c_str()
          add_token tk
        }
        let int_tk = CHARACTER_TOKENS[Char]
        add_int_token int_tk
        token = ""
        continue;
      }

      let token_str = token.c_str()
      if token_str in CHARACTER_TOKENS{
        let int_tk = CHARACTER_TOKENS[token_str]
        add_int_token int_tk
        token = ""
        continue;
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
  tokens.print()
  

  // DESTRUCTOR_CODE //
  ///*///
      // clang-format on

      return 0;
}