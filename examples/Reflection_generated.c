
#include <string.h>

///////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

typedef struct Variant {
  enum { INT, STRING } data_type;

  union {
    int int_data;
    char *str_data;
  } data;
} Variant;

typedef struct Variant Variant;
typedef struct ListObject *ListObjectptr;

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// IMPORTS //

struct ListObject {
  Variant data;
  struct ListObject *next;
};

struct List {
  struct ListObject *head;
  struct ListObject *tail;
  int size;
};

void ListObject__init__OVDint(struct ListObject *this, int p_value);
void ListObject__init__OVDstr(struct ListObject *this, char *p_value);
bool ListObjectis_int(struct ListObject *this);
int ListObjectget_int(struct ListObject *this);
bool ListObjectis_str(struct ListObject *this);
char *ListObjectget_str(struct ListObject *this);
void ListObject_clear_str(struct ListObject *this);
void ListObject__del__(struct ListObject *this);
struct ListObject ListObject_duplicate(struct ListObject *this);
bool ListObject__eq__OVDint(struct ListObject *this, int p_value);
bool ListObject__eq__OVDstr(struct ListObject *this, char *p_value);

int Listlen(struct List *this);
void List__init__(struct List *this);
void List__del__(struct List *this);
struct ListObject List__getitem__(struct List *this, int index);
struct ListObject Listpop(struct List *this, int index);
bool List__contains__OVDint(struct List *this, int p_value);
bool List__contains__OVDstr(struct List *this, char *p_value);
void Listprint(struct List *this);
void List_insert_end(struct List *this, ListObjectptr new_node);
ListObjectptr Listcreate_int_node(struct List *this, int p_value);
ListObjectptr Listcreate_string_node(struct List *this, char *p_value);
void Listappend_int(struct List *this, int p_value);
void Listappend_str(struct List *this, char *p_value);
void ListappendOVDint(struct List *this, int p_value);
void ListappendOVDstr(struct List *this, char *p_value);
void ListappendOVDstructListObject(struct List *this,
                                   struct ListObject p_value);
void List__reassign__(struct List *this, struct List p_list);

void ListObject__init__OVDint(struct ListObject *this, int p_value) {
  this->data.data.int_data = p_value;
  this->data.data_type = INT;
  this->next = NULL;
}

void ListObject__init__OVDstr(struct ListObject *this, char *p_value) {
  this->data.data.str_data = strdup(p_value);
  this->data.data_type = STRING;
  this->next = NULL;
}

bool ListObjectis_int(struct ListObject *this) {
  return this->data.data_type == INT;
}

int ListObjectget_int(struct ListObject *this) {
  return this->data.data.int_data;
}

bool ListObjectis_str(struct ListObject *this) {
  return this->data.data_type == STRING;
}

char *ListObjectget_str(struct ListObject *this) {
  return this->data.data.str_data;
}

void ListObject_clear_str(struct ListObject *this) {
  free(this->data.data.str_data);
}

void ListObject__del__(struct ListObject *this) {

  if (ListObjectis_str(this)) {
    ListObject_clear_str(this);
  }
}

struct ListObject ListObject_duplicate(struct ListObject *this) {
  // Perform a deep copy.
  struct ListObject copy = *this;
  if (this->data.data_type == STRING) {
    copy.data.data.str_data = strdup(this->data.data.str_data);
  }
  return copy;
}

bool ListObject__eq__OVDint(struct ListObject *this, int p_value) {

  if (ListObjectis_int(this)) {
    bool return_value = ListObjectget_int(this) == p_value;
    return return_value;
  } else {
    return false;
  }
}

bool ListObject__eq__OVDstr(struct ListObject *this, char *p_value) {

  if (ListObjectis_str(this)) {
    bool return_value = strcmp(p_value, ListObjectget_str(this)) == 0;
    return return_value;
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
  struct ListObject *current = this->head;
  while (current != NULL) {
    struct ListObject *temp = current;
    current = current->next;

    ListObject__del__(temp);
    free(temp);
  }
  this->head = NULL;
  this->tail = NULL;
  this->size = 0;
}

struct ListObject List__getitem__(struct List *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  struct ListObject *current = this->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  // Duplicate contents of node and return it.
  // If we return a reference, then the calling function will call destructor,
  // which will free the str_data causing free() errors later.
  return ListObject_duplicate(current);
}

struct ListObject Listpop(struct List *this, int index) {
  if (this->size == 0) {
    printf("List is empty. Can't pop element.\n");
    exit(EXIT_FAILURE);
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  struct ListObject *current = this->head;
  struct ListObject *previous = NULL;

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

  struct ListObject popped_node = *current;
  // Don't free current->data.str_data even though current data_type is String.
  // After copying the *pointer above, popped_node now owns
  // current->data.str_data. This avoids duplicating current->data.str_data into
  // popped_node.
  free(current);
  return popped_node;
}

bool List__contains__OVDint(struct List *this, int p_value) {
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == INT) {
      int data = current->data.data.int_data;

      if (data == p_value) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

bool List__contains__OVDstr(struct List *this, char *p_value) {
  struct ListObject *current = this->head;
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      char *data = current->data.data.str_data;

      if (strcmp(data, p_value) == 0) {
        return true;
      }
    }
    current = current->next;
  }
  return false;
}

void Listprint(struct List *this) {
  struct ListObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      printf("\"%s\"", current->data.data.str_data);
    } else {
      int data = current->data.data.int_data;

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
  struct ListObject *current = this->head;
  printf("[");
  while (current != NULL) {
    if (current->data.data_type == STRING) {
      printf("\"%s\"", current->data.data.str_data);
    } else {
      int data = current->data.data.int_data;

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

void List_insert_end(struct List *this, ListObjectptr new_node) {
  this->size++;
  if (this->head == NULL) {
    this->head = new_node;
    this->tail = new_node;
    return;
  }

  this->tail->next = new_node;
  this->tail = new_node;
}

ListObjectptr Listcreate_int_node(struct List *this, int p_value) {
  struct ListObject *new_node =
      (struct ListObject *)malloc(sizeof(struct ListObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type int for value %d.",
           p_value);
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDint(new_node, p_value);
  return new_node;
}

ListObjectptr Listcreate_string_node(struct List *this, char *p_value) {
  struct ListObject *new_node =
      (struct ListObject *)malloc(sizeof(struct ListObject));
  if (new_node == NULL) {
    printf("List : Failed to allocate a new node of type char*.");
    exit(EXIT_FAILURE);
  }
  ListObject__init__OVDstr(new_node, p_value);
  return new_node;
}

void Listappend_int(struct List *this, int p_value) {
  ListObjectptr int_node = Listcreate_int_node(this, p_value);
  List_insert_end(this, int_node);
}

void Listappend_str(struct List *this, char *p_value) {
  ListObjectptr str_node = Listcreate_string_node(this, p_value);
  List_insert_end(this, str_node);
}

void ListappendOVDint(struct List *this, int p_value) {
  Listappend_int(this, p_value);
}

void ListappendOVDstr(struct List *this, char *p_value) {
  Listappend_str(this, p_value);
}

void ListappendOVDstructListObject(struct List *this,
                                   struct ListObject p_value) {

  if (ListObjectis_int(&p_value)) {
    Listappend_int(this, ListObjectget_int(&p_value));
  } else if (ListObjectis_str(&p_value)) {
    Listappend_str(this, ListObjectget_str(&p_value));
  }
}

void List__reassign__(struct List *this, struct List p_list) {
  int tmp_len_0 = Listlen(&p_list);
  for (size_t i = 0; i < tmp_len_0; i++) {
    struct ListObject item = List__getitem__(&p_list, i);
    ListappendOVDstructListObject(this, item);
    ListObject__del__(&item);
  }
}

int main() {

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

  return 0;
}