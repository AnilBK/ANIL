// clang-format off
///*///

#include <string.h>

///////////////////////////////////////////
struct Vector<T>{T* arr,int size,int capacity};

namespace Vector
function len() -> size_t:
  return this.size
endfunction

c_function<> __getitem__(index : int) -> T:
  if (index < 0){index += this->size;}
  return *(this->arr + index);
endc_function

c_function<String> __getitem__(index : int) -> &T:
  if (index < 0){index += this->size;}
  // Vector<String> Specialization: 
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the end of the loop.
  // Since __getitem__() is a reference return type, it isn't freed.
  return *(this->arr + index);
endc_function

c_function<> __setitem__(index : int, value : T)
  if (index < 0){index += this->size;}
  // FIXME: If previous value is a struct with destructor, then that destructor should be called.
  // This is fixed in the next overloaded function for String class.
  this->arr[index] = value;
endc_function

c_function<String> __setitem__(index : int, value : T)
  if (index < 0){index += this->size;}

  String__del__(&this->arr[index]);

  struct String str;
  String__init__OVDstructString(&str, value);

  this->arr[index] = str;
endc_function

c_function __init__(capacity : int)
  // if we want to use instanced template type in fn body, we use following syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (@TEMPLATED_DATA_TYPE@ *)malloc(capacity * sizeof(@TEMPLATED_DATA_TYPE@));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
endc_function  

c_function<> __del__()
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
endc_function

c_function<String> __del__()
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }    

  free(this->arr);  
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
endc_function

c_function<> push(value : T)
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (@TEMPLATED_DATA_TYPE@ *)realloc(this->arr, this->capacity * sizeof(@TEMPLATED_DATA_TYPE@));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
endc_function

c_function<String> push(value : T)
  // Vector<String> Specialization: 
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  struct String str;
  String__init__OVDstructString(&str, value);

  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (@TEMPLATED_DATA_TYPE@ *)realloc(this->arr, this->capacity * sizeof(@TEMPLATED_DATA_TYPE@));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = str;
endc_function

c_function allocate_more(n : int)
  this->capacity += n;
  this->arr = (@TEMPLATED_DATA_TYPE@  *)realloc(this->arr, this->capacity * sizeof(@TEMPLATED_DATA_TYPE@));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
endc_function

c_function push_unchecked(value : T)
  this->arr[this->size++] = value;
endc_function

c_function pop() -> T:
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
endc_function

c_function remove_at(index : int)
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
  this->size--;
endc_function

c_function<> clear()
  free(this->arr);

  this->capacity = 1; 
  this->arr = (@TEMPLATED_DATA_TYPE@  *)malloc(this->capacity * sizeof(@TEMPLATED_DATA_TYPE@ ));
  
  if (this->arr == NULL) {
    fprintf(stderr, "clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
endc_function

c_function<String> clear()
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }    

  free(this->arr);

  this->capacity = 1; 
  this->arr = (@TEMPLATED_DATA_TYPE@  *)malloc(this->capacity * sizeof(@TEMPLATED_DATA_TYPE@ ));
  
  if (this->arr == NULL) {
    fprintf(stderr, "clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
endc_function

c_function<> __contains__(value : T) -> bool:
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
    if(this->arr[i] == value){
      return true;
    }
  }    
  return false;  
endc_function

function<String> __contains__(value : T) -> bool:
  for string in this{
    if string.len() == value.len(){
      if string == value{
        return true
      }
    }
  }
  return false
endfunction


def pushn(X...) for Vector:
  self.allocate_more(X.size)
	forall x: self.push_unchecked(x)
enddef  

c_function<> print()
  // Default overload.
  printf("Dynamic Array (size = %d, capacity = %d) : [ ]", this->size, this->capacity);
  // @TEMPLATED_DATA_TYPE@ will be replaced by the actual templated data type.
  printf("Unknown Format Specifier for type @TEMPLATED_DATA_TYPE@.\n");
endc_function

c_function<int> print()
  printf("Vector<int> (size = %d, capacity = %d) : [", this->size, this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%d",this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
endc_function

c_function<float> print()
  printf("Vector<float> (size = %d, capacity = %d) : [", this->size, this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%f",this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
endc_function

c_function<char> print()
  printf("Vector<char> (size = %d, capacity = %d) : [", this->size, this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\'%c\'",this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
endc_function

c_function<String> print()
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size, this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"",this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
endc_function
endnamespace
///*///

