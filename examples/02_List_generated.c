#include <stdlib.h>
#include <string.h>

typedef union {
  int int_data;
  char *str_data;
} CPLObject_Data;

typedef enum { INT, STRING } CPLObject_DataType;

typedef struct CPLObject CPLObject;
typedef CPLObject *CPLObjectptr;

///*///

///*///

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct CPLObject {
  CPLObject_Data data;
  CPLObject_DataType data_type;
  CPLObject *next;
};

void CPLObject__init__OVDint(struct CPLObject *this, int p_value) {
  this->data.int_data = p_value;
  this->data_type = INT;
  this->next = NULL;
}

void CPLObject__init__OVDstr(struct CPLObject *this, char *p_value) {
  this->data.str_data = strdup(p_value);
  this->data_type = STRING;
  this->next = NULL;
}

bool CPLObjectis_int(struct CPLObject *this) { return this->data_type == INT; }

int CPLObjectget_int(struct CPLObject *this) { return this->data.int_data; }

bool CPLObjectis_str(struct CPLObject *this) {
  return this->data_type == STRING;
}

char *CPLObjectget_str(struct CPLObject *this) { return this->data.str_data; }

void CPLObject_clear_str(struct CPLObject *this) { free(this->data.str_data); }

void CPLObject__del__(struct CPLObject *this) {

  if (CPLObjectis_str(this)) {
    CPLObject_clear_str(this);
  }
}

struct CPLObject CPLObject_duplicate(struct CPLObject *this) {
  // Perform a deep copy.
  CPLObject copy = *this;
  if (this->data_type == STRING) {
    copy.data.str_data = strdup(this->data.str_data);
  }
  return copy;
}

bool CPLObject__eq__OVDint(struct CPLObject *this, int p_value) {

  if (CPLObjectis_int(this)) {
    return CPLObjectget_int(this) == p_value;
  } else {
    return false;
  }
}

bool CPLObject__eq__OVDstr(struct CPLObject *this, char *p_value) {

  if (CPLObjectis_str(this)) {
    return strcmp(p_value, CPLObjectget_str(this)) == 0;
  } else {
    return false;
  }
}

struct List {
  CPLObject *head;
  CPLObject *tail;
  int size;
};

int Listlen(struct List *this) { return this->size; }

void List__init__(struct List *this) {
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

void List__del__(struct List *this) {
  CPLObject *current = this->head;
  while (current != NULL) {
    CPLObject *temp = current;
    current = current->next;

    CPLObject__del__(temp);
    free(temp);
  }
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

struct CPLObject List__getitem__(struct List *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  CPLObject *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  // Duplicate contents of node and return it.
  // If we return a reference, then the calling function will call destructor,
  // which will free the str_data causing free() errors later.
  return CPLObject_duplicate(current);
}

struct CPLObject Listpop(struct List *this, int index) {
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    exit(EXIT_FAILURE);
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  CPLObject *current = this->head;
  CPLObject *previous = NULL;

  for (int i = 0; i < index; i++) {
    previous = current;
    current = current->next;
  }

  if (previous == NULL) {
    // Popping the head.
    this->head = current->next;
    if (this->head == NULL) {
      // The list is now empty.
      this->tail = NULL;
    }
  } else {
    previous->next = current->next;
    if (current->next == NULL) {
      // Popping the tail.
      this->tail = previous;
    }
  }

  this->size--;

  CPLObject popped_node = *current;
  // Don't free current->data.str_data even though current data_type is String.
  // After copying the *pointer above, popped_node now owns
  // current->data.str_data. This avoids duplicating current->data.str_data into
  // popped_node.
  free(current);
  return popped_node;
}

bool List__contains__OVDint(struct List *this, int p_value) {
  CPLObject *current = this->head;
  while (current != NULL) {
    if (current->data_type == INT) {
      int data = current->data.int_data;

      if (data == p_value) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

bool List__contains__OVDstr(struct List *this, char *p_value) {
  CPLObject *current = this->head;
  while (current != NULL) {
    if (current->data_type == STRING) {
      char *data = current->data.str_data;

      if (strcmp(data, p_value) == 0) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

void Listprint(struct List *this) {
  CPLObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\"", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d", data);
      //@hook_end
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

typedef void (*custom_integer_printer)(int);
void Listprint_hooked_custom_integer_printer(
    struct List *this, custom_integer_printer p_custom_integer_printer) {
  CPLObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data_type == STRING) {
      printf("\"%s\"", current->data.str_data);
    } else {
      int data = current->data.int_data;

      //
      p_custom_integer_printer(data);
    }

    current = current->next;
    if (current != NULL) {
      printf(", ");
    }
  }
  printf("]\n");
}

void List_insert_end(struct List *this, CPLObjectptr new_node) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
}

CPLObjectptr Listcreate_int_node(struct List *this, int p_value) {
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type int for value %d.",
           p_value);
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDint(new_node, p_value);
  return new_node;
}

CPLObjectptr Listcreate_string_node(struct List *this, char *p_value) {
  CPLObject *new_node = (CPLObject *)malloc(sizeof(CPLObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  CPLObject__init__OVDstr(new_node, p_value);
  return new_node;
}

void Listappend_int(struct List *this, int p_value) {
  CPLObjectptr int_node = Listcreate_int_node(this, p_value);
  List_insert_end(this, int_node);
}

void Listappend_str(struct List *this, char *p_value) {
  CPLObjectptr str_node = Listcreate_string_node(this, p_value);
  List_insert_end(this, str_node);
}

void ListappendOVDint(struct List *this, int p_value) {
  Listappend_int(this, p_value);
}

void ListappendOVDstr(struct List *this, char *p_value) {
  Listappend_str(this, p_value);
}

int main() {

  ///*///  main()

  struct List test_list;
  List__init__(&test_list);
  // The following line is also valid, as the above syntax is shorthand for the
  // statement below. let test_list = List{};

  // append is an overloaded function.
  // append_str() and append_int() are explicit versions,
  // which are called by the overloaded append functions.
  // test_list.append_str("Hello")
  // test_list.append_int(10)

  ListappendOVDstr(&test_list, "Hello");
  ListappendOVDstr(&test_list, "World");
  ListappendOVDstr(&test_list, "Puppy");

  ListappendOVDint(&test_list, 10);
  ListappendOVDint(&test_list, 20);
  ListappendOVDint(&test_list, 30);
  ListappendOVDint(&test_list, 40);
  ListappendOVDint(&test_list, 50);

  struct CPLObject node = Listpop(&test_list, 7);

  Listprint(&test_list);

  if (List__contains__OVDint(&test_list, 10)) {
    printf("10 is in the list.");
  }

  if (!List__contains__OVDint(&test_list, 5)) {
    printf("5 is not in the list.");
  }

  CPLObject__del__(&node);
  List__del__(&test_list);
  ///*///

  return 0;
}