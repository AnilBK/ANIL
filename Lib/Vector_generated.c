
struct Vector {
  int *arr;
  int size;
  int capacity;
};

void __init__Vector(struct Vector *this, int capacity) {
  this->arr = (int *)malloc(capacity * sizeof(int));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void push(struct Vector *this, int value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (int *)realloc(this->arr, this->capacity * sizeof(int));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void freeVector(struct Vector *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void printVector(struct Vector *this) {
  printf("Dynamic Array (size=%zu, capacity=%zu): [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("%d", this->arr[i]);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

///*///

///*///
