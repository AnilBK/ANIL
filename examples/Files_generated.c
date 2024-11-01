///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct File {
  FILE *file_ptr;
};

void File__init__(struct File *this, char *p_file_name);
void Filewriteline(struct File *this, char *p_content);
void File__del__(struct File *this);

void File__init__(struct File *this, char *p_file_name) {
  this->file_ptr = fopen(p_file_name, "w");
  if (this->file_ptr == NULL) {
    printf("Failed to open file %s.\n", p_file_name);
    exit(0);
  }
}

void Filewriteline(struct File *this, char *p_content) {
  // Write a line to the file with terminating newline.
  fprintf(this->file_ptr, "%s\n", p_content);
}

void File__del__(struct File *this) { fclose(this->file_ptr); }

int main() {

  ///*/// main()

  struct File outputFile;
  File__init__(&outputFile, "hello.txt");
  Filewriteline(&outputFile, "Hello World :)");

  File__del__(&outputFile);
  ///*///

  return 0;
}