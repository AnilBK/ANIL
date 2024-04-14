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

  ///*///

  char str[100] = "Hello";
  puts(str);

  char *iterator = str;
  while (*iterator != '\0') {
    char value = *iterator;
    iterator++;
    putchar(value);
  }

  if (strcmp(str, "Hello") == 0) {
    puts("Str is hello.");
  }

  strcat(str, " World");
  puts(str);

  strcat(str, " Its me Anil");
  puts(str);

  if (strstr(str, "xxx") != NULL) {
    printf("xxx is in the given string\n ");
  }

  int arr[] = {1, 2, 3, 4, 5};
  unsigned int arr_array_size = 5;

  if (array_contains(arr, arr_array_size, 10)) {
    printf("10 is in arr. ");
  }

  for (unsigned int i = 0; i < arr_array_size; i++) {
    int value = arr[i];
    printf("%d \n ", value);
  }

  ///*///

  return 0;
}