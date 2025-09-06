// clang-format off
///*///

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

struct String{char* arr, int length, int capacity, bool is_constexpr};

namespace String
function c_str() -> str:
  return this.arr
endfunction

function len() -> size_t:
  return this.length
endfunction

c_function __getitem__(index: int) -> char:
  return *(this->arr + index);
endc_function

c_function length_of_charptr(p_string:str) -> size_t:
  // This should be some kind of static method.
  return strlen(p_string);
endc_function

c_function __init__from_charptr(text: str, p_text_length: int)
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
endc_function

c_function init__STATIC__(text: str, p_text_length: int)
  // WARNING: Only the compiler should write a call to this function.
  // The compiler uses this initialization function to create a temporary String object
  // when a string literal is passed to a function that expects a String object.
  this->arr = text;
  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = true;
endc_function

function __init__<>(text: str)
  let p_text_length = this.length_of_charptr(text)
  this.__init__from_charptr(text, p_text_length)
endfunction

function __init__<>(text: str, p_text_length: int)
  this.__init__from_charptr(text, p_text_length)
endfunction

function __init__<>(text: String)
  let p_text_length = text.len()
  this.__init__from_charptr(text, p_text_length);
endfunction

c_function clear()
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
endc_function

c_function _allocate_more(n: int)
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return; 
  } 

  size_t new_capacity = this->capacity + n + 1;

  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
  if (!new_arr) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  } else{
    this->arr = new_arr;
    this->capacity = new_capacity;
  }
endc_function

c_function print()
  printf("%s", this->arr);
endc_function

c_function printLn()
  printf("%s\n", this->arr);
endc_function

c_function __del__()
  if (!this->is_constexpr){
    free(this->arr);
  }
endc_function

c_function startswith(prefix: str) -> bool:
	return strncmp(this->arr, prefix, strlen(prefix)) == 0;
endc_function

c_function substr(start : int, length : int) -> String:
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;    
endc_function

c_function strip() -> String:
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
endc_function

function split(delimeter: char) -> Vector<String>:
  // NOTE : Because of this function, before import String, we require import Vector.
  let split_result = Vector<String>{2};

  let index : int = 0
  let segment_start : int = 0

  for character in this{
    if character == delimeter {
      if segment_start < index {
        let segment = this.substr(segment_start, index - segment_start)
        split_result.push(segment)
      }
      segment_start = index + 1
    }
    index = index + 1
  }

  if segment_start < this.len() {
    let remaining_segment = this.substr(segment_start, this.len() - segment_start)
    split_result.push(remaining_segment)
  }

  return split_result
endfunction

c_function __contains__(substring: str) -> bool:
  return strstr(this->arr, substring) != NULL;
endc_function

c_function __eq__(pstring: str) -> bool:
  return strcmp(this->arr, pstring) == 0;
endc_function

c_function __add__(pstring: str)
  size_t new_length = this->length + strlen(pstring) + 1;
  
  if(new_length > this->capacity){
    size_t new_capacity;
    if(this->capacity == 0){
      new_capacity = new_length * 2;
    }else{
      new_capacity = this->capacity;
      while(new_capacity <= new_length){
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
endc_function

c_function reassign_internal(pstring: str, p_text_length: int)
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
endc_function

function __reassign__<>(pstring: String)
  let src = pstring.c_str()
  let p_text_length = pstring.len()
  this.reassign_internal(src, p_text_length)
endfunction

function __reassign__<>(pstring: str)
  let p_text_length = this.length_of_charptr(pstring)
  this.reassign_internal(pstring, p_text_length)
endfunction

c_function set_to_file_contents(pfilename: str)
  if(this->is_constexpr){
    // Probably not necessary, as constexpr strings are compiler generated, but just in case.
    fprintf(stderr, "Error: Attempt to modify a constexpr String object.\n");
    exit(EXIT_FAILURE);
  }

  // Use fopen in binary read mode ("rb") to prevent newline translation.
  FILE *ptr = fopen(pfilename, "rb");
  if (ptr == NULL) {
    fprintf(stderr, "File \"%s\" couldn't be opened.\n", pfilename);
    Stringclear(this);
    return;
  }

  fseek(ptr, 0, SEEK_END);
  long fileSize = ftell(ptr);
  if (fileSize < 0) {
    fprintf(stderr, "Failed to get file size");
    fclose(ptr);
    Stringclear(this);
    return;
  }

  fseek(ptr, 0, SEEK_SET);

  int buffer_capacity = fileSize + 1;
  char *buffer = (char *)malloc(buffer_capacity);
  if (buffer == NULL) {
    fprintf(stderr, "Memory allocation failed for file content.\n");
    fclose(ptr);
    Stringclear(this);
    return;
  }

  size_t bytesRead = fread(buffer, 1, fileSize, ptr);
  fclose(ptr);

  if (bytesRead != (size_t)fileSize) {
    fprintf(stderr, "Error reading file \"%s\". Expected %ld bytes, got %zu.\n",
      pfilename, fileSize, bytesRead);
    free(buffer);
    Stringclear(this);
    return;
  }

  buffer[fileSize] = '\0';

  if (this->arr != NULL){
    free(this->arr);
  }

  // Take ownership of the buffer.
  // The buffer should not be freed after this point.
  this->arr = buffer;
  this->length = fileSize;
  this->capacity = buffer_capacity;
endc_function

function readlinesFrom(pfilename: str) -> Vector<String>:
  this.set_to_file_contents(pfilename)
  let result = this.split("\n")
  return result
endfunction

endnamespace

///*///

