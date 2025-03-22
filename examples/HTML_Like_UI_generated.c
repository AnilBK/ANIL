#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// IMPORTS //

HWND hStaticLabel1, hStaticLabel2, hStaticButton1, hTextInput1, hSubmitButton;
struct Form1Output {
  char __dummy;
} Form1Output;

// Function to redirect console I/O to a console window
void RedirectIOToConsole() {
  // Allocate a console for the current process
  AllocConsole();

  // Redirect the STDOUT to the console
  FILE *fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);

  // Redirect STDIN to the console
  freopen_s(&fp, "CONIN$", "r", stdin);

  // Optional: You can set the console title if you like
  SetConsoleTitle(TEXT("Console Window"));
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam) {
  switch (msg) {
  case WM_CREATE:
    // Static Label 1
    hStaticLabel1 = CreateWindowW(L"Static", L"Label 1", WS_VISIBLE | WS_CHILD,
                                  10, 10, 200, 25, hwnd, NULL, NULL, NULL);
    // Static Label 2
    hStaticLabel2 = CreateWindowW(L"Static", L"Label 2", WS_VISIBLE | WS_CHILD,
                                  10, 50, 200, 25, hwnd, NULL, NULL, NULL);
    // Text Input Field
    hTextInput1 = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 90, 280, 24,
        hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
    // Static Button 1
    hStaticButton1 =
        CreateWindowW(L"Button", L"Button", WS_VISIBLE | WS_CHILD, 10, 130, 100,
                      25, hwnd, (HMENU)2001, NULL, NULL);
    hSubmitButton =
        CreateWindowW(L"Button", L"Submit", WS_VISIBLE | WS_CHILD, 10, 170, 100,
                      25, hwnd, (HMENU)1000, NULL, NULL);
    break;

  case WM_COMMAND:
    if (LOWORD(wParam) == 1000) { // Submit Button Clicked

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

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args,
                   int ncmdshow) {

  // Redirect console I/O
  RedirectIOToConsole();

  ///*///  main()

  WNDCLASSW wc = {0};
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hInstance = hInst;
  wc.lpszClassName = L"FormWindow";
  wc.lpfnWndProc = WindowProcedure;

  if (!RegisterClassW(&wc)) {
    return -1;
  }

  CreateWindowW(L"FormWindow", L"Form", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100,
                100, 400, 270, NULL, NULL, NULL, NULL);

  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  ///*///

  return 0;
}