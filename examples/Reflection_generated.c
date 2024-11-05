///*///

#include <string.h>

///////////////////////////////////////////

///*///

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
  struct CPLObject *next;
};

struct List {
  struct CPLObject *head;
  struct CPLObject *tail;
  int size;
};

void CPLObject__init__OVDint(struct CPLObject *this, int p_value);
void CPLObject__init__OVDstr(struct CPLObject *this, char *p_value);
bool CPLObjectis_int(struct CPLObject *this);
int CPLObjectget_int(struct CPLObject *this);
bool CPLObjectis_str(struct CPLObject *this);
char *CPLObjectget_str(struct CPLObject *this);
void CPLObject_clear_str(struct CPLObject *this);
void CPLObject__del__(struct CPLObject *this);
struct CPLObject CPLObject_duplicate(struct CPLObject *this);
bool CPLObject__eq__OVDint(struct CPLObject *this, int p_value);
bool CPLObject__eq__OVDstr(struct CPLObject *this, char *p_value);

int Listlen(struct List *this);
void List__init__(struct List *this);
void List__del__(struct List *this);
struct CPLObject List__getitem__(struct List *this, int index);
struct CPLObject Listpop(struct List *this, int index);
bool List__contains__OVDint(struct List *this, int p_value);
bool List__contains__OVDstr(struct List *this, char *p_value);
void Listprint(struct List *this);
void List_insert_end(struct List *this, CPLObjectptr new_node);
CPLObjectptr Listcreate_int_node(struct List *this, int p_value);
CPLObjectptr Listcreate_string_node(struct List *this, char *p_value);
void Listappend_int(struct List *this, int p_value);
void Listappend_str(struct List *this, char *p_value);
void ListappendOVDint(struct List *this, int p_value);
void ListappendOVDstr(struct List *this, char *p_value);
void ListappendOVDstructCPLObject(struct List *this, struct CPLObject p_value);
void List__reassign__(struct List *this, struct List p_list);

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

void ListappendOVDstructCPLObject(struct List *this, struct CPLObject p_value) {

  if (CPLObjectis_int(&p_value)) {
    Listappend_int(this, CPLObjectget_int(&p_value));
  } else if (CPLObjectis_str(&p_value)) {
    Listappend_str(this, CPLObjectget_str(&p_value));
  }
}

void List__reassign__(struct List *this, struct List p_list) {
  int tmp_len_0 = Listlen(&p_list);
  for (size_t i = 0; i < tmp_len_0; i++) {
    struct CPLObject item = List__getitem__(&p_list, i);
    ListappendOVDstructCPLObject(this, item);
    CPLObject__del__(&item);
  }
}

int main() {

  ///*/// main()

  struct List vector_class_members;
  List__init__(&vector_class_members);
  struct List vector_class_member_functions;
  List__init__(&vector_class_member_functions);
  struct List list_instances;
  List__init__(&list_instances);

  Listappend_str(&vector_class_members, "arr");
  Listappend_str(&vector_class_members, "size");
  Listappend_str(&vector_class_members, "capacity");

  Listappend_str(&list_instances, "vector_class_members");
  Listappend_str(&list_instances, "vector_class_member_functions");
  Listappend_str(&list_instances, "list_instances");

  // ^^^^^^^ just writing the macro name calls the macro.
  printf("We print the different instances of class List using Reflection as "
         "follows: \n");

  // 'instances_of_class' returns name of the instances in "string" form,
  // so, we unquote that using UNQUOTE in macro as belows.
  Listprint(&vector_class_members);
  Listprint(&vector_class_member_functions);
  Listprint(&list_instances);

  List__del__(&list_instances);
  List__del__(&vector_class_member_functions);
  List__del__(&vector_class_members);
  ///*///

  return 0;
}