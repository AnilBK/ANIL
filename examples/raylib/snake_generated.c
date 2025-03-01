///*///

// This provides bindings for raylib.h.
// https://github.com/raysan5/raylib/blob/master/LICENSE

#include "raylib.h"

typedef unsigned char u8;

///*///
///*///
///////////////////////////////////////////
#include <stdlib.h>
#include <time.h>

///*///

///*///

#include <string.h>

///////////////////////////////////////////

///*///

///*///

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

///*///

#include <stdio.h>
#include <stdlib.h>

// https://github.com/raysan5/raylib/wiki/Working-on-Windows
// Open  C:\raylib\w64devkit\w64devkit.exe
// ~ $ cd "E:\ANIL\examples\raylib"
// ~ $ gcc -o snake.exe snake_generated.c -lraylib -lgdi32 -lwinmm
// ~ $ snake

// IMPORTS //

struct rlColor {
  Color color;
};

struct rlVector2 {
  Vector2 vector2;
};

struct raylib {
  char dummy;
};

struct Random {
  char dummy;
};

struct String {
  char *arr;
  int length;
  int capacity;
};

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

struct Food {
  struct rlVector2 position;
};

struct Vector_rlVector2 {
  struct rlVector2 *arr;
  int size;
  int capacity;
};

struct Snake {
  struct rlVector2 position;
  struct rlVector2 speed;
  struct Vector_rlVector2 body;
};

void rlColor__init__(struct rlColor *this, u8 r, u8 g, u8 b, u8 a);

void rlVector2__init__OVDfloatfloat(struct rlVector2 *this, float x, float y);
void rlVector2__init__OVDintint(struct rlVector2 *this, int x, int y);
void rlVector2__add__(struct rlVector2 *this, struct rlVector2 p_vec);
void rlVector2__reassign__(struct rlVector2 *this, struct rlVector2 p_vec);
bool rlVector2__eq__(struct rlVector2 *this, struct rlVector2 p_vector);
float rlVector2get_x(struct rlVector2 *this);
float rlVector2get_y(struct rlVector2 *this);
int rlVector2get_x_int(struct rlVector2 *this);
int rlVector2get_y_int(struct rlVector2 *this);
void rlVector2set_int(struct rlVector2 *this, int x, int y);
void rlVector2translate_i(struct rlVector2 *this, struct rlVector2 v);

void raylibInitWindow(struct raylib *this, int screenWidth, int screenHeight,
                      char *title);
bool raylibWindowShouldOpen(struct raylib *this);
void raylibCloseWindow(struct raylib *this);
void raylibClearBackground(struct raylib *this, struct rlColor color);
void raylibBeginDrawing(struct raylib *this);
void raylibEndDrawing(struct raylib *this);
void raylibSetTargetFPS(struct raylib *this, int fps);
bool raylibIsKeyPressed(struct raylib *this, int key);
void raylibDrawLine(struct raylib *this, int startPosX, int startPosY,
                    int endPosX, int endPosY, struct rlColor color);
void raylibDrawLineV(struct raylib *this, struct rlVector2 startPos,
                     struct rlVector2 endPos, struct rlColor color);
void raylibDrawCircle(struct raylib *this, int centerX, int centerY,
                      float radius, struct rlColor color);
void raylibDrawCircleGradient(struct raylib *this, int centerX, int centerY,
                              float radius, struct rlColor color1,
                              struct rlColor color2);
void raylibDrawCircleLines(struct raylib *this, int centerX, int centerY,
                           float radius, struct rlColor color);
void raylibDrawRectangle(struct raylib *this, int posX, int posY, int width,
                         int height, struct rlColor color);
void raylibDrawRectangleV(struct raylib *this, struct rlVector2 position,
                          struct rlVector2 size, struct rlColor color);
void raylibDrawRectangleGradientV(struct raylib *this, int posX, int posY,
                                  int width, int height, struct rlColor color1,
                                  struct rlColor color2);
void raylibDrawRectangleGradientH(struct raylib *this, int posX, int posY,
                                  int width, int height, struct rlColor color1,
                                  struct rlColor color2);
void raylibDrawRectangleLines(struct raylib *this, int posX, int posY,
                              int width, int height, struct rlColor color);
void raylibDrawTriangle(struct raylib *this, struct rlVector2 v1,
                        struct rlVector2 v2, struct rlVector2 v3,
                        struct rlColor color);
void raylibDrawTriangleLines(struct raylib *this, struct rlVector2 v1,
                             struct rlVector2 v2, struct rlVector2 v3,
                             struct rlColor color);
void raylibDrawPoly(struct raylib *this, struct rlVector2 center, int sides,
                    float radius, float rotation, struct rlColor color);
void raylibDrawPolyLines(struct raylib *this, struct rlVector2 center,
                         int sides, float radius, float rotation,
                         struct rlColor color);
void raylibDrawPolyLinesEx(struct raylib *this, struct rlVector2 center,
                           int sides, float radius, float rotation,
                           float lineThick, struct rlColor color);
void raylibDrawText(struct raylib *this, char *text, int x, int y,
                    int font_size, struct rlColor color);

void Random__init__(struct Random *this);
int Randomrandrange(struct Random *this, int upper_limit);

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstructString(struct String *this, struct String text);
void Stringclear(struct String *this);
void Stringprint(struct String *this);
void StringprintLn(struct String *this);
void String__del__(struct String *this);
bool Stringstartswith(struct String *this, char *prefix);
struct String Stringsubstr(struct String *this, int start, int length);
struct String Stringstrip(struct String *this);
struct Vector_String Stringsplit(struct String *this, char delimeter);
bool String__contains__(struct String *this, char *substring);
bool String__eq__(struct String *this, char *pstring);
void String__add__(struct String *this, char *pstring);
void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length);
void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring);
void String__reassign__OVDstr(struct String *this, char *pstring);
void Stringset_to_file_contents(struct String *this, char *pfilename);
struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename);

void Food__init__(struct Food *this, struct rlVector2 p_position);

void Snake__init__(struct Snake *this, struct rlVector2 p_position,
                   struct rlVector2 p_speed);
void Snake__del__(struct Snake *this);
bool Snakeis_touching_itself(struct Snake *this);
bool Snakeate_food(struct Snake *this, struct Food p_food);
void Snakemove_body(struct Snake *this);
bool Snakeis_out_of_bounds(struct Snake *this, int GRID_WIDTH, int GRID_HEIGHT);

struct rlVector2 new_food_spawn(struct Snake snake, int grid_width,
                                int grid_height);
size_t Vector_Stringlen(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String__del__(struct Vector_String *this);
void Vector_Stringpush(struct Vector_String *this, struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_Stringremove_at(struct Vector_String *this, int index);
void Vector_Stringclear(struct Vector_String *this);
bool Vector_String__contains__(struct Vector_String *this, struct String value);
void Vector_Stringprint(struct Vector_String *this);
size_t Vector_rlVector2len(struct Vector_rlVector2 *this);
struct rlVector2 Vector_rlVector2__getitem__(struct Vector_rlVector2 *this,
                                             int index);
void Vector_rlVector2__setitem__(struct Vector_rlVector2 *this, int index,
                                 struct rlVector2 value);
void Vector_rlVector2__init__(struct Vector_rlVector2 *this, int capacity);
void Vector_rlVector2__del__(struct Vector_rlVector2 *this);
void Vector_rlVector2push(struct Vector_rlVector2 *this,
                          struct rlVector2 value);
void Vector_rlVector2allocate_more(struct Vector_rlVector2 *this, int n);
void Vector_rlVector2push_unchecked(struct Vector_rlVector2 *this,
                                    struct rlVector2 value);
struct rlVector2 Vector_rlVector2pop(struct Vector_rlVector2 *this);
void Vector_rlVector2remove_at(struct Vector_rlVector2 *this, int index);
void Vector_rlVector2clear(struct Vector_rlVector2 *this);
bool Vector_rlVector2__contains__(struct Vector_rlVector2 *this,
                                  struct rlVector2 value);
void Vector_rlVector2print(struct Vector_rlVector2 *this);

void rlColor__init__(struct rlColor *this, u8 r, u8 g, u8 b, u8 a) {
  this->color = (Color){r, g, b, a};
}

void rlVector2__init__OVDfloatfloat(struct rlVector2 *this, float x, float y) {
  this->vector2 = (Vector2){x, y};
}

void rlVector2__init__OVDintint(struct rlVector2 *this, int x, int y) {
  this->vector2 = (Vector2){(float)x, (float)y};
}

void rlVector2__add__(struct rlVector2 *this, struct rlVector2 p_vec) {
  this->vector2.x = this->vector2.x + p_vec.vector2.x;
  this->vector2.y = this->vector2.y + p_vec.vector2.y;
}

void rlVector2__reassign__(struct rlVector2 *this, struct rlVector2 p_vec) {
  this->vector2 = p_vec.vector2;
}

bool rlVector2__eq__(struct rlVector2 *this, struct rlVector2 p_vector) {
  return this->vector2.x == p_vector.vector2.x &&
         this->vector2.y == p_vector.vector2.y;
}

float rlVector2get_x(struct rlVector2 *this) { return this->vector2.x; }

float rlVector2get_y(struct rlVector2 *this) { return this->vector2.y; }

int rlVector2get_x_int(struct rlVector2 *this) { return (int)this->vector2.x; }

int rlVector2get_y_int(struct rlVector2 *this) { return (int)this->vector2.y; }

void rlVector2set_int(struct rlVector2 *this, int x, int y) {
  this->vector2.x = (float)x;
  this->vector2.y = (float)y;
}

void rlVector2translate_i(struct rlVector2 *this, struct rlVector2 v) {
  this->vector2.x = (int)this->vector2.x + (int)v.vector2.x;
  this->vector2.y = (int)this->vector2.y + (int)v.vector2.y;
}

void raylibInitWindow(struct raylib *this, int screenWidth, int screenHeight,
                      char *title) {
  InitWindow(screenWidth, screenHeight, title);
}

bool raylibWindowShouldOpen(struct raylib *this) {
  return !WindowShouldClose();
}

void raylibCloseWindow(struct raylib *this) { CloseWindow(); }

void raylibClearBackground(struct raylib *this, struct rlColor color) {
  ClearBackground(color.color);
}

void raylibBeginDrawing(struct raylib *this) { BeginDrawing(); }

void raylibEndDrawing(struct raylib *this) { EndDrawing(); }

void raylibSetTargetFPS(struct raylib *this, int fps) { SetTargetFPS(fps); }

bool raylibIsKeyPressed(struct raylib *this, int key) {
  return IsKeyPressed(key);
}

void raylibDrawLine(struct raylib *this, int startPosX, int startPosY,
                    int endPosX, int endPosY, struct rlColor color) {
  DrawLine(startPosX, startPosY, endPosX, endPosY, color.color);
}

void raylibDrawLineV(struct raylib *this, struct rlVector2 startPos,
                     struct rlVector2 endPos, struct rlColor color) {
  DrawLineV(startPos.vector2, endPos.vector2, color.color);
}

void raylibDrawCircle(struct raylib *this, int centerX, int centerY,
                      float radius, struct rlColor color) {
  DrawCircle(centerX, centerY, radius, color.color);
}

void raylibDrawCircleGradient(struct raylib *this, int centerX, int centerY,
                              float radius, struct rlColor color1,
                              struct rlColor color2) {
  DrawCircleGradient(centerX, centerY, radius, color1.color, color2.color);
}

void raylibDrawCircleLines(struct raylib *this, int centerX, int centerY,
                           float radius, struct rlColor color) {
  DrawCircleLines(centerX, centerY, radius, color.color);
}

void raylibDrawRectangle(struct raylib *this, int posX, int posY, int width,
                         int height, struct rlColor color) {
  DrawRectangle(posX, posY, width, height, color.color);
}

void raylibDrawRectangleV(struct raylib *this, struct rlVector2 position,
                          struct rlVector2 size, struct rlColor color) {
  DrawRectangleV(position.vector2, size.vector2, color.color);
}

void raylibDrawRectangleGradientV(struct raylib *this, int posX, int posY,
                                  int width, int height, struct rlColor color1,
                                  struct rlColor color2) {
  DrawRectangleGradientV(posX, posY, width, height, color1.color, color2.color);
}

void raylibDrawRectangleGradientH(struct raylib *this, int posX, int posY,
                                  int width, int height, struct rlColor color1,
                                  struct rlColor color2) {
  DrawRectangleGradientH(posX, posY, width, height, color1.color, color2.color);
}

void raylibDrawRectangleLines(struct raylib *this, int posX, int posY,
                              int width, int height, struct rlColor color) {
  DrawRectangleLines(posX, posY, width, height, color.color);
}

void raylibDrawTriangle(struct raylib *this, struct rlVector2 v1,
                        struct rlVector2 v2, struct rlVector2 v3,
                        struct rlColor color) {
  DrawTriangle(v1.vector2, v2.vector2, v3.vector2, color.color);
}

void raylibDrawTriangleLines(struct raylib *this, struct rlVector2 v1,
                             struct rlVector2 v2, struct rlVector2 v3,
                             struct rlColor color) {
  DrawTriangleLines(v1.vector2, v2.vector2, v3.vector2, color.color);
}

void raylibDrawPoly(struct raylib *this, struct rlVector2 center, int sides,
                    float radius, float rotation, struct rlColor color) {
  DrawPoly(center.vector2, sides, radius, rotation, color.color);
}

void raylibDrawPolyLines(struct raylib *this, struct rlVector2 center,
                         int sides, float radius, float rotation,
                         struct rlColor color) {
  DrawPolyLines(center.vector2, sides, radius, rotation, color.color);
}

void raylibDrawPolyLinesEx(struct raylib *this, struct rlVector2 center,
                           int sides, float radius, float rotation,
                           float lineThick, struct rlColor color) {
  DrawPolyLinesEx(center.vector2, sides, radius, rotation, lineThick,
                  color.color);
}

void raylibDrawText(struct raylib *this, char *text, int x, int y,
                    int font_size, struct rlColor color) {
  DrawText(text, x, y, font_size, color.color);
}

void Random__init__(struct Random *this) { srand(time(0)); }

int Randomrandrange(struct Random *this, int upper_limit) {
  return rand() % upper_limit;
}

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

size_t Stringlength_of_charptr(struct String *this, char *p_string) {
  // This should be some kind of static method.
  return strlen(p_string);
}

void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length) {
  // p_text_length : Length of the string without the null terminator.
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strncpy(this->arr, text, p_text_length);
  this->arr[p_text_length] = '\0';

  this->length = p_text_length;
  this->capacity = p_text_length + 1;
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstructString(struct String *this, struct String text) {
  size_t p_text_length = Stringlen(&text);
  String__init__from_charptr(this, Stringc_str(&text), p_text_length);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) { free(this->arr); }

bool Stringstartswith(struct String *this, char *prefix) {
  return strncmp(this->arr, prefix, strlen(prefix)) == 0;
}

struct String Stringsubstr(struct String *this, int start, int length) {
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;
}

struct String Stringstrip(struct String *this) {
  char *begin = this->arr;
  char *end = begin + Stringlen(this) - 1;

  // Remove leading whitespaces.
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces.
  while (end > begin && isspace(*end)) {
    end--;
  }

  // Length of the substring between 'begin' and 'end' inclusive.
  int new_length = end - begin + 1;

  struct String text;
  String__init__from_charptr(&text, begin, new_length);
  return text;
}

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // NOTE : Because of this function, before import String, we require import
  // Vector.
  struct Vector_String split_result;
  Vector_String__init__(&split_result, 2);

  int index = 0;
  int segment_start = 0;

  size_t tmp_len_0 = Stringlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char character = String__getitem__(this, i);

    if (character == delimeter) {

      if (segment_start < index) {
        struct String segment =
            Stringsubstr(this, segment_start, index - segment_start);
        Vector_Stringpush(&split_result, segment);
        String__del__(&segment);
      }
      segment_start = index + 1;
    }
    index = index + 1;
  }

  if (segment_start < Stringlen(this)) {
    struct String remaining_segment =
        Stringsubstr(this, segment_start, Stringlen(this) - segment_start);
    Vector_Stringpush(&split_result, remaining_segment);
    String__del__(&remaining_segment);
  }

  return split_result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = this->length + strlen(pstring) + 1;

  if (new_length > this->capacity) {
    size_t new_capacity;
    if (this->capacity == 0) {
      new_capacity = new_length * 2;
    } else {
      new_capacity = this->capacity;
      while (new_capacity <= new_length) {
        new_capacity *= 2;
      }
    }
    this->arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
    this->capacity = new_capacity;
  }

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
  this->length = new_length;
}

void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length) {
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
}

void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring) {
  char *src = Stringc_str(&pstring);
  size_t p_text_length = Stringlen(&pstring);
  Stringreassign_internal(this, src, p_text_length);
}

void String__reassign__OVDstr(struct String *this, char *pstring) {
  size_t p_text_length = Stringlength_of_charptr(this, pstring);
  Stringreassign_internal(this, pstring, p_text_length);
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  // Read from the file & store the contents to this string.

  // TODO: Use CPL to generate this, because the function below is a mangled
  // function name.
  Stringclear(this);

  FILE *ptr;

  ptr = fopen(pfilename, "r");

  if (ptr == NULL) {
    printf("File \"%s\" couldn't be opened.\n", pfilename);
    exit(0);
  }

  char myString[256];
  while (fgets(myString, 256, ptr)) {
    String__add__(this, myString);
  }

  fclose(ptr);
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

void Food__init__(struct Food *this, struct rlVector2 p_position) {
  rlVector2__reassign__(&this->position, p_position);
}

void Snake__init__(struct Snake *this, struct rlVector2 p_position,
                   struct rlVector2 p_speed) {
  rlVector2__reassign__(&this->position, p_position);
  rlVector2__reassign__(&this->speed, p_speed);
  Vector_rlVector2__init__(&this->body, 10);
}

void Snake__del__(struct Snake *this) { Vector_rlVector2__del__(&this->body); }

bool Snakeis_touching_itself(struct Snake *this) {
  struct Vector_rlVector2 body_ref = this->body;

  size_t tmp_len_1 = Vector_rlVector2len(&body_ref);
  for (size_t i = 1; i < tmp_len_1; i++) {
    struct rlVector2 body_pos = Vector_rlVector2__getitem__(&body_ref, i);

    if (rlVector2__eq__(&this->position, body_pos)) {
      return true;
    }
  }
  return false;
}

bool Snakeate_food(struct Snake *this, struct Food p_food) {
  bool return_value = rlVector2__eq__(&this->position, p_food.position);
  return return_value;
}

void Snakemove_body(struct Snake *this) {
  for (size_t i = Vector_rlVector2len(&this->body) - 1; i >= 1; i += -1) {
    struct rlVector2 left_pos = Vector_rlVector2__getitem__(&this->body, i - 1);
    Vector_rlVector2__setitem__(&this->body, i, left_pos);
  }
}

bool Snakeis_out_of_bounds(struct Snake *this, int GRID_WIDTH,
                           int GRID_HEIGHT) {

  if (rlVector2get_x_int(&this->position) < 0) {
    return true;
  } else if (rlVector2get_x_int(&this->position) >= GRID_WIDTH) {
    return true;
  } else if (rlVector2get_y_int(&this->position) < 0) {
    return true;
  } else if (rlVector2get_y_int(&this->position) >= GRID_HEIGHT) {
    return true;
  }
  return false;
}

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value) {
  if (index < 0) {
    index += this->size;
  }

  String__del__(&this->arr[index]);

  struct String str;
  String__init__OVDstructString(&str, value);

  this->arr[index] = str;
}

void Vector_String__init__(struct Vector_String *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct String *)malloc(capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_String__del__(struct Vector_String *this) {
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }

  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  struct String str;
  String__init__OVDstructString(&str, value);

  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct String *)realloc(this->arr, this->capacity *
                                                        sizeof(struct String));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = str;
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  this->capacity += n;
  this->arr = (struct String *)realloc(this->arr,
                                       this->capacity * sizeof(struct String));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
  this->size--;
}

void Vector_Stringclear(struct Vector_String *this) {
  for (size_t i = 0; i < this->size; ++i) {
    String__del__(&this->arr[i]);
  }

  free(this->arr);

  this->capacity = 1;
  this->arr = (struct String *)malloc(this->capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_3 = Vector_Stringlen(this);
  for (size_t h = 0; h < tmp_len_3; h++) {
    struct String string = Vector_String__getitem__(this, h);

    if (Stringlen(&string) == Stringlen(&value)) {

      if (String__eq__(&string, Stringc_str(&value))) {
        return true;
      }
    }
  }
  return false;
}

void Vector_Stringprint(struct Vector_String *this) {
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"", this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

size_t Vector_rlVector2len(struct Vector_rlVector2 *this) { return this->size; }

struct rlVector2 Vector_rlVector2__getitem__(struct Vector_rlVector2 *this,
                                             int index) {
  if (index < 0) {
    index += this->size;
  }
  return *(this->arr + index);
}

void Vector_rlVector2__setitem__(struct Vector_rlVector2 *this, int index,
                                 struct rlVector2 value) {
  if (index < 0) {
    index += this->size;
  }
  // FIXME: If previous value is a struct with destructor, then that destructor
  // should be called. This is fixed in the next overloaded function for String
  // class.
  this->arr[index] = value;
}

void Vector_rlVector2__init__(struct Vector_rlVector2 *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct rlVector2 *)malloc(capacity * sizeof(struct rlVector2));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_rlVector2__del__(struct Vector_rlVector2 *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_rlVector2push(struct Vector_rlVector2 *this,
                          struct rlVector2 value) {
  if (this->size == this->capacity) {
    this->capacity *= 2;
    this->arr = (struct rlVector2 *)realloc(
        this->arr, this->capacity * sizeof(struct rlVector2));
    if (this->arr == NULL) {
      fprintf(stderr, "Memory reallocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
  this->arr[this->size++] = value;
}

void Vector_rlVector2allocate_more(struct Vector_rlVector2 *this, int n) {
  this->capacity += n;
  this->arr = (struct rlVector2 *)realloc(
      this->arr, this->capacity * sizeof(struct rlVector2));
  if (this->arr == NULL) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }
}

void Vector_rlVector2push_unchecked(struct Vector_rlVector2 *this,
                                    struct rlVector2 value) {
  this->arr[this->size++] = value;
}

struct rlVector2 Vector_rlVector2pop(struct Vector_rlVector2 *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_rlVector2remove_at(struct Vector_rlVector2 *this, int index) {
  if (index < 0) {
    index += this->size;
  }

  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
  this->size--;
}

void Vector_rlVector2clear(struct Vector_rlVector2 *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr =
      (struct rlVector2 *)malloc(this->capacity * sizeof(struct rlVector2));

  if (this->arr == NULL) {
    fprintf(stderr, "clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

bool Vector_rlVector2__contains__(struct Vector_rlVector2 *this,
                                  struct rlVector2 value) {
  size_t tmp_len_4 = Vector_rlVector2len(this);
  for (size_t h = 0; h < tmp_len_4; h++) {
    struct rlVector2 vec = Vector_rlVector2__getitem__(this, h);

    if (rlVector2__eq__(&vec, value)) {
      return true;
    }
  }
  return false;
}

void Vector_rlVector2print(struct Vector_rlVector2 *this) {
  // Default overload.
  printf("Dynamic Array (size = %d, capacity = %d) : [ ]", this->size,
         this->capacity);
  // struct rlVector2 will be replaced by the actual templated data type.
  printf("Unknown Format Specifier for type struct rlVector2.\n");
}

///*///

struct rlVector2 new_food_spawn(struct Snake snake, int grid_width,
                                int grid_height) {
  struct Random rng;
  Random__init__(&rng);

  while (true) {
    int new_x = Randomrandrange(&rng, grid_width);
    int new_y = Randomrandrange(&rng, grid_height);
    struct rlVector2 new_pos;
    rlVector2__init__OVDintint(&new_pos, new_x, new_y);

    if (!Vector_rlVector2__contains__(&snake.body, new_pos)) {
      return new_pos;
    }
  }

  // Infinite loop above or should never reach here.
  struct rlVector2 res;
  rlVector2__init__OVDintint(&res, 0, 0);
  return res;
}
///*///

int main() {

  ///*///  main()

  struct rlColor rlRED;
  rlColor__init__(&rlRED, 230, 41, 55, 255);
  struct rlColor rlGREEN;
  rlColor__init__(&rlGREEN, 0, 228, 48, 255);
  struct rlColor rlRAYWHITE;
  rlColor__init__(&rlRAYWHITE, 245, 245, 245, 255);
  struct rlColor rlBLACK;
  rlColor__init__(&rlBLACK, 0, 0, 0, 255);

  int rlKEY_UP = 265;
  int rlKEY_DOWN = 264;
  int rlKEY_RIGHT = 262;
  int rlKEY_LEFT = 263;
  int rlKEY_R = 82;

  int CELL_SIZE = 20;
  int GRID_WIDTH = 25;
  int GRID_HEIGHT = 25;

  int w_x = GRID_WIDTH * CELL_SIZE;
  int w_y = GRID_HEIGHT * CELL_SIZE;

  int mid_x = GRID_WIDTH / 2;
  int mid_y = GRID_HEIGHT / 2;

  struct rlVector2 pos;
  rlVector2__init__OVDintint(&pos, mid_x, mid_y);
  struct rlVector2 speed;
  rlVector2__init__OVDintint(&speed, 1, 0);

  struct Snake snake;
  Snake__init__(&snake, pos, speed);
  Vector_rlVector2push(&snake.body, pos);

  struct rlVector2 new_food_pos =
      new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT);
  struct Food food;
  Food__init__(&food, new_food_pos);

  bool gameOver = false;

  int score = 0;
  struct String scoreText;
  String__init__OVDstr(&scoreText, "Score: 0        ");
  //                       ^^^^^^^^^ These spaces act as a buffer where sprintf
  //                                 can write the score digits.

  struct raylib rl;
  raylibInitWindow(&rl, w_x, w_y, "Snake Game");
  raylibSetTargetFPS(&rl, 10);

  while (raylibWindowShouldOpen(&rl)) {
    raylibBeginDrawing(&rl);

    if (gameOver) {
      raylibClearBackground(&rl, rlRAYWHITE);
      raylibDrawText(&rl, "Game Over! Press R to Restart", 100, 200, 20, rlRED);

      if (raylibIsKeyPressed(&rl, rlKEY_R)) {
        gameOver = false;

        rlVector2set_int(&snake.speed, 1, 0);
        rlVector2set_int(&snake.position, mid_x, mid_y);

        Vector_rlVector2clear(&snake.body);
        Vector_rlVector2push(&snake.body, snake.position);

        rlVector2__reassign__(&food.position,
                              new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT));

        score = 0;
        char *score_ptr = Stringc_str(&scoreText);
        sprintf(score_ptr, "Score: %d", score);
      }
    } else {
      raylibClearBackground(&rl, rlBLACK);

      // Update game.

      if (raylibIsKeyPressed(&rl, rlKEY_UP)) {

        if (rlVector2get_y(&snake.speed) == 0) {
          rlVector2set_int(&snake.speed, 0, -1);
        }
      }

      if (raylibIsKeyPressed(&rl, rlKEY_DOWN)) {

        if (rlVector2get_y(&snake.speed) == 0) {
          rlVector2set_int(&snake.speed, 0, 1);
        }
      }

      if (raylibIsKeyPressed(&rl, rlKEY_LEFT)) {

        if (rlVector2get_x(&snake.speed) == 0) {
          rlVector2set_int(&snake.speed, -1, 0);
        }
      }

      if (raylibIsKeyPressed(&rl, rlKEY_RIGHT)) {

        if (rlVector2get_x(&snake.speed) == 0) {
          rlVector2set_int(&snake.speed, 1, 0);
        }
      }

      Snakemove_body(&snake);

      // Update head position.
      rlVector2translate_i(&snake.position, snake.speed);
      Vector_rlVector2__setitem__(&snake.body, 0, snake.position);

      // Check for wall collisions.

      if (Snakeis_out_of_bounds(&snake, GRID_WIDTH, GRID_HEIGHT)) {
        gameOver = true;
      }

      if (Snakeis_touching_itself(&snake)) {
        gameOver = true;
      }

      if (Snakeate_food(&snake, food)) {
        struct rlVector2 last_pos =
            Vector_rlVector2__getitem__(&snake.body, -1);
        rlVector2__add__(&last_pos, snake.speed);

        Vector_rlVector2push(&snake.body, last_pos);

        rlVector2__reassign__(&food.position,
                              new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT));

        score = score + 1;
        char *score_ptr = Stringc_str(&scoreText);
        sprintf(score_ptr, "Score: %d", score);
      }

      // Draw food.
      int fx = rlVector2get_x_int(&food.position) * CELL_SIZE;
      int fy = rlVector2get_y_int(&food.position) * CELL_SIZE;

      raylibDrawRectangle(&rl, fx, fy, CELL_SIZE, CELL_SIZE, rlRED);

      // Draw snake
      size_t tmp_len_2 = Vector_rlVector2len(&snake.body);
      for (size_t i = 0; i < tmp_len_2; i++) {
        struct rlVector2 body = Vector_rlVector2__getitem__(&snake.body, i);
        int x = rlVector2get_x_int(&body) * CELL_SIZE;
        int y = rlVector2get_y_int(&body) * CELL_SIZE;

        raylibDrawRectangle(&rl, x, y, CELL_SIZE, CELL_SIZE, rlGREEN);
      }
    }

    raylibDrawText(&rl, Stringc_str(&scoreText), 10, 20, 20, rlRED);

    raylibEndDrawing(&rl);
  }

  raylibCloseWindow(&rl);

  String__del__(&scoreText);
  Snake__del__(&snake);
  ///*///

  return 0;
}