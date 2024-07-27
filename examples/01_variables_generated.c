#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

int main() {

  ///*///

  int x = 69420;
  printf("The magic number is %d.\n", x);

  // Char uses " " but the contents inside it should be of length 1.
  char gender = 'M';
  printf("The gender is %c. \n", gender);

  int arr[] = {1, 2, 3, 4, 5, 10};
  unsigned int arr_array_size = 6;

  for (unsigned int i = 0; i < arr_array_size; i++) {
    int value2 = arr[i];
    printf("%d \n", value2);
  }

  bool arr__contains__10_0 = false;
  for (unsigned int i = 0; i < arr_array_size; i++) {
    if (arr[i] == 10) {
      arr__contains__10_0 = true;
      break;
    }
  }

  if (arr__contains__10_0) {
    printf("10 is in arr. \n");
  }

  float arr2[] = {1, 2, 3, 4, 5, 10};
  unsigned int arr2_array_size = 6;

  for (unsigned int i = 0; i < arr2_array_size; i++) {
    float value3 = arr2[i];
    printf("%f \n", value3);
  }

  for (size_t i = 1; i < 10; i++) {
    printf("%llu\n", i);
  }

  for (size_t i = 1; i < 10; i += 2) {
    printf("%llu\n", i);
  }

  for (size_t i = 1; i <= 10; i++) {
    printf("%llu\n", i);
  }

  for (size_t i = 1; i <= 10; i += 2) {
    printf("%llu\n", i);
  }

  for (size_t i = 10; i >= 1; i += -2) {
    printf("%llu\n", i);
  }

  ///*///

  return 0;
}