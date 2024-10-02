#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// IMPORTS //

// STRUCT_DEFINATIONS //

// HWND_VARIABLE_DECLARATIONS //

// Function to redirect console I/O to a console window
void RedirectIOToConsole() {
    // Allocate a console for the current process
    AllocConsole();

    // Redirect the STDOUT to the console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    // Redirect STDIN to the console
    freopen_s(&fp, "CONIN$", "r", stdin);

    // Optional: You can set the console title if you like
    SetConsoleTitle(TEXT("Console Window"));
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  switch (msg) {
    case WM_CREATE:
// GUI_NODES_CREATION //
      break;

    case WM_COMMAND:
      if (LOWORD(wParam) == 1) { // Submit Button Clicked
// ASSIGN_GUI_OUTPUTS //

        // Close the window
        DestroyWindow(hwnd);
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProcW(hwnd, msg, wParam, lParam);
  }

  return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

  // clang-format off

  // Redirect console I/O
  RedirectIOToConsole();

  ///*///  main()

  <form>
  let car_age : int = 5 ## 5, 10, 15
  let car_mileage : int = 200
  let is_electric : bool = true
  </form>

  print("Car age is {car_age} years.\n");
  print("Car mileage is {car_mileage} miles.\n");
  print("Is the car electric? {is_electric}.\n");

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}