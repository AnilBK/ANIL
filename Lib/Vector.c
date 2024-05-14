// clang-format off
///*///

#include <string.h>

///////////////////////////////////////////
struct Vector<T>{T* arr,int size,int capacity};

impl Vector __init__ capacity : int
  // if we want to use instanced template type in fn body, we use following syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (@TEMPLATED_DATA_TYPE@ *)malloc(capacity * sizeof(@TEMPLATED_DATA_TYPE@));

  if (this->arr == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
endfunc  

impl Vector __del__
  // Python Version of destructor.
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
endfunc

impl Vector push value : T
   if (this->size == this->capacity) {
      this->capacity *= 2;
      this->arr = (@TEMPLATED_DATA_TYPE@ *)realloc(this->arr, this->capacity * sizeof(@TEMPLATED_DATA_TYPE@));
      if (this->arr == NULL) {
          fprintf(stderr, "Memory reallocation failed.\n");
          exit(EXIT_FAILURE);
      }
  }
  this->arr[this->size++] = value;
endfunc


impl Vector allocate_more n : int
  this->capacity += n;
  this->arr = (@TEMPLATED_DATA_TYPE@  *)realloc(this->arr, this->capacity * sizeof(@TEMPLATED_DATA_TYPE@));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
endfunc

impl Vector push_unchecked value : T
  this->arr[this->size++] = value;
endfunc

impl Vector<> print
  //maybe print instanced vec name.
  //instanced name should be passed as fn parameter ig.
  //or add instance_name member silently to the struct itself.
  //printf("@INSTANCE_NAME@\n");
  /*
  
  void Vector_print(struct Vector* this, const char* p_instance_name){
    printf("%s\n", p_instance_name);
  }
  */

  printf("Dynamic Array (size = %zu, capacity = %zu) : [", this->size, this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
      const char* type = "@TEMPLATED_DATA_TYPE@";
      if(strcmp(type, "float") == 0){
        printf("%f", this->arr[i]);
      }else if(strcmp(type, "int") == 0){
        printf("%d", this->arr[i]);
      }else if(strcmp(type, "char") == 0){
        printf("%c", this->arr[i]);
      }
      if (i < this->size - 1) {
          printf(", ");
      }
  }
  printf("]\n");
endfunc

impl Vector<String> print
  for (size_t i = 0; i < this->size; ++i) {
    printf("%s\n",this->arr[i].arr);
  }    
endfunc

impl Vector<> __contains__ value : T -> bool
  // This function is an overloaded function.
  // Here <> in function defination means the base overload.
  for (size_t i = 0; i < this->size; ++i) {
      if(this->arr[i] == value){
        return true;
      }
  }    
  return false;  
endfunc

impl Vector<String> __contains__ value : T -> bool
  for (size_t i = 0; i < this->size; ++i) {
      if(strcmp(this->arr[i].arr,value.arr) == 0){
        return true;
      }
  }    
  return false;  
endfunc


impl Vector len -> size_t:
  return this->size;
endfunc

impl Vector __getitem__ index : int -> T
  return *(this->arr + index);
endfunc

def pushn(X...) for Vector:
  self.allocate_more(X.size)
	forall x: self.push_unchecked(x)
enddef  
///*///

