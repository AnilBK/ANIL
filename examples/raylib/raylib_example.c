#include <stdio.h>

// Partial Reimplementation of 
// https://github.com/raysan5/raylib/blob/master/examples/shapes/shapes_basic_shapes.c

// Windows Compilation:
// https://github.com/raysan5/raylib/wiki/Working-on-Windows
// Open C:\raylib\w64devkit\w64devkit.exe
// ~ $ cd "E:\ANIL\examples\raylib"
// ~ $ gcc -o raylib_example.exe raylib_example_generated.c -lraylib -lgdi32 -lwinmm
// ~ $ raylib_example

// IMPORTS //

// STRUCT_DEFINATIONS //

int main() {

  // clang-format off

  ///*///  main()

  import raylib

  let rlGOLD       = rlColor{ 255, 203, 0, 255 };  
  let rlORANGE     = rlColor{ 255, 161, 0, 255 };  
  let rlRED        = rlColor{ 230, 41, 55, 255 };  
  let rlMAROON     = rlColor{ 190, 33, 55, 255 };  
  let rlGREEN      = rlColor{ 0, 228, 48, 255 };   
  let rlSKYBLUE    = rlColor{ 102, 191, 255, 255 };
  let rlDARKBLUE   = rlColor{ 0, 82, 172, 255 };   
  let rlWHITE      = rlColor{ 255, 255, 255, 255 };
  let rlBLACK      = rlColor{ 0, 0, 0, 255 };      
  let rlRAYWHITE   = rlColor{ 245, 245, 245, 255 };

  let screenWidth : int = 800
  let screenHeight : int = 600

  let rl = raylib{};

  rl.InitWindow(screenWidth, screenHeight, "raylib basic window")
  rl.SetTargetFPS(60)

  while rl.WindowShouldOpen(){
    rl.BeginDrawing()
    rl.ClearBackground(rlRAYWHITE)
    rl.DrawText("It works!", 20, 20, 20, rlRED)

    rl.DrawCircle(screenWidth/5, 120, 35, rlDARKBLUE)
    rl.DrawCircleGradient(screenWidth/5, 220, 60, rlGREEN, rlSKYBLUE)
    rl.DrawCircleLines(screenWidth/5, 340, 80, rlDARKBLUE)

    // Rectangle shapes and lines
    rl.DrawRectangle(screenWidth/4*2 - 60, 100, 120, 60, rlRED)
    rl.DrawRectangleGradientH(screenWidth/4*2 - 90, 170, 180, 130, rlMAROON, rlGOLD)
    rl.DrawRectangleLines(screenWidth/4*2 - 40, 320, 80, 60, rlORANGE)

    // NOTE: We draw all LINES based shapes together to optimize internal drawing,
    // this way, all LINES are rendered in a single draw pass
    rl.DrawLine(18, 42, screenWidth - 18, 42, rlBLACK)
    
    rl.EndDrawing()
  }

  rl.CloseWindow()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}