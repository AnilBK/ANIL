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
};

struct List {
  struct ListObject *items;
  int size;
  int capacity;
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
void List_ensure_capacity(struct List *this, int min_capacity);
struct ListObject List__getitem__(struct List *this, int index);
struct ListObject List__getitem_ref__(struct List *this, int index);
void List_shift_left_from(struct List *this, int index);
struct ListObject Listpop(struct List *this, int index);
bool List__contains__OVDint(struct List *this, int p_value);
bool List__contains__OVDstr(struct List *this, char *p_value);
void Listprint(struct List *this);
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
}

void ListObject__init__OVDstr(struct ListObject *this, char *p_value) {
  this->data.data.str_data = strdup(p_value);
  this->data.data_type = STRING;
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
  this->items = NULL;
  this->size = 0;
  this->capacity = 0;
}

void List__del__(struct List *this) {
  for (int i = 0; i < this->size; i++) {
    ListObject__del__(&this->items[i]);
  }
  free(this->items);
  this->items = NULL;
  this->size = 0;
  this->capacity = 0;
}

void List_ensure_capacity(struct List *this, int min_capacity) {
  if (this->capacity >= min_capacity) {
    return;
  }

  int new_capacity = this->capacity == 0 ? 4 : this->capacity * 2;
  if (new_capacity < min_capacity) {
    new_capacity = min_capacity;
  }

  this->items = realloc(this->items, new_capacity * sizeof(struct ListObject));
  if (this->items == NULL) {
    printf("List: Failed to reallocate array to capacity %d.\n", new_capacity);
    exit(EXIT_FAILURE);
  }
  this->capacity = new_capacity;
}

struct ListObject List__getitem__(struct List *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  return ListObject_duplicate(&this->items[index]);
}

struct ListObject List__getitem_ref__(struct List *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    printf("Index %d out of bounds(max : %d).\n", index, this->size - 1);
    exit(EXIT_FAILURE);
  }

  return this->items[index];
}

void List_shift_left_from(struct List *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->items[i] = this->items[i + 1];
  }
}

struct ListObject Listpop(struct List *this, int index) {
  // Duplicate the item.
  // We cant return a reference to the popped item, as it will be freed when the
  // list is modified (e.g when a new item is appended or another item is
  // popped).
  struct ListObject item = List__getitem__(this, index);

  List_shift_left_from(this, index);
  this->size -= 1;
  return item;
}

bool List__contains__OVDint(struct List *this, int p_value) {
  int tmp_len_0 = Listlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    struct ListObject item = List__getitem_ref__(this, i);

    if (ListObject__eq__OVDint(&item, p_value)) {
      return true;
    }
  }
  return false;
}

bool List__contains__OVDstr(struct List *this, char *p_value) {
  int tmp_len_1 = Listlen(this);
  for (size_t i = 0; i < tmp_len_1; i++) {
    struct ListObject item = List__getitem_ref__(this, i);

    if (ListObject__eq__OVDstr(&item, p_value)) {
      return true;
    }
  }
  return false;
}

void Listprint(struct List *this) {
  printf("[");
  for (int i = 0; i < this->size; i++) {
    if (this->items[i].data.data_type == STRING) {
      printf("\"%s\"", this->items[i].data.data.str_data);
    } else {
      int data = this->items[i].data.data.int_data;

      //@hook_begin("custom_integer_printer" "int" data)
      printf("%d", data);
      //@hook_end
    }

    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

typedef void (*custom_integer_printer)(int);
void Listprint_hooked_custom_integer_printer(
    struct List *this, custom_integer_printer p_custom_integer_printer) {
  printf("[");
  for (int i = 0; i < this->size; i++) {
    if (this->items[i].data.data_type == STRING) {
      printf("\"%s\"", this->items[i].data.data.str_data);
    } else {
      int data = this->items[i].data.data.int_data;

      //
      p_custom_integer_printer(data);
    }

    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

void Listappend_int(struct List *this, int p_value) {
  List_ensure_capacity(this, this->size + 1);
  ListObject__init__OVDint(&this->items[this->size], p_value);
  this->size++;
}

void Listappend_str(struct List *this, char *p_value) {
  List_ensure_capacity(this, this->size + 1);
  ListObject__init__OVDstr(&this->items[this->size], p_value);
  this->size++;
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
  List_ensure_capacity(this, this->size + p_list.size);
  int tmp_len_2 = Listlen(&p_list);
  for (size_t i = 0; i < tmp_len_2; i++) {
    struct ListObject item = List__getitem_ref__(&p_list, i);
    ListappendOVDstructListObject(this, item);
  }
}

int main() {

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

  struct ListObject node = Listpop(&test_list, 7);

  Listprint(&test_list);

  if (List__contains__OVDint(&test_list, 10)) {
    printf("10 is in the list.\n\n");
  }

  if (!List__contains__OVDint(&test_list, 5)) {
    printf("5 is not in the list.\n\n");
  }

  printf("Duplicating List: \n");
  struct List test_list2;
  List__init__(&test_list2);
  List__reassign__(&test_list2, test_list);
  Listprint(&test_list2);

  List__del__(&test_list2);
  ListObject__del__(&node);
  List__del__(&test_list);

  return 0;
}