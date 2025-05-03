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
import Vector
import String
import UI
import File

function CreateUIWidgetFromVoidPtr(ptr: voidPtr) -> UIWidget:
  // We don't have static functions in ANIL yet, so we have to do this.
  let w = UIWidget{};
  let widget = w.CreateUIWidgetFromVoidPtr(ptr)
  return widget
endfunction

function WriteTodosToFile(listElement: UIWidget)
  let file = File{"todos.txt"};

  let todoItems = listElement.GetAllItemsInList()
  for todoItem in todoItems{
    file.writeline(todoItem)
  }
endfunction

function LoadTodosFromFile(listElement: UIWidget)
  let str = ""
  let storedTodos = str.readlinesFrom("todos.txt")

  if storedTodos.len() > 0{
    // If todos.txt already exists and has some lines, then read the todos from it and add them to the list.
    for todo in storedTodos{
      listElement.AddItemToList(todo)
    }
  }else{
    // Otherwise, add some default todos and write to file.
    listElement.AddItemToList("Complete UI Framework")
    listElement.AddItemToList("Implement JSX like syntax")
    WriteTodosToFile(listElement)
  }
endfunction

function AddTodo(userData: voidPtr)
  // 'userData' has UIElement* to the root element.
  // Convert it to UIWidget for easier access to UIWidget methods,
  // and tree traversal.
  let root = CreateUIWidgetFromVoidPtr(userData)

  let editElement = root.FindElementById("todoInput")
  let listElement = root.FindElementById("todoList")

  if editElement.isValid(){
    if listElement.isValid(){
      let text = editElement.GetEditText()
      listElement.AddItemToList(text)
      editElement.ClearEditText()
      WriteTodosToFile(listElement)
    }
  }
endfunction

function DeleteSelectedTodo(userData: voidPtr)
  let root = CreateUIWidgetFromVoidPtr(userData)

  let listElement = root.FindElementById("todoList")

  if listElement.isValid(){
    listElement.RemoveSelectedListItem()
    WriteTodosToFile(listElement)
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

  LoadTodosFromFile(todoList)

  let exitCode = App.Run()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}
