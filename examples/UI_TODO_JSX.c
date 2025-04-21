#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

// gcc -O2 UI_TODO_JSX_generated.c -o UI_TODO_JSX_generated -lgdi32 -lcomctl32 -mwindows

// IMPORTS //

// STRUCT_DEFINATIONS //

// HWND_VARIABLE_DECLARATIONS //

void AddTodoHandler(UIElement *button, void *userData) {
  // Cast userData back to the expected type (the root UIElement)
  UIElement *root = (UIElement *)userData; 
  if (!root) {
    fprintf(stderr, "Error: AddTodoHandler called with NULL userData (root).\n");
    return;
  }

  // Find elements starting from the provided root context
  UIElement *editElement = FindElementById(root, "todoInput");
  UIElement *listElement = FindElementById(root, "todoList");

  if (editElement && listElement) {
    char *text = GetEditText(editElement);
    if (text) {
      if (strlen(text) > 0) {
        AddItemToList(listElement, text);
        ClearEditText(editElement);
      }
      free(text);
    } else {
        fprintf(stderr, "Error: GetEditText failed for 'todoInput'.\n");
    }
  } else {
    if (!editElement) fprintf(stderr, "Error: Could not find element 'todoInput' from root passed to AddTodoHandler.\n");
    if (!listElement) fprintf(stderr, "Error: Could not find element 'todoList' from root passed to AddTodoHandler.\n");
  }
}

///*///
import UI

function AddTodo(button: UIElementPtr, userData: voidPtr)
  AddTodoHandler(button, userData);
endfunction
///*///

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
    // DrawVectorString(hdc, 10, 170, &todo_text_list);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
  // clang-format off

  // RedirectIOToConsole();

  ///*///  main()
      
  // hInstance and nCmdShow are C types above and not registered in ANIL yet,
  // so we use a struct to pass them.
  let AppConfig = WinUIAppConfig{hInstance, nCmdShow};

  let App = WinUIApp{};
  let result_code : int = App.Create(AppConfig, "Todo Application")
  if result_code == -1 {
    // Initialization failed
    return -1; 
  }

  // JSX like syntax to create UI elements.
  // Alternative, for UI_TODO_App.c.

  <UI>
    <App id="_" name="App" rootElement="root_elem"></App>
    <Label id="headerLabel">"Todo Application"</Label>
    <List id="todoList"></List>
    <HBox id="inputRow">
      <Input id="todoInput"></Input>
      <Button id="addButton">"Add TODO"</Button>
    </HBox>
  </UI>

  // Create Windows Controls (HWNDs) for Children of Root.
  let create_status = App.CreateControls()
  if create_status == false {
    fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n", root_elem.uiElement->id);
    App.CleanUp()
    return -1
  }

  // Setup Event Handlers.
  // Pass the root element as userData so the handler can find other elements
  let payload = VoidPointer{root_elem};
  addButton.SetOnClickCallback(AddTodo, payload)

  todoList.AddItemToList("Complete UI Framework")
  todoList.AddItemToList("Implement JSX like syntax")

  let exitCode = App.Run()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}
