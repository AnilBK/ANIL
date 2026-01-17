#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <commctrl.h>

// gcc -O2 TodoAppJSX_generated.c -o TodoAppJSX -lgdi32 -lcomctl32 -mwindows

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import Vector
import String
import UI
import File

function SaveTodos(todoList: UIWidget)
  let file = File{"todos.txt"};
  let todoItems = todoList.GetAllItemsInList()
  for todoItem in todoItems{
    file.writeline(todoItem)
  }
endfunction

function LoadTodos(todoList: UIWidget)
  let str = ""
  let storedTodos = str.readlinesFrom("todos.txt")

  if storedTodos.len() > 0{
    for todo in storedTodos{
      todoList.AddItemToList(todo)
    }
  }else{
    todoList.AddItemToList("Complete UI Framework")
    todoList.AddItemToList("Implement JSX like syntax")
    SaveTodos(todoList)
  }
endfunction

function AddTodo(userData: voidPtr)
  // 'userData' has UIElement* to the root element.
  // Convert it to UIWidget for easier access to UIWidget methods,
  // and tree traversal.
  let root = UIWidget::CreateUIWidgetFromVoidPtr(userData)

  let todoInput = root.FindElementById("todoInput")
  let todoList = root.FindElementById("todoList")

  if todoInput.isValid(){
    if todoList.isValid(){
      let text = todoInput.GetEditText()
      todoList.AddItemToList(text)
      todoInput.ClearEditText()
      SaveTodos(todoList)
    }
  }
endfunction

function DeleteSelectedTodo(userData: voidPtr)
  let root = UIWidget::CreateUIWidgetFromVoidPtr(userData)

  let todoList = root.FindElementById("todoList")

  if todoList.isValid(){
    todoList.RemoveSelectedListItem()
    SaveTodos(todoList)
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

  LoadTodos(todoList)

  let exitCode = App.Run()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}
