#include <stdbool.h>
#include <stdio.h>

#define Option(type)                                                           \
  struct {                                                                     \
    bool is_present;                                                           \
    type value;                                                                \
  }

/*
  trait bool;

  struct Optional<T>{
    bool is_present;
    T value;
  };

  Optional::bool(){
    return is_present;
  }
*/

// STRUCT_DEFINATIONS //

bool array_contains(int *arr, int size, int query) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == query) {
      return true;
    }
  }
  return false;
}

void print_array(int *arr, int size) {
  printf("[");
  for (int i = 0; i < size; i++) {
    printf("%d", arr[i]);
    if (i < size - 1) {
      printf(",");
    }
  }
  printf("]");
}

int main() {

  int meaning = 42;

  // clang-format off

  ///*///
  print("Hello {meaning} , {meaning}")

  Option<int> value;
  Option<float> float_value;
  Option<char> char_value;
  Option<char> char_value2;

  struct Point {int x, int y};
  let p1 = Point{0, 0};

  match p1 {
    (_, _) => printf("\nOrigin.\n");
    (0, y) => printf("\nX-axis.\n");
    (x, 0) => printf("\nY-axis.\n");
    (_, 10) => printf("\nY = 10\n");
    (10, _) => printf("\nX = 10\n");
    (_, _) => printf("\nDefault Match\n");
    (x, y) => printf("\nAny random pos.\n");
  %}

  let arr<int> = [ 1, 2, 3, 4, 5 ];
  for value in arr{
   print("{value} \n")
  %}

  if meaning in arr { 
    print("{meaning} was found in array.\n")
  %}

  match meaning {
    40 ... 100 => printf("In Between 40 and 100.");
    2 | 4 | 6 => printf("Even");
    1 => printf("One");
    2 => printf("Two");
    _ => printf("Nothing");
  %}

  ///*///
  // clang-format on

  return 0;
}