#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

// gcc -O2 UI_TODO_JSX_generated.c -o UI_TODO_JSX_generated -lgdi32 -lcomctl32 -mwindows

// IMPORTS //

// STRUCT_DEFINATIONS //

// HWND_VARIABLE_DECLARATIONS //

///*///
import UI

function AddTodo(userData: voidPtr)
  // 'userData' has UIElement* to the root element.
  // Convert it to UIWidget for easier access to UIWidget methods,
  // and tree traversal.
  let r1 = UIWidget{};
  let root = r1.CreateUIWidgetFromVoidPtr(userData)

  let editElement = root.FindElementById("todoInput")
  let listElement = root.FindElementById("todoList")

  if editElement.isValid(){
    if listElement.isValid(){
      let text = editElement.GetEditText()
      listElement.AddItemToList(text)
      editElement.ClearEditText()
    }
  }
endfunction

function DeleteSelectedTodo(userData: voidPtr)
  let r1 = UIWidget{};
  let root = r1.CreateUIWidgetFromVoidPtr(userData)

  let listElement = root.FindElementById("todoList")

  if listElement.isValid(){
    listElement.RemoveSelectedListItem()
  }
endfunction

///*///

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
      <Button id="addButton" onclick="AddTodo(root_elem)">"Add TODO"</Button>
      <Button id="deleteButton" onclick="DeleteSelectedTodo(root_elem)">"Delete Selected TODO"</Button>
    </HBox>
  </UI>

  // Create Windows Controls (HWNDs) for Children of Root.
  let create_status = App.CreateControls()
  if create_status == false {
    fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n", root_elem.uiElement->id);
    App.CleanUp()
    return -1
  }

  todoList.AddItemToList("Complete UI Framework")
  todoList.AddItemToList("Implement JSX like syntax")

  let exitCode = App.Run()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}
