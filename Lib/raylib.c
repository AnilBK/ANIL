// clang-format off
///*///

// This provides bindings for raylib.h.
// https://github.com/raysan5/raylib/blob/master/LICENSE

#include "raylib.h"

typedef unsigned char u8;

struct rlColor{Color color};
namespace rlColor
c_function __init__(r : u8, g : u8, b : u8, a : u8)
  this->color = (Color){r, g, b, a};
endc_function
endnamespace

struct rlVector2{Vector2 vector2};
namespace rlVector2
c_function __init__<>(x : float, y : float)
  this->vector2 = (Vector2){x, y};
endc_function

c_function __init__<>(x : int, y : int)
  this->vector2 = (Vector2){(float)x, (float)y};
endc_function

c_function __add__(p_vec : rlVector2)
  this->vector2.x = this->vector2.x + p_vec.vector2.x;
  this->vector2.y = this->vector2.y + p_vec.vector2.y;
endc_function

c_function __reassign__(p_vec : rlVector2)
  this->vector2 = p_vec.vector2;
endc_function

c_function __eq__(p_vector : rlVector2) -> bool:
  return this->vector2.x == p_vector.vector2.x && this->vector2.y == p_vector.vector2.y;
endc_function

c_function get_x() -> float:
  return this->vector2.x;
endc_function

c_function get_y() -> float:
  return this->vector2.y;
endc_function

c_function get_x_int() -> int:
  return (int)this->vector2.x;
endc_function

c_function get_y_int() -> int:
  return (int)this->vector2.y;
endc_function

c_function set_int(x : int, y : int)
  this->vector2.x = (float)x;
  this->vector2.y = (float)y;
endc_function

c_function translate_i(v : rlVector2)
  this->vector2.x = (int)this->vector2.x + (int)v.vector2.x;
  this->vector2.y = (int)this->vector2.y + (int)v.vector2.y;
endc_function
endnamespace

struct raylib{char dummy};
namespace raylib
// Window-related functions
c_function InitWindow(screenWidth : int, screenHeight : int, title : str)
  InitWindow(screenWidth, screenHeight, title);
endc_function

c_function WindowShouldOpen() -> bool:
  return !WindowShouldClose();
endc_function

c_function CloseWindow()
  CloseWindow();
endc_function

// Drawing-related functions
c_function ClearBackground(color : rlColor)
  ClearBackground(color.color);
endc_function

c_function BeginDrawing()
  BeginDrawing();
endc_function

c_function EndDrawing()
  EndDrawing();
endc_function

// Timing-related functions
c_function SetTargetFPS(fps : int)
  SetTargetFPS(fps);
endc_function

// Input-related functions: keyboard
c_function IsKeyPressed(key : int) -> bool:
  return IsKeyPressed(key);
endc_function

// Basic shapes drawing functions
c_function DrawLine(startPosX : int, startPosY : int, endPosX : int, endPosY : int, color : rlColor)
  DrawLine(startPosX, startPosY, endPosX, endPosY, color.color);
endc_function

c_function DrawLineV(startPos : rlVector2, endPos : rlVector2, color : rlColor)
  DrawLineV(startPos.vector2, endPos.vector2, color.color);
endc_function

c_function DrawCircle(centerX : int, centerY : int, radius : float, color : rlColor)
  DrawCircle(centerX, centerY, radius, color.color);
endc_function

c_function DrawCircleGradient(centerX : int, centerY : int, radius : float, color1 : rlColor, color2 : rlColor)
  DrawCircleGradient(centerX, centerY, radius, color1.color, color2.color);
endc_function

c_function DrawCircleLines(centerX : int, centerY : int, radius : float, color : rlColor)
  DrawCircleLines(centerX, centerY, radius, color.color);
endc_function

c_function DrawRectangle(posX : int, posY : int, width : int, height : int, color : rlColor)
  DrawRectangle(posX, posY, width, height, color.color);
endc_function

c_function DrawRectangleV(position : rlVector2, size : rlVector2, color : rlColor)
  DrawRectangleV(position.vector2, size.vector2, color.color);
endc_function

c_function DrawRectangleGradientV(posX : int, posY : int, width : int, height : int, color1 : rlColor, color2 : rlColor)
  DrawRectangleGradientV(posX, posY, width, height, color1.color, color2.color);
endc_function

c_function DrawRectangleGradientH(posX : int, posY : int, width : int, height : int, color1 : rlColor, color2 : rlColor)
  DrawRectangleGradientH(posX, posY, width, height, color1.color, color2.color);
endc_function

c_function DrawRectangleLines(posX : int, posY : int, width : int, height : int, color : rlColor)
  DrawRectangleLines(posX, posY, width, height, color.color);
endc_function

c_function DrawTriangle(v1 : rlVector2, v2 : rlVector2, v3 : rlVector2, color : rlColor)
  DrawTriangle(v1.vector2, v2.vector2, v3.vector2, color.color);
endc_function

c_function DrawTriangleLines(v1 : rlVector2, v2 : rlVector2, v3 : rlVector2, color : rlColor)
  DrawTriangleLines(v1.vector2, v2.vector2, v3.vector2, color.color);
endc_function

c_function DrawPoly(center : rlVector2, sides : int, radius : float, rotation : float, color : rlColor)
  DrawPoly(center.vector2, sides, radius, rotation, color.color);
endc_function

c_function DrawPolyLines(center : rlVector2, sides : int, radius : float, rotation : float, color : rlColor)
  DrawPolyLines(center.vector2, sides, radius, rotation, color.color);
endc_function

c_function DrawPolyLinesEx(center : rlVector2, sides : int, radius : float, rotation : float, lineThick : float, color : rlColor)
  DrawPolyLinesEx(center.vector2, sides, radius, rotation, lineThick, color.color);
endc_function

// Text drawing functions
c_function DrawText(text : str, x : int, y : int, font_size : int, color : rlColor)
  DrawText(text, x, y, font_size, color.color);
endc_function
endnamespace
///*///
