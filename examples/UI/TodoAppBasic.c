
// Very Basic Version of Todo App.
// Doesn't use JSX like syntax to create UI elements.
// No todo saving/loading functionality.
// Look TodoAppJSX.c for a more advanced version.

// gcc -O2 TodoAppBasic_generated.c -o TodoAppBasic -lgdi32 -lcomctl32 -mwindows

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///

import Vector
import String
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

  let ui = UIWidget{};
  let headerLabel = ui.CreateLabel(0, 0, 100, 25, "Todo Application", "headerLabel")
  let todoList = ui.CreateList(0, 0, 0, 150, "todoList")
  let inputRow = ui.CreateHBox(0, 0, 0, 30, "inputRow")
  let todoInput = ui.CreateLineInput(0, 0, 0, 0, "todoInput")
  let addButton = ui.CreateButton(0, 0, 60, 0, "Add TODO", "addButton")
  
  let root_elem = App.GetRootWidget()
  root_elem.AddChild(headerLabel)
  root_elem.AddChild(todoList)
  root_elem.AddChild(inputRow)
  inputRow.AddChild(todoInput)
  inputRow.AddChild(addButton)

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
