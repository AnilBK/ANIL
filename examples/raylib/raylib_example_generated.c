///*///

// This provides bindings for raylib.h.
// https://github.com/raysan5/raylib/blob/master/LICENSE

#include "raylib.h"

typedef unsigned char u8;

///*///
#include <stdio.h>

// Partial Reimplementation of
// https://github.com/raysan5/raylib/blob/master/examples/shapes/shapes_basic_shapes.c

// Windows Compilation:
// https://github.com/raysan5/raylib/wiki/Working-on-Windows
// Open C:\raylib\w64devkit\w64devkit.exe
// ~ $ cd "E:\ANIL\examples\raylib"
// ~ $ gcc -o raylib_example.exe raylib_example_generated.c -lraylib -lgdi32
// -lwinmm ~ $ raylib_example

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

int main() {

  ///*///  main()

  struct rlColor rlGOLD;
  rlColor__init__(&rlGOLD, 255, 203, 0, 255);
  struct rlColor rlORANGE;
  rlColor__init__(&rlORANGE, 255, 161, 0, 255);
  struct rlColor rlRED;
  rlColor__init__(&rlRED, 230, 41, 55, 255);
  struct rlColor rlMAROON;
  rlColor__init__(&rlMAROON, 190, 33, 55, 255);
  struct rlColor rlGREEN;
  rlColor__init__(&rlGREEN, 0, 228, 48, 255);
  struct rlColor rlSKYBLUE;
  rlColor__init__(&rlSKYBLUE, 102, 191, 255, 255);
  struct rlColor rlDARKBLUE;
  rlColor__init__(&rlDARKBLUE, 0, 82, 172, 255);
  struct rlColor rlWHITE;
  rlColor__init__(&rlWHITE, 255, 255, 255, 255);
  struct rlColor rlBLACK;
  rlColor__init__(&rlBLACK, 0, 0, 0, 255);
  struct rlColor rlRAYWHITE;
  rlColor__init__(&rlRAYWHITE, 245, 245, 245, 255);

  int screenWidth = 800;
  int screenHeight = 600;

  struct raylib rl;

  raylibInitWindow(&rl, screenWidth, screenHeight, "raylib basic window");
  raylibSetTargetFPS(&rl, 60);

  while (raylibWindowShouldOpen(&rl)) {
    raylibBeginDrawing(&rl);
    raylibClearBackground(&rl, rlRAYWHITE);
    raylibDrawText(&rl, "It works!", 20, 20, 20, rlRED);

    raylibDrawCircle(&rl, screenWidth / 5, 120, 35, rlDARKBLUE);
    raylibDrawCircleGradient(&rl, screenWidth / 5, 220, 60, rlGREEN, rlSKYBLUE);
    raylibDrawCircleLines(&rl, screenWidth / 5, 340, 80, rlDARKBLUE);

    // Rectangle shapes and lines
    raylibDrawRectangle(&rl, screenWidth / 4 * 2 - 60, 100, 120, 60, rlRED);
    raylibDrawRectangleGradientH(&rl, screenWidth / 4 * 2 - 90, 170, 180, 130,
                                 rlMAROON, rlGOLD);
    raylibDrawRectangleLines(&rl, screenWidth / 4 * 2 - 40, 320, 80, 60,
                             rlORANGE);

    // NOTE: We draw all LINES based shapes together to optimize internal
    // drawing, this way, all LINES are rendered in a single draw pass
    raylibDrawLine(&rl, 18, 42, screenWidth - 18, 42, rlBLACK);

    raylibEndDrawing(&rl);
  }

  raylibCloseWindow(&rl);

  ///*///

  return 0;
}