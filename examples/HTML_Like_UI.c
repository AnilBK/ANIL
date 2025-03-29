#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

// gcc -O2 HTML_Like_UI_generated.c -o HTML_Like_UI_generated -lgdi32 -lcomctl32 

// IMPORTS //

// STRUCT_DEFINATIONS //

// HWND_VARIABLE_DECLARATIONS //

///*///
import Vector
import String

let todo_text = ""
let todo_text_list = Vector<String>{5};

function add_todo()
  todo_text_list.push(todo_text)
  todo_text = ""
endfunction
///*///

void DrawVectorString(HDC hdc, int x, int y, struct Vector_String *vec) {
  int yOffset = 20;
  for (size_t i = 0; i < vec->size; i++) {
    TextOutA(hdc, x, y + (int)(i * yOffset), vec->arr[i].arr,
             lstrlenA(vec->arr[i].arr));
  }
}

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

// Update a String struct with text from a text input field
void UpdateStringFromTextInput(HWND textInput, struct String *str) {
  // Get the length of text (UTF-16)
  int len = GetWindowTextLengthW(textInput);
  if (len == 0) {
    // TODO: Use utility functions from String class itself.
    free(str->arr);
    str->arr = NULL;
    str->length = 0;
    return;
  }

  // Allocate buffer for wide characters
  wchar_t *wideText = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
  GetWindowTextW(textInput, wideText, len + 1); // Retrieve text

  // Convert wide string (UTF-16) to multi-byte string (UTF-8 or ANSI)
  int charLen =
      WideCharToMultiByte(CP_UTF8, 0, wideText, -1, NULL, 0, NULL, NULL);
  free(str->arr); // Free old data if any
  str->arr = (char *)malloc(charLen);
  WideCharToMultiByte(CP_UTF8, 0, wideText, -1, str->arr, charLen, NULL, NULL);

  str->length = charLen - 1; // Exclude null terminator
  free(wideText);            // Free wide-text buffer

  printf("Updated String: %s\n", str->arr);
}

typedef void (*CallbackFunction)();

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam, UINT_PTR uIdSubclass,
                                  DWORD_PTR dwRefData) {
  switch (msg) {
  case WM_KEYDOWN:
    if (wParam == VK_RETURN) {
      // Call the function stored in dwRefData
      CallbackFunction func = (CallbackFunction)dwRefData;
      if (func){
        func(); // Execute function if valid
      }
      
      SetWindowTextW(hwnd, L"");
      InvalidateRect(GetParent(hwnd), NULL,
                     TRUE); // Trigger repaint of parent window
      return 0; // Fully handle the Enter key and prevent default processing
    }
    break;

  case WM_CHAR:
    if (wParam == VK_RETURN) { // Suppress the Enter key character
      return 0;                // Prevent beep by consuming WM_CHAR for Enter
    }
    break;
  }
  return DefSubclassProc(hwnd, msg, wParam,
                         lParam); // Default handling for other messages
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam) {
  switch (msg) {
  case WM_CREATE:
    // GUI_NODES_CREATION //
    break;

  case WM_COMMAND:

    if (HIWORD(wParam) == EN_CHANGE) { // Text input changed
      switch (LOWORD(wParam)) {
        // INPUT_FIELD_AUTO_BIND_TO_STRING //
      }
    }

    if (LOWORD(wParam) == 1000) { // Submit Button Clicked
// ASSIGN_GUI_OUTPUTS //

      // Close the window
      DestroyWindow(hwnd);
    }
    break;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    DrawVectorString(hdc, 10, 170, &todo_text_list);
    EndPaint(hwnd, &ps);
  } break;

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

  // clang-format off

  // Redirect console I/O
  RedirectIOToConsole();

  ///*///  main()

  let str1 = "Complete UI"
  todo_text_list.push(str1)

  str1 = "Design Containers"
  todo_text_list.push(str1)

  str1 = "Implement Lists"
  todo_text_list.push(str1)

    <UI>
      <Label>"Enter Todo"</Label>
      <Input @todo_text onenterkeypress=add_todo()>"Todo"</Input>
      <Button>"Add Todo"</Button>
    </UI>

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}