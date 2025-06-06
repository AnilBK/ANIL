
#include <string.h>

///////////////////////////////////////////

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 101

unsigned int hash(char *str) {
  unsigned int hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % TABLE_SIZE;
}

#include <stdlib.h>
#include <string.h>

typedef struct Variant {
  enum { INT, STRING } data_type;

  union {
    int int_data;
    char *str_data;
  } data;
} Variant;

typedef struct Variant Variant;
typedef struct ListObject *ListObjectptr;

///////////////////////////////////////////
#include <stdlib.h>
#include <time.h>

// Dictionary of <int, list<String>>
// This is just for Symbol Table in preprocess_test as we dont have
// Dictionary<int, String> as of now.

// Ordered Dictionary of key(int) and value(list of string).

#include <stdlib.h>

#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
  bool is_constexpr;
};

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

struct Set {
  struct Vector_String arr;
};

struct ListObject {
  Variant data;
  struct ListObject *next;
};

struct List {
  struct ListObject *head;
  struct ListObject *tail;
  int size;
};

struct Random {
  char dummy;
};

struct int_str_list {
  int key;
  struct Vector_String value;
};

struct Vector_int_str_list {
  struct int_str_list *arr;
  int size;
  int capacity;
};

struct Dict_int_string {
  struct Vector_int_str_list pairs;
};

struct ErrorHandler {
  char dummy;
};

struct StructInstance {
  struct String struct_type;
  struct String struct_name;
  bool is_templated;
  struct String templated_data_type;
  int scope;
  bool should_be_freed;
  bool is_pointer_type;
};

struct Symbol {
  struct String name;
  struct String data_type;
};

struct NameSymbolPair {
  struct String name;
  struct Symbol symbol;
};

struct OrderedDictObject_Symbol {
  char *key_str;
  struct Symbol value;
  struct OrderedDictObject_Symbol *next;
};

struct OrderedDict_Symbol {
  struct OrderedDictObject_Symbol *nodes;
};

struct Optional_Symbol {
  bool _has_value;
  struct Symbol _value;
};

struct Scope {
  int scope_id;
  struct OrderedDict_Symbol symbols;
};

struct ScopeScopeIDPair {
  int scope_id;
  struct Scope scope;
};

struct Vector_ScopeScopeIDPair {
  struct ScopeScopeIDPair *arr;
  int size;
  int capacity;
};

struct Vector_int {
  int *arr;
  int size;
  int capacity;
};

struct SymbolTable {
  struct Vector_ScopeScopeIDPair scopes;
  struct Vector_int scope_stack;
};

struct Optional_String {
  bool _has_value;
  struct String _value;
};

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void Stringinit__STATIC__(struct String *this, char *text, int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length);
void String__init__OVDstructString(struct String *this, struct String text);
void Stringclear(struct String *this);
void String_allocate_more(struct String *this, int n);
void Stringprint(struct String *this);
void StringprintLn(struct String *this);
void String__del__(struct String *this);
bool Stringstartswith(struct String *this, char *prefix);
struct String Stringsubstr(struct String *this, int start, int length);
struct String Stringstrip(struct String *this);
struct Vector_String Stringsplit(struct String *this, char delimeter);
bool String__contains__(struct String *this, char *substring);
bool String__eq__(struct String *this, char *pstring);
void String__add__(struct String *this, char *pstring);
void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length);
void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring);
void String__reassign__OVDstr(struct String *this, char *pstring);
void Stringset_to_file_contents(struct String *this, char *pfilename);
struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename);

size_t Setlen(struct Set *this);
struct String Set__getitem__(struct Set *this, int index);
void Set__init__(struct Set *this, int count);
bool Set__contains__(struct Set *this, struct String value);
void Setadd(struct Set *this, struct String value);
void Setprint(struct Set *this);

void ListObject__init__OVDint(struct ListObject *this, int p_value);
void ListObject__init__OVDstr(struct ListObject *this, char *p_value);
bool ListObjectis_int(struct ListObject *this);
int ListObjectget_int(struct ListObject *this);
bool ListObjectis_str(struct ListObject *this);
char *ListObjectget_str(struct ListObject *this);
void ListObject_clear_str(struct ListObject *this);
void ListObject__del__(struct ListObject *this);
struct ListObject ListObject_duplicate(struct ListObject *this);
bool ListObject__eq__OVDint(struct ListObject *this, int p_value);
bool ListObject__eq__OVDstr(struct ListObject *this, char *p_value);

int Listlen(struct List *this);
void List__init__(struct List *this);
void List__del__(struct List *this);
struct ListObject List__getitem__(struct List *this, int index);
struct ListObject Listpop(struct List *this, int index);
bool List__contains__OVDint(struct List *this, int p_value);
bool List__contains__OVDstr(struct List *this, char *p_value);
void Listprint(struct List *this);
void List_insert_end(struct List *this, ListObjectptr new_node);
ListObjectptr Listcreate_int_node(struct List *this, int p_value);
ListObjectptr Listcreate_string_node(struct List *this, char *p_value);
void Listappend_int(struct List *this, int p_value);
void Listappend_str(struct List *this, char *p_value);
void ListappendOVDint(struct List *this, int p_value);
void ListappendOVDstr(struct List *this, char *p_value);
void ListappendOVDstructListObject(struct List *this,
                                   struct ListObject p_value);
void List__reassign__(struct List *this, struct List p_list);

void Random__init__(struct Random *this);
int Randomrandrange(struct Random *this, int upper_limit);

void int_str_list__init__(struct int_str_list *this, int p_key);

void Dict_int_string__init__(struct Dict_int_string *this);
bool Dict_int_string__contains__(struct Dict_int_string *this, int p_key);
void Dict_int_stringkey_exists_quit(struct Dict_int_string *this);
void Dict_int_stringadd_key(struct Dict_int_string *this, int p_key);

void ErrorHandlerRAISE_ERROR(struct ErrorHandler *this, char *p_error_msg);

struct String insert_string(struct String original_string, int p_index,
                            struct String string_to_insert);
struct String escape_quotes(struct String s);
struct String get_format_specifier(struct String p_type);
struct String get_mangled_fn_name(struct String p_struct_type,
                                  struct String p_fn_name);
struct String
get_templated_mangled_fn_name(struct String p_struct_type,
                              struct String p_fn_name,
                              struct String p_templated_data_type);
bool has_constructors(struct String p_struct_type);
void StructInstance__init__(struct StructInstance *this,
                            struct String p_struct_type,
                            struct String p_struct_name, bool p_is_templated,
                            struct String p_templated_data_type, int p_scope);
bool StructInstanceis_templated_instance(struct StructInstance *this);
bool StructInstanceshould_struct_be_freed(struct StructInstance *this);

void Symbol__init__(struct Symbol *this, struct String p_name,
                    struct String p_data_type);
struct String Symbolget_name(struct Symbol *this);
struct String Symbolget_data_type(struct Symbol *this);
void Symbol__reassign__(struct Symbol *this, struct Symbol p_symbol);

void NameSymbolPair__init__(struct NameSymbolPair *this, struct String p_name,
                            struct Symbol p_symbol);
struct String NameSymbolPairget_name(struct NameSymbolPair *this);
struct Symbol NameSymbolPairget_symbol(struct NameSymbolPair *this);

void Scope__init__(struct Scope *this, int p_scope_id);
void Scopedeclare_variable(struct Scope *this, struct String name,
                           struct String p_type);
struct Optional_Symbol Scopelookup_variable(struct Scope *this,
                                            struct String name);
struct String Scopedestructor_for_all_variables(struct Scope *this);

void ScopeScopeIDPair__init__(struct ScopeScopeIDPair *this, int p_scope_id);
int ScopeScopeIDPairget_scope_id(struct ScopeScopeIDPair *this);

void SymbolTable__init__(struct SymbolTable *this);
int SymbolTablecurrent_scope(struct SymbolTable *this);
struct Scope SymbolTableget_scope_by_id(struct SymbolTable *this, int id);
int SymbolTablenew_unique_scope_id(struct SymbolTable *this);
void SymbolTableenter_scope(struct SymbolTable *this);
void SymbolTableremove_scope_by_id(struct SymbolTable *this, int scope_id);
struct String SymbolTabledestructor_code_for_all_remaining_variables(
    struct SymbolTable *this);
void SymbolTableexit_scope(struct SymbolTable *this);
void SymbolTableprint_symbol_table(struct SymbolTable *this);
void SymbolTabledeclare_variable(struct SymbolTable *this, struct String name,
                                 struct String p_type);
struct Optional_Symbol SymbolTablelookup_variable(struct SymbolTable *this,
                                                  struct String name);

int get_current_scope();
void increment_scope();
void decrement_scope();
void REGISTER_VARIABLE(struct String p_var_name, struct String p_var_data_type);
struct Optional_String get_type_of_variable(struct String p_var_name);
bool is_variable_of_type(struct String p_var_name, struct String p_type);
bool is_variable(struct String p_var_name);
bool is_variable_char_type(struct String p_var_name);
bool is_variable_const_char_ptr(struct String p_var_name);
bool is_variable_str_type(struct String p_var_name);
bool is_variable_boolean_type(struct String p_var_name);
bool is_variable_int_type(struct String p_var_name);
bool is_variable_size_t_type(struct String p_var_name);
size_t Vector_Stringlen(struct Vector_String *this);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index);
void Vector_String_call_destructor_for_all_elements(struct Vector_String *this);
void Vector_String_reset(struct Vector_String *this);
void Vector_String__del__(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
void Vector_Stringvalidate_index(struct Vector_String *this, int index);
struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s);
void Vector_String_set(struct Vector_String *this, int index,
                       struct String value);
void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_String_grow_if_required(struct Vector_String *this);
void Vector_String_push(struct Vector_String *this, struct String value);
void Vector_Stringpush(struct Vector_String *this, struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_String_shift_left_from(struct Vector_String *this, int index);
void Vector_Stringremove_at(struct Vector_String *this, int index);
void Vector_String_clear(struct Vector_String *this);
void Vector_Stringclear(struct Vector_String *this);
bool Vector_String__contains__(struct Vector_String *this, struct String value);
void Vector_Stringprint(struct Vector_String *this);
size_t Vector_int_str_listlen(struct Vector_int_str_list *this);
void Vector_int_str_list__init__(struct Vector_int_str_list *this,
                                 int capacity);
void Vector_int_str_list_call_destructor_for_element(
    struct Vector_int_str_list *this, int index);
void Vector_int_str_list_call_destructor_for_all_elements(
    struct Vector_int_str_list *this);
void Vector_int_str_list_reset(struct Vector_int_str_list *this);
void Vector_int_str_list__del__(struct Vector_int_str_list *this);
struct int_str_list
Vector_int_str_list__getitem__(struct Vector_int_str_list *this, int index);
void Vector_int_str_list_set_ith_item(struct Vector_int_str_list *this,
                                      int index, struct int_str_list value);
void Vector_int_str_listpush_unchecked(struct Vector_int_str_list *this,
                                       struct int_str_list value);
void Vector_int_str_listvalidate_index(struct Vector_int_str_list *this,
                                       int index);
void Vector_int_str_list_set(struct Vector_int_str_list *this, int index,
                             struct int_str_list value);
void Vector_int_str_list__setitem__(struct Vector_int_str_list *this, int index,
                                    struct int_str_list value);
void Vector_int_str_listallocate_more(struct Vector_int_str_list *this, int n);
void Vector_int_str_list_grow_if_required(struct Vector_int_str_list *this);
void Vector_int_str_list_push(struct Vector_int_str_list *this,
                              struct int_str_list value);
void Vector_int_str_listpush(struct Vector_int_str_list *this,
                             struct int_str_list value);
struct int_str_list Vector_int_str_listpop(struct Vector_int_str_list *this);
void Vector_int_str_list_shift_left_from(struct Vector_int_str_list *this,
                                         int index);
void Vector_int_str_listremove_at(struct Vector_int_str_list *this, int index);
void Vector_int_str_list_clear(struct Vector_int_str_list *this);
void Vector_int_str_listclear(struct Vector_int_str_list *this);
void Vector_int_str_listprint(struct Vector_int_str_list *this);
bool Vector_int_str_list__contains__(struct Vector_int_str_list *this,
                                     struct int_str_list value);

void OrderedDict_Symbol__init__(struct OrderedDict_Symbol *this);
void OrderedDict_Symbol__del__(struct OrderedDict_Symbol *this);
struct Symbol OrderedDict_Symbol__getitem__(struct OrderedDict_Symbol *this,
                                            char *p_key);
void OrderedDict_Symbol__setitem__(struct OrderedDict_Symbol *this, char *p_key,
                                   struct Symbol p_value);
bool OrderedDict_Symbol__contains__(struct OrderedDict_Symbol *this,
                                    char *p_key);
struct Optional_Symbol OrderedDict_Symbolget(struct OrderedDict_Symbol *this,
                                             char *key);
void OrderedDict_Symbolpush(struct OrderedDict_Symbol *this, char *p_key,
                            struct Symbol p_value);

void Optional_Symbol__init__(struct Optional_Symbol *this);
bool Optional_Symbolhas_value(struct Optional_Symbol *this);
struct Symbol Optional_Symbolget_value(struct Optional_Symbol *this);
void Optional_Symbolset_value(struct Optional_Symbol *this,
                              struct Symbol p_value);

size_t Vector_ScopeScopeIDPairlen(struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPair__init__(struct Vector_ScopeScopeIDPair *this,
                                     int capacity);
void Vector_ScopeScopeIDPair_call_destructor_for_element(
    struct Vector_ScopeScopeIDPair *this, int index);
void Vector_ScopeScopeIDPair_call_destructor_for_all_elements(
    struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPair_reset(struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPair__del__(struct Vector_ScopeScopeIDPair *this);
struct ScopeScopeIDPair
Vector_ScopeScopeIDPair__getitem__(struct Vector_ScopeScopeIDPair *this,
                                   int index);
void Vector_ScopeScopeIDPair_set_ith_item(struct Vector_ScopeScopeIDPair *this,
                                          int index,
                                          struct ScopeScopeIDPair value);
void Vector_ScopeScopeIDPairpush_unchecked(struct Vector_ScopeScopeIDPair *this,
                                           struct ScopeScopeIDPair value);
void Vector_ScopeScopeIDPairvalidate_index(struct Vector_ScopeScopeIDPair *this,
                                           int index);
void Vector_ScopeScopeIDPair_set(struct Vector_ScopeScopeIDPair *this,
                                 int index, struct ScopeScopeIDPair value);
void Vector_ScopeScopeIDPair__setitem__(struct Vector_ScopeScopeIDPair *this,
                                        int index,
                                        struct ScopeScopeIDPair value);
void Vector_ScopeScopeIDPairallocate_more(struct Vector_ScopeScopeIDPair *this,
                                          int n);
void Vector_ScopeScopeIDPair_grow_if_required(
    struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPair_push(struct Vector_ScopeScopeIDPair *this,
                                  struct ScopeScopeIDPair value);
void Vector_ScopeScopeIDPairpush(struct Vector_ScopeScopeIDPair *this,
                                 struct ScopeScopeIDPair value);
struct ScopeScopeIDPair
Vector_ScopeScopeIDPairpop(struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPair_shift_left_from(
    struct Vector_ScopeScopeIDPair *this, int index);
void Vector_ScopeScopeIDPairremove_at(struct Vector_ScopeScopeIDPair *this,
                                      int index);
void Vector_ScopeScopeIDPair_clear(struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPairclear(struct Vector_ScopeScopeIDPair *this);
void Vector_ScopeScopeIDPairprint(struct Vector_ScopeScopeIDPair *this);
bool Vector_ScopeScopeIDPair__contains__(struct Vector_ScopeScopeIDPair *this,
                                         struct ScopeScopeIDPair value);

size_t Vector_intlen(struct Vector_int *this);
void Vector_int__init__(struct Vector_int *this, int capacity);
void Vector_int_call_destructor_for_element(struct Vector_int *this, int index);
void Vector_int_call_destructor_for_all_elements(struct Vector_int *this);
void Vector_int_reset(struct Vector_int *this);
void Vector_int__del__(struct Vector_int *this);
int Vector_int__getitem__(struct Vector_int *this, int index);
void Vector_int_set_ith_item(struct Vector_int *this, int index, int value);
void Vector_intpush_unchecked(struct Vector_int *this, int value);
void Vector_intvalidate_index(struct Vector_int *this, int index);
void Vector_int_set(struct Vector_int *this, int index, int value);
void Vector_int__setitem__(struct Vector_int *this, int index, int value);
void Vector_intallocate_more(struct Vector_int *this, int n);
void Vector_int_grow_if_required(struct Vector_int *this);
void Vector_int_push(struct Vector_int *this, int value);
void Vector_intpush(struct Vector_int *this, int value);
int Vector_intpop(struct Vector_int *this);
void Vector_int_shift_left_from(struct Vector_int *this, int index);
void Vector_intremove_at(struct Vector_int *this, int index);
void Vector_int_clear(struct Vector_int *this);
void Vector_intclear(struct Vector_int *this);
bool Vector_int__contains__(struct Vector_int *this, int value);
void Vector_intprint(struct Vector_int *this);
void Optional_String__init__(struct Optional_String *this);
bool Optional_Stringhas_value(struct Optional_String *this);
struct String Optional_Stringget_value(struct Optional_String *this);
void Optional_Stringset_value(struct Optional_String *this,
                              struct String p_value);

void int_str_list__del__(struct int_str_list *this) {
  Vector_String__del__(&this->value);
}

void Symbol__del__(struct Symbol *this) {
  String__del__(&this->data_type);
  String__del__(&this->name);
}

void Scope__del__(struct Scope *this) {
  OrderedDict_Symbol__del__(&this->symbols);
}

void ScopeScopeIDPair__del__(struct ScopeScopeIDPair *this) {
  Scope__del__(&this->scope);
}

void Optional_Symbol__del__(struct Optional_Symbol *this) {
  Symbol__del__(&this->_value);
}

void Optional_String__del__(struct Optional_String *this) {
  String__del__(&this->_value);
}

void SymbolTable__del__(struct SymbolTable *this) {
  Vector_int__del__(&this->scope_stack);
  Vector_ScopeScopeIDPair__del__(&this->scopes);
}

void Set__del__(struct Set *this) { Vector_String__del__(&this->arr); }

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

size_t Stringlength_of_charptr(struct String *this, char *p_string) {
  // This should be some kind of static method.
  return strlen(p_string);
}

void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length) {
  // p_text_length : Length of the string without the null terminator.
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strncpy(this->arr, text, p_text_length);
  this->arr[p_text_length] = '\0';

  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = false;
}

void Stringinit__STATIC__(struct String *this, char *text, int p_text_length) {
  // WARNING: Only the compiler should write a call to this function.
  // The compiler uses this initialization function to create a temporary String
  // object when a string literal is passed to a function that expects a String
  // object.
  this->arr = text;
  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = true;
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length) {
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstructString(struct String *this, struct String text) {
  size_t p_text_length = Stringlen(&text);
  String__init__from_charptr(this, Stringc_str(&text), p_text_length);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
}

void String_allocate_more(struct String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n + 1;

  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
  if (!new_arr) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  } else {
    this->arr = new_arr;
    this->capacity = new_capacity;
  }
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) {
  if (!this->is_constexpr) {
    free(this->arr);
  }
}

bool Stringstartswith(struct String *this, char *prefix) {
  return strncmp(this->arr, prefix, strlen(prefix)) == 0;
}

struct String Stringsubstr(struct String *this, int start, int length) {
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;
}

struct String Stringstrip(struct String *this) {
  char *begin = this->arr;
  char *end = begin + Stringlen(this) - 1;

  // Remove leading whitespaces.
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces.
  while (end > begin && isspace(*end)) {
    end--;
  }

  // Length of the substring between 'begin' and 'end' inclusive.
  int new_length = end - begin + 1;

  struct String text;
  String__init__from_charptr(&text, begin, new_length);
  return text;
}

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // NOTE : Because of this function, before import String, we require import
  // Vector.
  struct Vector_String split_result;
  Vector_String__init__(&split_result, 2);

  int index = 0;
  int segment_start = 0;

  size_t tmp_len_0 = Stringlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char character = String__getitem__(this, i);

    if (character == delimeter) {

      if (segment_start < index) {
        struct String segment =
            Stringsubstr(this, segment_start, index - segment_start);
        Vector_Stringpush(&split_result, segment);
        String__del__(&segment);
      }
      segment_start = index + 1;
    }
    index = index + 1;
  }

  if (segment_start < Stringlen(this)) {
    struct String remaining_segment =
        Stringsubstr(this, segment_start, Stringlen(this) - segment_start);
    Vector_Stringpush(&split_result, remaining_segment);
    String__del__(&remaining_segment);
  }

  return split_result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = this->length + strlen(pstring) + 1;

  if (new_length > this->capacity) {
    size_t new_capacity;
    if (this->capacity == 0) {
      new_capacity = new_length * 2;
    } else {
      new_capacity = this->capacity;
      while (new_capacity <= new_length) {
        new_capacity *= 2;
      }
    }
    this->arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
    this->capacity = new_capacity;
  }

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
  this->length = new_length;
}

void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length) {
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
}

void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring) {
  char *src = Stringc_str(&pstring);
  size_t p_text_length = Stringlen(&pstring);
  Stringreassign_internal(this, src, p_text_length);
}

void String__reassign__OVDstr(struct String *this, char *pstring) {
  size_t p_text_length = Stringlength_of_charptr(this, pstring);
  Stringreassign_internal(this, pstring, p_text_length);
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  // Read from the file & store the contents to this string.

  // TODO: Implement this function in ANIL itself, because the function below is
  // a mangled function name.
  Stringclear(this);

  FILE *ptr = fopen(pfilename, "r");
  if (ptr == NULL) {
    printf("File \"%s\" couldn't be opened.\n", pfilename);
    return;
  }

  char myString[256];
  bool has_data = false;

  while (fgets(myString, sizeof(myString), ptr)) {
    String__add__(this, myString);
    has_data = true;
  }

  fclose(ptr);

  if (!has_data) {
    // Double-clear just in case
    Stringclear(this);
  }
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

size_t Setlen(struct Set *this) {
  size_t return_value = Vector_Stringlen(&this->arr);
  return return_value;
}

struct String Set__getitem__(struct Set *this, int index) {
  struct String return_value = Vector_String__getitem__(&this->arr, index);
  return return_value;
}

void Set__init__(struct Set *this, int count) {
  Vector_String__init__(&this->arr, count);
}

bool Set__contains__(struct Set *this, struct String value) {
  bool return_value = Vector_String__contains__(&this->arr, value);
  return return_value;
}

void Setadd(struct Set *this, struct String value) {

  if (!Set__contains__(this, value)) {
    Vector_Stringpush(&this->arr, value);
  }
}

void Setprint(struct Set *this) {
  printf("[");
  size_t tmp_len_1 = Setlen(this);
  for (size_t i = 0; i < tmp_len_1; i++) {
    struct String str = Set__getitem__(this, i);
    Stringprint(&str);
    printf(",");
  }
  printf("]\n");
}

void ListObject__init__OVDint(struct ListObject *this, int p_value) {
  this->data.data.int_data = p_value;
  this->data.data_type = INT;
  this->next = NULL;
}

void ListObject__init__OVDstr(struct ListObject *this, char *p_value) {
  this->data.data.str_data = strdup(p_value);
  this->data.data_type = STRING;
  this->next = NULL;
}

bool ListObjectis_int(struct ListObject *this) {
  return this->data.data_type == INT;
}

int ListObjectget_int(struct ListObject *this) {
  return this->data.data.int_data;
}

bool ListObjectis_str(struct ListObject *this) {
  return this->data.data_type == STRING;
}

char *ListObjectget_str(struct ListObject *this) {
  return this->data.data.str_data;
}

void ListObject_clear_str(struct ListObject *this) {
  free(this->data.data.str_data);
}

void ListObject__del__(struct ListObject *this) {

  if (ListObjectis_str(this)) {
    ListObject_clear_str(this);
  }
}

struct ListObject ListObject_duplicate(struct ListObject *this) {
  // Perform a deep copy.
  struct ListObject copy = *this;
  if (this->data.data_type == STRING) {
    copy.data.data.str_data = strdup(this->data.data.str_data);
  }
  return copy;
}

bool ListObject__eq__OVDint(struct ListObject *this, int p_value) {

  if (ListObjectis_int(this)) {
    bool return_value = ListObjectget_int(this) == p_value;
    return return_value;
  } else {
    return false;
  }
}

bool ListObject__eq__OVDstr(struct ListObject *this, char *p_value) {

  if (ListObjectis_str(this)) {
    bool return_value = strcmp(p_value, ListObjectget_str(this)) == 0;
    return return_value;
  } else {
    return false;
  }
}

int Listlen(struct List *this) { return this->size; }

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

void List__del__(struct List *this) {
  struct ListObject *current = this->head;
  while (current != NULL) {
    struct ListObject *temp = current;
    current = current->next;

    ListObject__del__(temp);
    free(temp);
  }
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

struct ListObject List__getitem__(struct List *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  struct ListObject *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  // Duplicate contents of node and return it.
  // If we return a reference, then the calling function will call destructor,
  // which will free the str_data causing free() errors later.
  return ListObject_duplicate(current);
}

struct ListObject Listpop(struct List *this, int index) {
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    exit(EXIT_FAILURE);
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  struct ListObject *current = this->head;
  struct ListObject *previous = NULL;

  for (int i = 0; i < index; i++) {
    previous = current;
    current = current->next;
  }

  if (previous == NULL) {
    // Popping the head.
    this->head = current->next;
    if (this->head == NULL) {
      // The list is now empty.
      this->tail = NULL;
    }
  } else {
    previous->next = current->next;
    if (current->next == NULL) {
      // Popping the tail.
      this->tail = previous;
    }
  }

  this->size--;

  struct ListObject popped_node = *current;
  // Don't free current->data.str_data even though current data_type is String.
  // After copying the *pointer above, popped_node now owns
  // current->data.str_data. This avoids duplicating current->data.str_data into
  // popped_node.
  free(current);
  return popped_node;
}

bool List__contains__OVDint(struct List *this, int p_value) {
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == INT) {
      int data = current->data.data.int_data;

      if (data == p_value) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

bool List__contains__OVDstr(struct List *this, char *p_value) {
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      char *data = current->data.data.str_data;

      if (strcmp(data, p_value) == 0) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

void Listprint(struct List *this) {
  struct ListObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      printf("\"%s\"", current->data.data.str_data);
    } else {
      int data = current->data.data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d", data);
      //@hook_end
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

typedef void (*custom_integer_printer)(int);
void Listprint_hooked_custom_integer_printer(
    struct List *this, custom_integer_printer p_custom_integer_printer) {
  struct ListObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      printf("\"%s\"", current->data.data.str_data);
    } else {
      int data = current->data.data.int_data;

      //
      p_custom_integer_printer(data);
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

void List_insert_end(struct List *this, ListObjectptr new_node) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
}

ListObjectptr Listcreate_int_node(struct List *this, int p_value) {
  struct ListObject *new_node =
      (struct ListObject *)malloc(sizeof(struct ListObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type int for value %d.",
           p_value);
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDint(new_node, p_value);
  return new_node;
}

ListObjectptr Listcreate_string_node(struct List *this, char *p_value) {
  struct ListObject *new_node =
      (struct ListObject *)malloc(sizeof(struct ListObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDstr(new_node, p_value);
  return new_node;
}

void Listappend_int(struct List *this, int p_value) {
  ListObjectptr int_node = Listcreate_int_node(this, p_value);
  List_insert_end(this, int_node);
}

void Listappend_str(struct List *this, char *p_value) {
  ListObjectptr str_node = Listcreate_string_node(this, p_value);
  List_insert_end(this, str_node);
}

void ListappendOVDint(struct List *this, int p_value) {
  Listappend_int(this, p_value);
}

void ListappendOVDstr(struct List *this, char *p_value) {
  Listappend_str(this, p_value);
}

void ListappendOVDstructListObject(struct List *this,
                                   struct ListObject p_value) {

  if (ListObjectis_int(&p_value)) {
    Listappend_int(this, ListObjectget_int(&p_value));
  } else if (ListObjectis_str(&p_value)) {
    Listappend_str(this, ListObjectget_str(&p_value));
  }
}

void List__reassign__(struct List *this, struct List p_list) {
  int tmp_len_2 = Listlen(&p_list);
  for (size_t i = 0; i < tmp_len_2; i++) {
    struct ListObject item = List__getitem__(&p_list, i);
    ListappendOVDstructListObject(this, item);
    ListObject__del__(&item);
  }
}

void Random__init__(struct Random *this) { srand(time(0)); }

int Randomrandrange(struct Random *this, int upper_limit) {
  return rand() % upper_limit;
}

void int_str_list__init__(struct int_str_list *this, int p_key) {
  this->key = p_key;
  Vector_String__init__(&this->value, 5);
}

void Dict_int_string__init__(struct Dict_int_string *this) {
  Vector_int_str_list__init__(&this->pairs, 5);
}

bool Dict_int_string__contains__(struct Dict_int_string *this, int p_key) {
  bool found = false;
  size_t tmp_len_3 = Vector_int_str_listlen(&this->pairs);
  for (size_t i = 0; i < tmp_len_3; i++) {
    struct int_str_list pair = Vector_int_str_list__getitem__(&this->pairs, i);

    if (pair.key == p_key) {
      found = true;
      // no break because the pair should be destructed at the end of the scope.
      // break doesn't do that as of now.
    }
    int_str_list__del__(&pair);
  }
  return found;
}

void Dict_int_stringkey_exists_quit(struct Dict_int_string *this) {
  fprintf(stderr, "Pop from empty Vector.\n");
  exit(EXIT_FAILURE);
}

void Dict_int_stringadd_key(struct Dict_int_string *this, int p_key) {

  if (Dict_int_string__contains__(this, p_key)) {
    Dict_int_stringkey_exists_quit(this);
  } else {
    struct int_str_list s;
    int_str_list__init__(&s, p_key);
    Vector_int_str_listpush(&this->pairs, s);
    int_str_list__del__(&s);
  }
}

void ErrorHandlerRAISE_ERROR(struct ErrorHandler *this, char *p_error_msg) {
  fprintf(stderr, p_error_msg);
  exit(EXIT_FAILURE);
}

void StructInstance__init__(struct StructInstance *this,
                            struct String p_struct_type,
                            struct String p_struct_name, bool p_is_templated,
                            struct String p_templated_data_type, int p_scope) {
  String__init__OVDstructString(&this->struct_type, p_struct_type);
  String__init__OVDstructString(&this->struct_name, p_struct_name);
  String__init__OVDstructString(&this->templated_data_type,
                                p_templated_data_type);

  this->is_templated = p_is_templated;

  this->scope = p_scope;

  this->should_be_freed = true;

  this->is_pointer_type = false;
}

bool StructInstanceis_templated_instance(struct StructInstance *this) {
  return this->is_templated;
}

bool StructInstanceshould_struct_be_freed(struct StructInstance *this) {
  return this->should_be_freed;
}

void Symbol__init__(struct Symbol *this, struct String p_name,
                    struct String p_data_type) {
  String__init__OVDstructString(&this->name, p_name);

  String__init__OVDstr(&this->data_type, "");
  // don't initialize data_type directly from p_data_type, so we can see,
  // reassign parsing is working as expected.
  String__reassign__OVDstructString(&this->data_type, p_data_type);
}

struct String Symbolget_name(struct Symbol *this) {
  return this->name;
}

struct String Symbolget_data_type(struct Symbol *this) {
  return this->data_type;
}

void Symbol__reassign__(struct Symbol *this, struct Symbol p_symbol) {
  String__reassign__OVDstructString(&this->name, Symbolget_name(&p_symbol));
  String__reassign__OVDstructString(&this->data_type,
                                    Symbolget_data_type(&p_symbol));
}

void NameSymbolPair__init__(struct NameSymbolPair *this, struct String p_name,
                            struct Symbol p_symbol) {
  String__init__OVDstructString(&this->name, p_name);
  Symbol__init__(&this->symbol, Symbolget_name(&p_symbol),
                 Symbolget_data_type(&p_symbol));
}

struct String NameSymbolPairget_name(struct NameSymbolPair *this) {
  struct String n;
  String__init__OVDstrint(&n, "", 0);
  String__reassign__OVDstructString(&n, this->name);
  return n;
}

struct Symbol NameSymbolPairget_symbol(struct NameSymbolPair *this) {
  return this->symbol;
}

void Scope__init__(struct Scope *this, int p_scope_id) {
  this->scope_id = p_scope_id;
  OrderedDict_Symbol__init__(&this->symbols);
}

void Scopedeclare_variable(struct Scope *this, struct String name,
                           struct String p_type) {

  if (OrderedDict_Symbol__contains__(&this->symbols, Stringc_str(&name))) {
    struct ErrorHandler e;
    ErrorHandlerRAISE_ERROR(&e, "Variable already declared.");
  }

  struct Symbol symbol;
  Symbol__init__(&symbol, name, p_type);
  OrderedDict_Symbolpush(&this->symbols, Stringc_str(&name), symbol);
  Symbol__del__(&symbol);
}

struct Optional_Symbol Scopelookup_variable(struct Scope *this,
                                            struct String name) {
  struct Optional_Symbol res =
      OrderedDict_Symbolget(&this->symbols, Stringc_str(&name));
  return res;
}

struct String Scopedestructor_for_all_variables(struct Scope *this) {
  struct String d;
  String__init__OVDstrint(&d, "", 0);
  return d;
}

void ScopeScopeIDPair__init__(struct ScopeScopeIDPair *this, int p_scope_id) {
  this->scope_id = p_scope_id;
  Scope__init__(&this->scope, p_scope_id);
}

int ScopeScopeIDPairget_scope_id(struct ScopeScopeIDPair *this) {
  return this->scope_id;
}

void SymbolTable__init__(struct SymbolTable *this) {
  Vector_ScopeScopeIDPair__init__(&this->scopes, 5);
  Vector_int__init__(&this->scope_stack, 5);
}

int SymbolTablecurrent_scope(struct SymbolTable *this) {
  int return_value = Vector_int__getitem__(&this->scope_stack, -1);
  return return_value;
}

struct Scope SymbolTableget_scope_by_id(struct SymbolTable *this, int id) {
  size_t tmp_len_4 = Vector_ScopeScopeIDPairlen(&this->scopes);
  for (size_t i = 0; i < tmp_len_4; i++) {
    struct ScopeScopeIDPair scope =
        Vector_ScopeScopeIDPair__getitem__(&this->scopes, i);

    if (scope.scope_id == id) {
      ScopeScopeIDPair__del__(&scope);
      return scope.scope;
    }
    ScopeScopeIDPair__del__(&scope);
  }

  struct ErrorHandler e;
  ErrorHandlerRAISE_ERROR(&e, "Didnt find scope of provided id.");
}

int SymbolTablenew_unique_scope_id(struct SymbolTable *this) {

  if (Vector_intlen(&this->scope_stack) == 0) {
    return 0;
  }

  int latest_scope = SymbolTablecurrent_scope(this);
  int new_scope = latest_scope + 1;

  while (Vector_int__contains__(&this->scope_stack, new_scope)) {
    new_scope = Randomrandrange(&random, 100000);
  }

  return new_scope;
}

void SymbolTableenter_scope(struct SymbolTable *this) {
  int new_scope_id = SymbolTablenew_unique_scope_id(this);
  Vector_intpush(&this->scope_stack, new_scope_id);

  struct ScopeScopeIDPair scope_pair;
  ScopeScopeIDPair__init__(&scope_pair, new_scope_id);
  Vector_ScopeScopeIDPairpush(&this->scopes, scope_pair);
  ScopeScopeIDPair__del__(&scope_pair);
}

void SymbolTableremove_scope_by_id(struct SymbolTable *this, int scope_id) {
  struct Vector_int id_to_remove;
  Vector_int__init__(&id_to_remove, 1);
  int index = 0;
  size_t tmp_len_5 = Vector_ScopeScopeIDPairlen(&this->scopes);
  for (size_t i = 0; i < tmp_len_5; i++) {
    struct ScopeScopeIDPair scope =
        Vector_ScopeScopeIDPair__getitem__(&this->scopes, i);
    int id = ScopeScopeIDPairget_scope_id(&scope);

    if (id == scope_id) {
      Vector_intpush(&id_to_remove, index);
    }
    index = index + 1;
    ScopeScopeIDPair__del__(&scope);
  }

  size_t tmp_len_6 = Vector_intlen(&id_to_remove);
  tmp_len_6 -= 1;
  for (size_t i = tmp_len_6; i != (size_t)-1; i += -1) {
    int idx = Vector_int__getitem__(&id_to_remove, i);
    Vector_ScopeScopeIDPairremove_at(&this->scopes, idx);
  }
  Vector_int__del__(&id_to_remove);
}

struct String SymbolTabledestructor_code_for_all_remaining_variables(
    struct SymbolTable *this) {
  struct String destructor_code;
  String__init__OVDstrint(&destructor_code, "", 0);

  while (Vector_intlen(&this->scope_stack) > 0) {
    int exiting_scope_id = Vector_intpop(&this->scope_stack);
    struct Scope scope = SymbolTableget_scope_by_id(this, exiting_scope_id);
    struct String des_code = Scopedestructor_for_all_variables(&scope);

    if (!(String__eq__(&des_code, ""))) {
      String__add__(&destructor_code, Stringc_str(&des_code));
    }
    SymbolTableremove_scope_by_id(this, exiting_scope_id);
    String__del__(&des_code);
    Scope__del__(&scope);
  }
  return destructor_code;
}

void SymbolTableexit_scope(struct SymbolTable *this) {

  if (Vector_intlen(&this->scope_stack) > 0) {
    int exiting_scope_id = Vector_intpop(&this->scope_stack);
    struct Scope scope = SymbolTableget_scope_by_id(this, exiting_scope_id);
    struct String destructor_code = Scopedestructor_for_all_variables(&scope);

    if (!(String__eq__(&destructor_code, ""))) {
      // LinesCache.append(destructor_code)
    }
    SymbolTableremove_scope_by_id(this, exiting_scope_id);
    String__del__(&destructor_code);
    Scope__del__(&scope);
  }

  // No more scopes remaining.
  // Create one so current_scope() wont return list index out of range, because
  // scope_stack is empty.

  if (Vector_intlen(&this->scope_stack) == 0) {
    SymbolTableenter_scope(this);
  }
}

void SymbolTableprint_symbol_table(struct SymbolTable *this) {
  printf("-------------------Symbol Table------------------");
  printf("Unimplemented");
  printf("-------------------------------------------------");
}

void SymbolTabledeclare_variable(struct SymbolTable *this, struct String name,
                                 struct String p_type) {
  int current_scope_id = SymbolTablecurrent_scope(this);
  struct Scope current_scope =
      SymbolTableget_scope_by_id(this, current_scope_id);

  if (OrderedDict_Symbol__contains__(&current_scope.symbols,
                                     Stringc_str(&name))) {
    SymbolTableprint_symbol_table(this);
    struct ErrorHandler e;
    ErrorHandlerRAISE_ERROR(&e, "Variable already declared.");
  }

  size_t tmp_len_7 = Vector_intlen(&this->scope_stack);
  for (size_t i = 0; i < tmp_len_7; i++) {
    int scope_id = Vector_int__getitem__(&this->scope_stack, i);
    struct Scope scope = SymbolTableget_scope_by_id(this, scope_id);

    if (OrderedDict_Symbol__contains__(&scope.symbols, Stringc_str(&name))) {
      SymbolTableprint_symbol_table(this);
      struct ErrorHandler e;
      ErrorHandlerRAISE_ERROR(&e,
                              "Variable already declared in previous scopes.");
    }
    Scope__del__(&scope);
  }

  Scopedeclare_variable(&current_scope, name, p_type);
  Scope__del__(&current_scope);
}

struct Optional_Symbol SymbolTablelookup_variable(struct SymbolTable *this,
                                                  struct String name) {
  size_t len = Vector_intlen(&this->scope_stack);
  for (size_t i = len - 1; i >= 0; i += -1) {
    int scope_id = Vector_int__getitem__(&this->scope_stack, i);
    struct Scope scope = SymbolTableget_scope_by_id(this, scope_id);
    struct Optional_Symbol variable = Scopelookup_variable(&scope, name);

    if (Optional_Symbolhas_value(&variable)) {
      Optional_Symbol__del__(&variable);
      Scope__del__(&scope);
      return variable;
    }
    Optional_Symbol__del__(&variable);
    Scope__del__(&scope);
  }

  struct Optional_Symbol none;
  Optional_Symbol__init__(&none);
  return none;
}

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

void Vector_String__init__(struct Vector_String *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct String *)malloc(capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  String__del__(&this->arr[index]);
}

void Vector_String_call_destructor_for_all_elements(
    struct Vector_String *this) {
  for (size_t i = 0; i < Vector_Stringlen(this); i++) {
    Vector_String_call_destructor_for_element(this, i);
  }
}

void Vector_String_reset(struct Vector_String *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_String__del__(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_reset(this);
}

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringvalidate_index(struct Vector_String *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s) {
  struct String string_copy;
  String__init__OVDstructString(&string_copy, s);
  return string_copy;
}

void Vector_String_set(struct Vector_String *this, int index,
                       struct String value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_set_ith_item(this, index, value);
}

void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_set(this, index, Vector_String_copy_string(this, value));
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct String *new_arr =
      (struct String *)realloc(this->arr, new_capacity * sizeof(struct String));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_String_grow_if_required(struct Vector_String *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_Stringallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_Stringallocate_more(this, 1);
    }
  }
}

void Vector_String_push(struct Vector_String *this, struct String value) {
  Vector_String_grow_if_required(this);
  Vector_Stringpush_unchecked(this, value);
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_push(this, Vector_String_copy_string(this, value));
}

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_String_shift_left_from(struct Vector_String *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_String_clear(struct Vector_String *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct String *)malloc(this->capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_Stringclear(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_clear(this);
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_11 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_11; i++) {
    struct String string = Vector_String__getitem__(this, i);

    if (Stringlen(&string) == Stringlen(&value)) {

      if (String__eq__(&string, Stringc_str(&value))) {
        return true;
      }
    }
  }
  return false;
}

void Vector_Stringprint(struct Vector_String *this) {
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"", this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

size_t Vector_int_str_listlen(struct Vector_int_str_list *this) {
  return this->size;
}

void Vector_int_str_list__init__(struct Vector_int_str_list *this,
                                 int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr =
      (struct int_str_list *)malloc(capacity * sizeof(struct int_str_list));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_int_str_list_call_destructor_for_element(
    struct Vector_int_str_list *this, int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  int_str_list__del__(&this->arr[index]);
}

void Vector_int_str_list_call_destructor_for_all_elements(
    struct Vector_int_str_list *this) {
  for (size_t i = 0; i < Vector_int_str_listlen(this); i++) {
    Vector_int_str_list_call_destructor_for_element(this, i);
  }
}

void Vector_int_str_list_reset(struct Vector_int_str_list *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_int_str_list__del__(struct Vector_int_str_list *this) {
  Vector_int_str_list_call_destructor_for_all_elements(this);
  Vector_int_str_list_reset(this);
}

struct int_str_list
Vector_int_str_list__getitem__(struct Vector_int_str_list *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_int_str_list_set_ith_item(struct Vector_int_str_list *this,
                                      int index, struct int_str_list value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_int_str_listpush_unchecked(struct Vector_int_str_list *this,
                                       struct int_str_list value) {
  this->arr[this->size++] = value;
}

void Vector_int_str_listvalidate_index(struct Vector_int_str_list *this,
                                       int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_int_str_list_set(struct Vector_int_str_list *this, int index,
                             struct int_str_list value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_int_str_listvalidate_index(this, index);
  Vector_int_str_list_call_destructor_for_element(this, index);
  Vector_int_str_list_set_ith_item(this, index, value);
}

void Vector_int_str_list__setitem__(struct Vector_int_str_list *this, int index,
                                    struct int_str_list value) {
  Vector_int_str_list_set(this, index, value);
}

void Vector_int_str_listallocate_more(struct Vector_int_str_list *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct int_str_list *new_arr = (struct int_str_list *)realloc(
      this->arr, new_capacity * sizeof(struct int_str_list));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_int_str_list_grow_if_required(struct Vector_int_str_list *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_int_str_listallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_int_str_listallocate_more(this, 1);
    }
  }
}

void Vector_int_str_list_push(struct Vector_int_str_list *this,
                              struct int_str_list value) {
  Vector_int_str_list_grow_if_required(this);
  Vector_int_str_listpush_unchecked(this, value);
}

void Vector_int_str_listpush(struct Vector_int_str_list *this,
                             struct int_str_list value) {
  Vector_int_str_list_push(this, value);
}

struct int_str_list Vector_int_str_listpop(struct Vector_int_str_list *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_int_str_list_shift_left_from(struct Vector_int_str_list *this,
                                         int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_int_str_listremove_at(struct Vector_int_str_list *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_int_str_listvalidate_index(this, index);
  Vector_int_str_list_call_destructor_for_element(this, index);
  Vector_int_str_list_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_int_str_list_clear(struct Vector_int_str_list *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct int_str_list *)malloc(this->capacity *
                                            sizeof(struct int_str_list));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_int_str_listclear(struct Vector_int_str_list *this) {
  Vector_int_str_list_call_destructor_for_all_elements(this);
  Vector_int_str_list_clear(this);
}

void Vector_int_str_listprint(struct Vector_int_str_list *this) {
  // Default overload.
  printf("Dynamic Array (size = %d, capacity = %d) : [ ]", this->size,
         this->capacity);
  // struct int_str_list will be replaced by the actual templated data type.
  printf("Unknown Format Specifier for type struct int_str_list.\n");
}

bool Vector_int_str_list__contains__(struct Vector_int_str_list *this,
                                     struct int_str_list value) {
  size_t tmp_len_12 = Vector_int_str_listlen(this);
  for (size_t i = 0; i < tmp_len_12; i++) {
    struct int_str_list pair = Vector_int_str_list__getitem__(this, i);

    if (pair.key == value.key) {
      // FIXME: Incomplete implementation.
      int_str_list__del__(&pair);
      return true;
    }
    int_str_list__del__(&pair);
  }
  return false;
}

void OrderedDict_Symbol__init__(struct OrderedDict_Symbol *this) {
  this->nodes = NULL;
}

void OrderedDict_Symbol__del__(struct OrderedDict_Symbol *this) {
  struct OrderedDictObject_Symbol *node = this->nodes;
  while (node != NULL) {
    struct OrderedDictObject_Symbol *temp = node;
    node = node->next;
    free(temp);
  }
}

struct Symbol OrderedDict_Symbol__getitem__(struct OrderedDict_Symbol *this,
                                            char *p_key) {
  struct OrderedDictObject_Symbol *node = this->nodes;
  while (node != NULL) {
    struct OrderedDictObject_Symbol *temp = node;
    if (strcmp(node->key_str, p_key) == 0) {
      return node->value;
    }
    node = node->next;
    free(temp);
  }

  struct Symbol item;
  return item;
}

void OrderedDict_Symbol__setitem__(struct OrderedDict_Symbol *this, char *p_key,
                                   struct Symbol p_value) {
  if (!p_key || p_key[0] == '\0') {
    printf("OrderedDict: Key cannot be NULL or empty.\n");
    exit(EXIT_FAILURE);
  }

  struct OrderedDictObject_Symbol *node = this->nodes;
  if (node == NULL) {
    // No items in the dict yet, add the new item as the first node.
    struct OrderedDictObject_Symbol *new_node =
        (struct OrderedDictObject_Symbol *)malloc(
            sizeof(struct OrderedDictObject_Symbol));
    new_node->key_str = strdup(p_key);
    new_node->value = p_value;
    new_node->next = NULL;
    this->nodes = new_node;
  } else {
    struct OrderedDictObject_Symbol *prev = NULL;
    while (node != NULL) {
      if (strcmp(node->key_str, p_key) == 0) {
        free(node->key_str); // Free the old key string
        node->key_str = strdup(p_key);
        if (!node->key_str) {
          printf("OrderedDict: Memory allocation for key failed.\n");
          exit(EXIT_FAILURE);
          return;
        }
        node->value = p_value;
        return;
      }
      prev = node;
      node = node->next;
    }

    // Add a new node since the key was not found.
    struct OrderedDictObject_Symbol *new_node =
        (struct OrderedDictObject_Symbol *)malloc(
            sizeof(struct OrderedDictObject_Symbol));
    new_node->key_str = strdup(p_key);
    new_node->value = p_value;
    new_node->next = NULL;
    prev->next = new_node;
  }
}

bool OrderedDict_Symbol__contains__(struct OrderedDict_Symbol *this,
                                    char *p_key) {
  struct OrderedDictObject_Symbol *node = this->nodes;
  while (node != NULL) {
    if (strcmp(node->key_str, p_key) == 0) {
      return true;
    }
    node = node->next;
  }
  return false;
}

struct Optional_Symbol OrderedDict_Symbolget(struct OrderedDict_Symbol *this,
                                             char *key) {
  struct Optional_Symbol res;
  Optional_Symbol__init__(&res);

  if (OrderedDict_Symbol__contains__(this, key)) {
    struct Symbol value = OrderedDict_Symbol__getitem__(this, key);
    Optional_Symbolset_value(&res, value);
    Symbol__del__(&value);
  }
  return res;
}

void OrderedDict_Symbolpush(struct OrderedDict_Symbol *this, char *p_key,
                            struct Symbol p_value) {
  OrderedDict_Symbol__setitem__(this, p_key, p_value);
}

void Optional_Symbol__init__(struct Optional_Symbol *this) {
  this->_has_value = false;
}

bool Optional_Symbolhas_value(struct Optional_Symbol *this) {
  return this->_has_value;
}

struct Symbol Optional_Symbolget_value(struct Optional_Symbol *this) {
  return this->_value;
}

void Optional_Symbolset_value(struct Optional_Symbol *this,
                              struct Symbol p_value) {
  this->_has_value = true;
  Symbol__reassign__(&this->_value, p_value);
}

size_t Vector_ScopeScopeIDPairlen(struct Vector_ScopeScopeIDPair *this) {
  return this->size;
}

void Vector_ScopeScopeIDPair__init__(struct Vector_ScopeScopeIDPair *this,
                                     int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct ScopeScopeIDPair *)malloc(
      capacity * sizeof(struct ScopeScopeIDPair));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_ScopeScopeIDPair_call_destructor_for_element(
    struct Vector_ScopeScopeIDPair *this, int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  ScopeScopeIDPair__del__(&this->arr[index]);
}

void Vector_ScopeScopeIDPair_call_destructor_for_all_elements(
    struct Vector_ScopeScopeIDPair *this) {
  for (size_t i = 0; i < Vector_ScopeScopeIDPairlen(this); i++) {
    Vector_ScopeScopeIDPair_call_destructor_for_element(this, i);
  }
}

void Vector_ScopeScopeIDPair_reset(struct Vector_ScopeScopeIDPair *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_ScopeScopeIDPair__del__(struct Vector_ScopeScopeIDPair *this) {
  Vector_ScopeScopeIDPair_call_destructor_for_all_elements(this);
  Vector_ScopeScopeIDPair_reset(this);
}

struct ScopeScopeIDPair
Vector_ScopeScopeIDPair__getitem__(struct Vector_ScopeScopeIDPair *this,
                                   int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_ScopeScopeIDPair_set_ith_item(struct Vector_ScopeScopeIDPair *this,
                                          int index,
                                          struct ScopeScopeIDPair value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_ScopeScopeIDPairpush_unchecked(struct Vector_ScopeScopeIDPair *this,
                                           struct ScopeScopeIDPair value) {
  this->arr[this->size++] = value;
}

void Vector_ScopeScopeIDPairvalidate_index(struct Vector_ScopeScopeIDPair *this,
                                           int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_ScopeScopeIDPair_set(struct Vector_ScopeScopeIDPair *this,
                                 int index, struct ScopeScopeIDPair value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_ScopeScopeIDPairvalidate_index(this, index);
  Vector_ScopeScopeIDPair_call_destructor_for_element(this, index);
  Vector_ScopeScopeIDPair_set_ith_item(this, index, value);
}

void Vector_ScopeScopeIDPair__setitem__(struct Vector_ScopeScopeIDPair *this,
                                        int index,
                                        struct ScopeScopeIDPair value) {
  Vector_ScopeScopeIDPair_set(this, index, value);
}

void Vector_ScopeScopeIDPairallocate_more(struct Vector_ScopeScopeIDPair *this,
                                          int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct ScopeScopeIDPair *new_arr = (struct ScopeScopeIDPair *)realloc(
      this->arr, new_capacity * sizeof(struct ScopeScopeIDPair));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_ScopeScopeIDPair_grow_if_required(
    struct Vector_ScopeScopeIDPair *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_ScopeScopeIDPairallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_ScopeScopeIDPairallocate_more(this, 1);
    }
  }
}

void Vector_ScopeScopeIDPair_push(struct Vector_ScopeScopeIDPair *this,
                                  struct ScopeScopeIDPair value) {
  Vector_ScopeScopeIDPair_grow_if_required(this);
  Vector_ScopeScopeIDPairpush_unchecked(this, value);
}

void Vector_ScopeScopeIDPairpush(struct Vector_ScopeScopeIDPair *this,
                                 struct ScopeScopeIDPair value) {
  Vector_ScopeScopeIDPair_push(this, value);
}

struct ScopeScopeIDPair
Vector_ScopeScopeIDPairpop(struct Vector_ScopeScopeIDPair *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_ScopeScopeIDPair_shift_left_from(
    struct Vector_ScopeScopeIDPair *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_ScopeScopeIDPairremove_at(struct Vector_ScopeScopeIDPair *this,
                                      int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_ScopeScopeIDPairvalidate_index(this, index);
  Vector_ScopeScopeIDPair_call_destructor_for_element(this, index);
  Vector_ScopeScopeIDPair_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_ScopeScopeIDPair_clear(struct Vector_ScopeScopeIDPair *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct ScopeScopeIDPair *)malloc(
      this->capacity * sizeof(struct ScopeScopeIDPair));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_ScopeScopeIDPairclear(struct Vector_ScopeScopeIDPair *this) {
  Vector_ScopeScopeIDPair_call_destructor_for_all_elements(this);
  Vector_ScopeScopeIDPair_clear(this);
}

void Vector_ScopeScopeIDPairprint(struct Vector_ScopeScopeIDPair *this) {
  // Default overload.
  printf("Dynamic Array (size = %d, capacity = %d) : [ ]", this->size,
         this->capacity);
  // struct ScopeScopeIDPair will be replaced by the actual templated data type.
  printf("Unknown Format Specifier for type struct ScopeScopeIDPair.\n");
}

bool Vector_ScopeScopeIDPair__contains__(struct Vector_ScopeScopeIDPair *this,
                                         struct ScopeScopeIDPair value) {
  size_t tmp_len_13 = Vector_ScopeScopeIDPairlen(this);
  for (size_t i = 0; i < tmp_len_13; i++) {
    struct ScopeScopeIDPair pair = Vector_ScopeScopeIDPair__getitem__(this, i);

    if (ScopeScopeIDPairget_scope_id(&pair) ==
        ScopeScopeIDPairget_scope_id(&value)) {
      // FIXME: Incomplete implementation.
      ScopeScopeIDPair__del__(&pair);
      return true;
    }
    ScopeScopeIDPair__del__(&pair);
  }
  return false;
}

size_t Vector_intlen(struct Vector_int *this) { return this->size; }

void Vector_int__init__(struct Vector_int *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (int *)malloc(capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_int_call_destructor_for_element(struct Vector_int *this,
                                            int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
}

void Vector_int_call_destructor_for_all_elements(struct Vector_int *this) {
  for (size_t i = 0; i < Vector_intlen(this); i++) {
    Vector_int_call_destructor_for_element(this, i);
  }
}

void Vector_int_reset(struct Vector_int *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_int__del__(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_reset(this);
}

int Vector_int__getitem__(struct Vector_int *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_int_set_ith_item(struct Vector_int *this, int index, int value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_intpush_unchecked(struct Vector_int *this, int value) {
  this->arr[this->size++] = value;
}

void Vector_intvalidate_index(struct Vector_int *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

void Vector_int_set(struct Vector_int *this, int index, int value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_set_ith_item(this, index, value);
}

void Vector_int__setitem__(struct Vector_int *this, int index, int value) {
  Vector_int_set(this, index, value);
}

void Vector_intallocate_more(struct Vector_int *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  int *new_arr = (int *)realloc(this->arr, new_capacity * sizeof(int));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_int_grow_if_required(struct Vector_int *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_intallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_intallocate_more(this, 1);
    }
  }
}

void Vector_int_push(struct Vector_int *this, int value) {
  Vector_int_grow_if_required(this);
  Vector_intpush_unchecked(this, value);
}

void Vector_intpush(struct Vector_int *this, int value) {
  Vector_int_push(this, value);
}

int Vector_intpop(struct Vector_int *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_int_shift_left_from(struct Vector_int *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_intremove_at(struct Vector_int *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_intvalidate_index(this, index);
  Vector_int_call_destructor_for_element(this, index);
  Vector_int_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_int_clear(struct Vector_int *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (int *)malloc(this->capacity * sizeof(int));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_intclear(struct Vector_int *this) {
  Vector_int_call_destructor_for_all_elements(this);
  Vector_int_clear(this);
}

bool Vector_int__contains__(struct Vector_int *this, int value) {
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if (this->arr[i] == value) {
      return true;
    }
  }
  return false;
}

void Vector_intprint(struct Vector_int *this) {
  printf("Vector<int> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%d", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

void Optional_String__init__(struct Optional_String *this) {
  this->_has_value = false;
}

bool Optional_Stringhas_value(struct Optional_String *this) {
  return this->_has_value;
}

struct String Optional_Stringget_value(struct Optional_String *this) {
  return this->_value;
}

void Optional_Stringset_value(struct Optional_String *this,
                              struct String p_value) {
  this->_has_value = true;
  String__reassign__OVDstructString(&this->_value, p_value);
}

// Insert a string at a given index in another string.
struct String insert_string(struct String original_string, int p_index,
                            struct String string_to_insert) {
  // return original_string[:index] + string_to_insert + original_string[index:]
  struct String left_part = Stringsubstr(&original_string, 0, p_index);
  struct String tmp_string_0 = Stringsubstr(
      &original_string, p_index, Stringlen(&original_string) - p_index);
  String__add__(&left_part, Stringc_str(&string_to_insert));
  String__add__(&left_part, Stringc_str(&tmp_string_0));
  String__del__(&tmp_string_0);
  return left_part;
}

struct String escape_quotes(struct String s) {
  // Add \ in front of any " in the string.
  // if we find \", then we don't add \ in front of ".
  struct String result;
  String__init__OVDstrint(&result, "", 0);
  size_t len = Stringlen(&s);

  for (size_t i = 0; i < len; i++) {
    char c = String__getitem__(&s, i);

    if (c == '\"') {

      if (i == 0) {
        String__add__(&result, "\\");
      } else {

        if (!(String__getitem__(&s, i - 1) == '\\')) {
          String__add__(&result, "\\");
        }
      }
    }
    char c_promoted_0[2] = {c, '\0'};
    String__add__(&result, c_promoted_0);
  }
  return result;
}

struct String get_format_specifier(struct String p_type) {
  struct String return_type_str;
  String__init__OVDstrint(&return_type_str, "d", 1);

  if (String__eq__(&p_type, "char")) {
    String__reassign__OVDstr(&return_type_str, "c");
  } else if (String__eq__(&p_type, "int")) {
    String__reassign__OVDstr(&return_type_str, "d");
  } else if (String__eq__(&p_type, "float")) {
    String__reassign__OVDstr(&return_type_str, "f");
  } else if (String__eq__(&p_type, "size_t")) {
    String__reassign__OVDstr(&return_type_str, "llu");
  }
  return return_type_str;
}

struct String get_mangled_fn_name(struct String p_struct_type,
                                  struct String p_fn_name) {
  struct String s;
  String__init__OVDstructString(&s, p_struct_type);
  String__add__(&s, Stringc_str(&p_fn_name));
  return s;
}

struct String
get_templated_mangled_fn_name(struct String p_struct_type,
                              struct String p_fn_name,
                              struct String p_templated_data_type) {
  struct String s;
  String__init__OVDstructString(&s, p_struct_type);
  String__add__(&s, "_");
  String__add__(&s, Stringc_str(&p_templated_data_type));
  String__add__(&s, Stringc_str(&p_fn_name));
  return s;
}

// Cached Items.
// If any structs have __init__ method, then we register them here.
// This could be stored in StructDefination.
struct Set structs_with_constructors;

bool has_constructors(struct String p_struct_type) {
  bool return_value =
      Set__contains__(&structs_with_constructors, p_struct_type);
  return return_value;
}

struct Random random;

struct SymbolTable symbol_table;

int get_current_scope() {
  int return_value = SymbolTablecurrent_scope(&symbol_table);
  return return_value;
}

void increment_scope() { SymbolTableenter_scope(&symbol_table); }

void decrement_scope() { SymbolTableexit_scope(&symbol_table); }

void REGISTER_VARIABLE(struct String p_var_name,
                       struct String p_var_data_type) {
  SymbolTabledeclare_variable(&symbol_table, p_var_name, p_var_data_type);
}

struct Optional_String get_type_of_variable(struct String p_var_name) {
  struct Optional_String return_type;
  Optional_String__init__(&return_type);

  struct Optional_Symbol var =
      SymbolTablelookup_variable(&symbol_table, p_var_name);

  if (Optional_Symbolhas_value(&var)) {
    struct Symbol symbol = Optional_Symbolget_value(&var);
    Optional_Stringset_value(&return_type, Symbolget_data_type(&symbol));
    Symbol__del__(&symbol);
  }

  Optional_Symbol__del__(&var);
  return return_type;
}

bool is_variable_of_type(struct String p_var_name, struct String p_type) {
  struct Optional_String var_type = get_type_of_variable(p_var_name);

  if (Optional_Stringhas_value(&var_type)) {
    struct String value = Optional_Stringget_value(&var_type);
    bool return_value = String__eq__(&value, Stringc_str(&p_type));
    String__del__(&value);
    Optional_String__del__(&var_type);
    return return_value;
  }
  Optional_String__del__(&var_type);
  return false;
}

bool is_variable(struct String p_var_name) {
  struct Optional_String var_type = get_type_of_variable(p_var_name);
  bool return_value = Optional_Stringhas_value(&var_type);
  Optional_String__del__(&var_type);
  return return_value;
}

bool is_variable_char_type(struct String p_var_name) {
  struct String tmp_string_1;
  Stringinit__STATIC__(&tmp_string_1, "char", 4);
  bool return_value = is_variable_of_type(p_var_name, tmp_string_1);
  return return_value;
}

bool is_variable_const_char_ptr(struct String p_var_name) {
  struct String tmp_string_2;
  Stringinit__STATIC__(&tmp_string_2, "c_str", 5);
  bool return_value = is_variable_of_type(p_var_name, tmp_string_2);
  return return_value;
}

bool is_variable_str_type(struct String p_var_name) {
  struct String tmp_string_3;
  Stringinit__STATIC__(&tmp_string_3, "str", 3);

  if (is_variable_of_type(p_var_name, tmp_string_3)) {
    return true;
  }

  struct String tmp_string_4;
  Stringinit__STATIC__(&tmp_string_4, "char*", 5);

  if (is_variable_of_type(p_var_name, tmp_string_4)) {
    return true;
  }

  return false;
}

bool is_variable_boolean_type(struct String p_var_name) {
  struct String tmp_string_5;
  Stringinit__STATIC__(&tmp_string_5, "bool", 4);
  bool return_value = is_variable_of_type(p_var_name, tmp_string_5);
  return return_value;
}

bool is_variable_int_type(struct String p_var_name) {
  struct String tmp_string_6;
  Stringinit__STATIC__(&tmp_string_6, "int", 3);
  bool return_value = is_variable_of_type(p_var_name, tmp_string_6);
  return return_value;
}

bool is_variable_size_t_type(struct String p_var_name) {
  struct String tmp_string_7;
  Stringinit__STATIC__(&tmp_string_7, "size_t", 6);
  bool return_value = is_variable_of_type(p_var_name, tmp_string_7);
  return return_value;
}

int main() {

  // Global Variables Initialization.
  Set__init__(&structs_with_constructors, 5);
  Random__init__(&random);
  SymbolTable__init__(&symbol_table);

  struct String source_file;
  String__init__OVDstrint(&source_file, "../examples/01_variables.c", 26);
  // source_file.printLn()

  // output_file_name = source_file.split(".")[0] + "_generated.c"
  // let _split = source_file.split(" ")
  // let output_file_name = _split[0]
  // output_file_name += "_generated.c"
  // output_file_name.print()

  struct String file;
  String__init__OVDstrint(&file, "", 0);
  StringprintLn(&file);

  struct Vector_String Lines =
      StringreadlinesFrom(&file, Stringc_str(&source_file));
  Vector_Stringprint(&Lines);

  struct Vector_String imported_modules;
  Vector_String__init__(&imported_modules, 5);

  size_t tmp_len_8 = Vector_Stringlen(&Lines);
  for (size_t i = 0; i < tmp_len_8; i++) {
    struct String line = Vector_String__getitem__(&Lines, i);
    struct String Line = Stringstrip(&line);

    if (Stringstartswith(&Line, "import")) {
      struct Vector_String import_split = Stringsplit(&Line, ' ');
      // let module_name = import_split[1]
      // module_name.printLn()

      Vector_Stringpush(&imported_modules,
                        Vector_String__getitem__(&import_split, 1));

      StringprintLn(&Line);
      Vector_String__del__(&import_split);
    }
    String__del__(&Line);
  }

  if (Vector_Stringlen(&imported_modules) > 0) {
    struct Vector_String ImportedCodeLines;
    Vector_String__init__(&ImportedCodeLines, 50);

    size_t tmp_len_9 = Vector_Stringlen(&imported_modules);
    for (size_t i = 0; i < tmp_len_9; i++) {
      struct String module_name =
          Vector_String__getitem__(&imported_modules, i);
      struct String relative_path;
      String__init__OVDstrint(&relative_path, "../Lib/", 7);
      String__add__(&relative_path, Stringc_str(&module_name));
      String__add__(&relative_path, ".c");

      StringprintLn(&relative_path);

      struct String module_file;
      String__init__OVDstrint(&module_file, "", 0);
      struct Vector_String lines =
          StringreadlinesFrom(&module_file, Stringc_str(&relative_path));
      // lines.print()

      // ImportedCodeLines += lines
      size_t tmp_len_10 = Vector_Stringlen(&lines);
      for (size_t j = 0; j < tmp_len_10; j++) {
        struct String line = Vector_String__getitem__(&lines, j);
        Vector_Stringpush(&ImportedCodeLines, line);
      }
      Vector_String__del__(&lines);
      String__del__(&module_file);
      String__del__(&relative_path);
    }
    Vector_String__del__(&ImportedCodeLines);
  }

  struct String s1;
  String__init__OVDstrint(&s1, "Hello World", 11);
  struct String insert;
  String__init__OVDstrint(&insert, "virus", 5);
  int index = 2;

  struct String new_string = insert_string(s1, index, insert);
  Stringprint(&new_string);

  String__del__(&new_string);
  String__del__(&insert);
  String__del__(&s1);
  Vector_String__del__(&imported_modules);
  Vector_String__del__(&Lines);
  String__del__(&file);
  String__del__(&source_file);
  SymbolTable__del__(&symbol_table);
  Set__del__(&structs_with_constructors);

  return 0;
}