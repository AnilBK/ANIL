#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool array_contains(int *arr, int size, int query) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == query) {
      return true;
    }
  }
  return false;
}

int main() {

  // clang-format off

  ///*/// 

  import String
  
  let str = "Hello";
  puts(str);

  for value in str{
   putchar(value);
  %}

  if str == "Hello"{
      puts("Str is hello.");
  %}

  str += " World";
  puts(str);
  
  str += " Its me Anil"
  puts(str);

  if "xxx" in str { 
    print(f "xxx is in the given string\n"); 
  %}

  let arr = [1, 2, 3, 4, 5];

  if 10 in arr{
    print(f "10 is in arr.");
  %}

  for value in arr{
   print(f "{value} \n"); 
  %}

  ///*///
  // clang-format on

  return 0;
}