///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

#include <stdbool.h>
#include <stdio.h>

// IMPORTS //

struct String {
  char *arr;
  int length;
  int capacity;
};

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
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
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

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) { free(this->arr); }

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

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

// template Vector<String> {
size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

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

void Vector_String__del__(struct Vector_String *this) {
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }

  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  struct String str;
  String__init__OVDstructString(&str, value);

  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct String *)realloc(this->arr, this->capacity *
                                                        sizeof(struct String));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = str;
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  this->capacity += n;
  this->arr = (struct String *)realloc(this->arr,
                                       this->capacity * sizeof(struct String));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  for (size_t i = 0; i < this->size; ++i) {
    if (strcmp(this->arr[i].arr, value.arr) == 0) {
      return true;
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

// template Vector<String> }

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // TODO: Because of this function, before import String, we require import
  // Vector.
  struct Vector_String result;
  Vector_String__init__(&result, 2);

  int delim_location = -1;

  int len = this->length;
  for (int i = 0; i < len; i++) {
    if (this->arr[i] == delimeter) {
      int length = i - (delim_location + 1);

      struct String text = Stringsubstr(this, delim_location + 1, length);
      Vector_Stringpush(&result, text);
      String__del__(&text);

      delim_location = i;
    }
  }

  // Add remaining string.
  if (delim_location + 1 < len) {
    char *remaining = &this->arr[delim_location + 1];

    struct String text;
    String__init__OVDstr(&text, remaining);
    Vector_Stringpush(&result, text);
    String__del__(&text);
  }

  return result;
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

  // TODO: Use CPL to generate this, because the function below is a mangled
  // function name.
  Stringclear(this);

  FILE *ptr;

  ptr = fopen(pfilename, "r");

  if (ptr == NULL) {
    printf("File can't be opened.\n");
    exit(0);
  }

  char myString[256];
  while (fgets(myString, 256, ptr)) {
    String__add__(this, myString);
  }

  fclose(ptr);
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

struct StructInstance {
  struct String struct_type;
  struct String struct_name;
  bool is_templated;
  struct String templated_data_type;
  int scope;
  bool should_be_freed;
  bool is_pointer_type;
};

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

void StructInstance__del__(struct StructInstance *this) {
  String__del__(&this->struct_type);
  String__del__(&this->struct_name);
  String__del__(&this->templated_data_type);
}

struct Symbol {
  struct String name;
  struct String data_type;
};

void Symbol__init__(struct Symbol *this, struct String p_name,
                    struct String p_data_type) {
  String__init__OVDstructString(&this->name, p_name);

  String__init__OVDstr(&this->data_type, "");
  // don't initialize data_type directly from p_data_type, so we can see,
  // reassign parsing is working as expected.
  String__reassign__OVDstructString(&this->data_type, p_data_type);
}

void Symbol__del__(struct Symbol *this) {
  String__del__(&this->name);
  String__del__(&this->data_type);
}

///*///

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
  // result variable is in String readLines function.
  // So, if we use result2 here, the types mix:
  // TODO : Investigate.
  struct String result2;
  String__init__OVDstr(&result2, "");
  size_t len = Stringlen(&s);

  for (size_t i = 0; i < len; i++) {
    char c = String__getitem__(&s, i);

    if (c == '\"') {

      if (i == 0) {
        String__add__(&result2, "\\");
      } else {

        if (!(String__getitem__(&s, i - 1) == '\\')) {
          String__add__(&result2, "\\");
        }
      }
    }
    char c_promoted_0[2] = {c, '\0'};
    String__add__(&result2, c_promoted_0);
  }
  return result2;
}

struct String get_format_specifier(struct String p_type) {
  struct String return_type_str;
  String__init__OVDstr(&return_type_str, "d");

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
  String__del__(&s);
  return s;
}

struct String
get_templated_mangled_fn_name(struct String p_struct_type1,
                              struct String p_fn_name1,
                              struct String p_templated_data_type1) {
  struct String s1;
  String__init__OVDstructString(&s1, p_struct_type1);
  String__add__(&s1, "_");
  String__add__(&s1, Stringc_str(&p_templated_data_type1));
  String__add__(&s1, Stringc_str(&p_fn_name1));
  return s1;
}

// function get_destructor_for_struct(p_name : String) -> String:
//   let instanced_struct_names = Vector<StructInstance>{10};
//   for m_struct in instanced_struct_names[::-1]{
//     if m_struct.should_be_freed{
//       let des_code = "{destructor_fn_name}(&{struct_name});\n"
//       return des_code
//     }
//   }
//   let code = ""
//   return code
// endfunction
///*///

int main() {

  ///*///

  struct String source_file;
  String__init__OVDstr(&source_file, "../examples/01_variables.c");
  // source_file.printLn()

  // output_file_name = source_file.split(".")[0] + "_generated.c"
  // let _split = source_file.split(" ")
  // let output_file_name = _split[0]
  // output_file_name += "_generated.c"
  // output_file_name.print()

  struct String file;
  String__init__OVDstr(&file, "");
  StringprintLn(&file);

  struct Vector_String Lines =
      StringreadlinesFrom(&file, Stringc_str(&source_file));
  Vector_Stringprint(&Lines);

  struct Vector_String imported_modules;
  Vector_String__init__(&imported_modules, 5);

  size_t tmp_len_0 = Vector_Stringlen(&Lines);
  for (size_t i = 0; i < tmp_len_0; i++) {
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

    size_t tmp_len_1 = Vector_Stringlen(&imported_modules);
    for (size_t i = 0; i < tmp_len_1; i++) {
      struct String module_name =
          Vector_String__getitem__(&imported_modules, i);
      struct String relative_path;
      String__init__OVDstr(&relative_path, "../Lib/");
      String__add__(&relative_path, Stringc_str(&module_name));
      String__add__(&relative_path, ".c");

      StringprintLn(&relative_path);

      struct String module_file;
      String__init__OVDstr(&module_file, "");
      struct Vector_String lines =
          StringreadlinesFrom(&module_file, Stringc_str(&relative_path));
      // lines.print()

      // ImportedCodeLines += lines
      size_t tmp_len_2 = Vector_Stringlen(&lines);
      for (size_t j = 0; j < tmp_len_2; j++) {
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
  String__init__OVDstr(&s1, "Hello World");
  struct String insert;
  String__init__OVDstr(&insert, "virus");
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
  ///*///

  return 0;
}