///*///

#include <ctype.h>
#include <string.h>

///*///

///*///

#include <string.h>

///////////////////////////////////////////

///*///

#include <string.h>

typedef struct {
  char *key_str;
} Key;

typedef struct {
  Key key;
  int value;
} KeyValuePair;

// We don't have arrays inside our custom struct datatype,so vvv.
typedef struct {
  KeyValuePair pair[30];
} KeyValuePairs;

///*///

///*///

#include <stdlib.h>

// Define a union for storing int or char*
typedef union {
  int int_data;
  char *str_data;
} Data;

enum DataType { INT, STRING };

// Define a struct for the node
typedef struct Node {
  Data data;
  enum DataType data_type;
  struct Node *next;
} Node;

Node *createIntNode(int p_int) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data.int_data = p_int;
  newNode->data_type = INT;
  newNode->next = NULL;
  return newNode;
}

Node *createStringNode(char *p_str) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->data.str_data = p_str;
  newNode->data_type = STRING;
  newNode->next = NULL;
  return newNode;
}

///*///

///*///

/*
FIXME: The order in which normal C code and our custom code is generated is
different. So this function will be emitted earlier than the actual List struct
causing forward declaration errors. Temporarily we replace the entire function
body at all the needed callsites.

void ListinsertEnd(struct List *this, Node *newNode) {
  if (this->head == NULL) {
    this->head = newNode;
    this->tail = newNode;
    return;
  }

  this->tail->next = newNode;
  this->tail = newNode;
}
*/

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct String {
  char *arr;
};

void String__init__(struct String *this, char *text) {
  this->arr = (char *)malloc((strlen(text) + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strcpy(this->arr, text);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, 1);
  this->arr[0] = '\0';
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) { free(this->arr); }

struct String Stringstrip(struct String *this) {
  //  char *str = "  Hello ";
  char *str = this->arr;

  char *begin = str;
  // Remove leading whitespaces
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces
  char *end = str + strlen(str) - 1;
  while (end > begin && isspace(*end)) {
    end--;
  }

  int new_length = end - begin + 1;

  struct String text;
  text.arr = (char *)malloc((new_length + 1) * sizeof(char));

  if (text.arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  // Copy the substring to the new variable
  strncpy(text.arr, begin, new_length);

  // Null-terminate the new string
  text.arr[new_length] = '\0';

  return text;
}

size_t Stringlen(struct String *this) { return strlen(this->arr); }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

bool Stringis_of_length(struct String *this, int p_len) {
  return strlen(this->arr) == p_len;
}

char *Stringc_str(struct String *this) { return this->arr; }

void String__add__(struct String *this, char *pstring) {
  size_t new_length = strlen(this->arr) + strlen(pstring) + 1;

  this->arr = (char *)realloc(this->arr, new_length);
  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
}

struct Dictionary {
  int added_values;
  KeyValuePairs pairs;
};

void Dictionary__init__(struct Dictionary *this) { this->added_values = 0; }

void Dictionaryadd_key_value(struct Dictionary *this, char *p_key_str,
                             int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

void Dictionaryprint(struct Dictionary *this) {
  printf("{\n");
  for (int i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    printf("\"%s\" : %d,\n", key, value);
  }
  printf("}\n");
}

bool Dictionary__contains__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return true;
    }
  }
  return false;
}

int Dictionaryget_value_from_key(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

int Dictionary__getitem__(struct Dictionary *this, char *p_key) {
  for (size_t i = 0; i < this->added_values; i++) {
    char *key = this->pairs.pair[i].key.key_str;
    int value = this->pairs.pair[i].value;
    if (strcmp(key, p_key) == 0) {
      return value;
    }
  }
  return 0;
}

void Dictionary__setitem__(struct Dictionary *this, char *p_key_str,
                           int p_value) {
  KeyValuePair pair;
  pair.key.key_str = p_key_str;
  pair.value = p_value;
  this->pairs.pair[this->added_values++] = pair;
}

struct List {
  Node *head;
  Node *tail;
};

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
}

void List__del__(struct List *this) {
  Node *current = this->head;
  while (current != NULL) {
    Node *temp = current;
    current = current->next;

    if (temp->data_type == STRING) {
      free(temp->data.str_data);
    }

    free(temp);
  }
}

void Listprint(struct List *this) {
  Node *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\" ", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d ", data);
      //@hook_end
    }
    printf(",");
    current = current->next;
  }
  printf("]\n");
}

typedef void (*custom_integer_printer)(int);
void Listprint_hooked_custom_integer_printer(
    struct List *this, custom_integer_printer p_custom_integer_printer) {
  Node *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\" ", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //
      p_custom_integer_printer(data);
    }
    printf(",");
    current = current->next;
  }
  printf("]\n");
}

void Listappend_int(struct List *this, int p_value) {
  Node *int_node = createIntNode(p_value);
  // ListinsertEnd(this, int_node);
  //  TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = int_node;
    this->tail = int_node;
    return;
  }

  this->tail->next = int_node;
  this->tail = int_node;
}

void Listappend_str(struct List *this, char *p_str) {
  Node *string_node = createStringNode(strdup(p_str));
  // ListinsertEnd(this, string_node);
  //  TODO : Move the below code to separate function 'ListinsertEnd'.
  if (this->head == NULL) {
    this->head = string_node;
    this->tail = string_node;
    return;
  }

  this->tail->next = string_node;
  this->tail = string_node;
}

struct Vector__char {
  char *arr;
  int size;
  int capacity;
};

// template <char> {
void Vector_char__init__(struct Vector__char *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (char *)malloc(capacity * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_char__del__(struct Vector__char *this) {
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_charpush(struct Vector__char *this, char value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_charallocate_more(struct Vector__char *this, int n) {
  this->capacity += n;
  this->arr = (char *)realloc(this->arr, this->capacity * sizeof(char));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_charpush_unchecked(struct Vector__char *this, char value) {
  this->arr[this->size++] = value;
}

void Vector_charprint(struct Vector__char *this) {
  // maybe print instanced vec name.
  // instanced name should be passed as fn parameter ig.
  // or add instance_name member silently to the struct itself.
  // printf("@INSTANCE_NAME@\n");
  /*

  void Vector_print(struct Vector* this, const char* p_instance_name){
  printf("%s\n", p_instance_name);
  }
  */

  printf("Dynamic Array (size = %zu, capacity = %zu) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    const char *type = "char";
    if (strcmp(type, "float") == 0) {
      printf("%f", this->arr[i]);
    } else if (strcmp(type, "int") == 0) {
      printf("%d", this->arr[i]);
    } else if (strcmp(type, "char") == 0) {
      printf("%c", this->arr[i]);
    }
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

bool Vector_char__contains__(struct Vector__char *this, char value) {
  // this returns bool.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t Vector_charlen(struct Vector__char *this) { return this->size; }

char Vector_char__getitem__(struct Vector__char *this, int index) {
  return *(this->arr + index);
}

// template <char> }

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

  ///*///

  struct Dictionary TOKEN_MAP;
  Dictionary__init__(&TOKEN_MAP);
  Dictionaryadd_key_value(&TOKEN_MAP, "let", 0);

  Dictionary__setitem__(&TOKEN_MAP, "=", 1);

  Dictionary__setitem__(&TOKEN_MAP, "[", 2);

  Dictionary__setitem__(&TOKEN_MAP, "]", 3);

  Dictionary__setitem__(&TOKEN_MAP, ";", 4);

  Dictionary__setitem__(&TOKEN_MAP, ",", 5);

  Dictionary__setitem__(&TOKEN_MAP, "%", 6);

  Dictionary__setitem__(&TOKEN_MAP, "{", 7);

  Dictionary__setitem__(&TOKEN_MAP, "}", 8);

  Dictionary__setitem__(&TOKEN_MAP, "struct", 9);

  Dictionary__setitem__(&TOKEN_MAP, "match", 10);

  Dictionary__setitem__(&TOKEN_MAP, "for", 11);

  Dictionary__setitem__(&TOKEN_MAP, "if", 12);

  Dictionary__setitem__(&TOKEN_MAP, "in", 13);

  Dictionary__setitem__(&TOKEN_MAP, "Option", 14);

  Dictionary__setitem__(&TOKEN_MAP, "<", 15);

  Dictionary__setitem__(&TOKEN_MAP, ">", 16);

  Dictionary__setitem__(&TOKEN_MAP, "enumerate", 17);

  Dictionary__setitem__(&TOKEN_MAP, "+", 18);

  Dictionary__setitem__(&TOKEN_MAP, "fn", 19);

  Dictionary__setitem__(&TOKEN_MAP, "(", 20);

  Dictionary__setitem__(&TOKEN_MAP, ")", 21);

  Dictionary__setitem__(&TOKEN_MAP, ":", 22);

  Dictionary__setitem__(&TOKEN_MAP, ".", 23);

  Dictionary__setitem__(&TOKEN_MAP, "*", 24);

  Dictionary__setitem__(&TOKEN_MAP, "-", 25);

  struct Vector__char character_tokens;
  Vector_char__init__(&character_tokens, 1);
  // Class Macro.
  Vector_charallocate_more(&character_tokens, 16);

  Vector_charpush_unchecked(&character_tokens, '=');

  Vector_charpush_unchecked(&character_tokens, '[');

  Vector_charpush_unchecked(&character_tokens, ']');

  Vector_charpush_unchecked(&character_tokens, ';');

  Vector_charpush_unchecked(&character_tokens, ',');

  Vector_charpush_unchecked(&character_tokens, '{');

  Vector_charpush_unchecked(&character_tokens, '}');

  Vector_charpush_unchecked(&character_tokens, '<');

  Vector_charpush_unchecked(&character_tokens, '>');

  Vector_charpush_unchecked(&character_tokens, '+');

  Vector_charpush_unchecked(&character_tokens, '(');

  Vector_charpush_unchecked(&character_tokens, ')');

  Vector_charpush_unchecked(&character_tokens, ':');

  Vector_charpush_unchecked(&character_tokens, '.');

  Vector_charpush_unchecked(&character_tokens, '*');

  Vector_charpush_unchecked(&character_tokens, '-');

  struct String line_org;
  String__init__(&line_org, "  let arr = [ 1, 2, 3, 4 , 5 ]; } let");
  struct String line = Stringstrip(&line_org);
  size_t length = Stringlen(&line);

  struct String token;
  String__init__(&token, "");

  struct List tokens;
  List__init__(&tokens);

  bool inside_string = false;
  bool escape_back_slash = false;

  StringprintLn(&line);

  size_t tmp_len_0 = Stringlen(&line);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char Char = String__getitem__(&line, i);
    // print(f "{Char}");

    if (escape_back_slash) {

      if (Char == '"') {

        char Char_promoted_0[2] = {Char, '\0'};

        String__add__(&token, Char_promoted_0);

        escape_back_slash = false;
      } else if (Char == '\\') {
        String__add__(&token, "\\\\");
        escape_back_slash = false;
      } else {

        char Char_promoted_1[2] = {Char, '\0'};

        String__add__(&token, Char_promoted_1);

        escape_back_slash = false;
      }
    } else if (Char == '"') {

      if (inside_string) {
        // End of string.
        inside_string = false;

        // Single character tokens like = are tokenized by add_token(),
        // so we use the following method.
        // "=" the inner equals to shouldn't be tokenized.

        char *tk1 = Stringc_str(&token);
        Listappend_str(&tokens, tk1);

        Stringclear(&token);

        if (Vector_char__contains__(&character_tokens, Char)) {
          char Char_promoted_2[2] = {Char, '\0'};
          int tk2 = Dictionary__getitem__(&TOKEN_MAP, Char_promoted_2);
          Listappend_int(&tokens, tk2);
        }
      } else {
        // Start of string.
        inside_string = true;

        if (Vector_char__contains__(&character_tokens, Char)) {
          char Char_promoted_3[2] = {Char, '\0'};
          int tk3 = Dictionary__getitem__(&TOKEN_MAP, Char_promoted_3);
          Listappend_int(&tokens, tk3);
        }
      }
    } else if (inside_string) {

      if (Char == '\\') {
        escape_back_slash = true;
        continue;
      }

      char Char_promoted_4[2] = {Char, '\0'};

      String__add__(&token, Char_promoted_4);

    } else if (Char == ' ') {

      if (String__eq__(&token, "")) {
        continue;
      }

      char *tk4 = Stringc_str(&token);

      if (Dictionary__contains__(&TOKEN_MAP, tk4)) {
        int token = Dictionary__getitem__(&TOKEN_MAP, tk4);
        Listappend_int(&tokens, token);

      } else {
        Listappend_str(&tokens, tk4);
      }
      Stringclear(&token);

    } else {

      if (Vector_char__contains__(&character_tokens, Char)) {

        if (!String__eq__(&token, "")) {
          char *tk5 = Stringc_str(&token);

          if (Dictionary__contains__(&TOKEN_MAP, tk5)) {
            int token = Dictionary__getitem__(&TOKEN_MAP, tk5);
            Listappend_int(&tokens, token);

          } else {
            Listappend_str(&tokens, tk5);
          }
        }
        char Char_promoted_5[2] = {Char, '\0'};
        int int_tk = Dictionary__getitem__(&TOKEN_MAP, Char_promoted_5);
        Listappend_int(&tokens, int_tk);

        Stringclear(&token);

        continue;
      }

      bool is_single_character = Stringis_of_length(&token, 1);

      if (is_single_character) {
        char tk6 = String__getitem__(&token, 0);
        // 'character_tokens' expect a char but token is char*, so do this hack.

        if (Vector_char__contains__(&character_tokens, tk6)) {
          char tk6_promoted_6[2] = {tk6, '\0'};
          int int_tk = Dictionary__getitem__(&TOKEN_MAP, tk6_promoted_6);
          Listappend_int(&tokens, int_tk);

          Stringclear(&token);

          continue;
        }
      }

      char Char_promoted_7[2] = {Char, '\0'};

      String__add__(&token, Char_promoted_7);
    }
  }

  // Process the last token.

  if (!String__eq__(&token, "")) {
    char *tk = Stringc_str(&token);

    if (Dictionary__contains__(&TOKEN_MAP, tk)) {
      int token = Dictionary__getitem__(&TOKEN_MAP, tk);
      Listappend_int(&tokens, token);

    } else {
      Listappend_str(&tokens, tk);
    }
  }

  Listprint_hooked_custom_integer_printer(&tokens, CustomPrint);

  List__del__(&tokens);
  String__del__(&token);
  String__del__(&line);
  String__del__(&line_org);
  Vector_char__del__(&character_tokens);
  ///*///

  return 0;
}