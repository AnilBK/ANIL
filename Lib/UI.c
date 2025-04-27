// clang-format off
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>
#include <commctrl.h>

#define UIElement__MAX_CHILDREN 10
#define UIElement__MAX_TEXT_LENGTH 256
#define UIElement__DEFAULT_ITEM_HEIGHT 20 // Default height for list items
#define UIElement__MIN_LIST_HEIGHT 50     // Minimum height for the listbox

typedef enum { LABEL, BUTTON, LIST, EDIT, HORIZONTAL, VERTICAL } UIType;

struct UIElement;

// Function pointer type for event handlers
typedef void (*EventHandler)(void *);

typedef struct UIElement {
  UIType type;
  HWND hwnd; // Handle to the WinAPI control (NULL until explicitly created)
  struct UIElement *parent; // Pointer to the parent UIElement in the hierarchy
  struct UIElement *children[UIElement__MAX_CHILDREN];
  int childCount;
  int x, y, width, height;    // Desired/Current position/size
  char id[32];                // Identifier for finding elements
  char text[UIElement__MAX_TEXT_LENGTH]; // Store element text (label, button text, initial
                              // edit text)
  EventHandler onClick;       // Event handler for button clicks
  void *userData;             // Custom data for event handlers
  int itemCount;              // For list elements - track number of items
} UIElement;

typedef struct WinUIAppCoreData {
  HINSTANCE hInstance;
  HWND mainHwnd;
  UIElement *rootElement;
} WinUIAppCoreData;

typedef WinUIAppCoreData *WinUIAppCoreDataPtr;
typedef UIElement *UIElementPtr;
typedef void *voidPtr;

// Global root element for traversal
UIElement *g_rootElement = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void RefreshUILayout();
void LayoutChildren(UIElement *);
int CalculatePreferredHeight(UIElement *element);
void FreeUIElementTree(UIElement *element);

UIElement *CreateUIElement(UIType type, int x, int y, int width, int height,
                           LPCSTR initialText, LPCSTR id) {
  UIElement *element =
      (UIElement *)calloc(1, sizeof(UIElement)); // Use calloc for zero-init
  if (!element) {
    perror("Failed to allocate memory for UIElement");
    return NULL;
  }

  element->type = type;
  element->hwnd = NULL;
  element->parent = NULL;
  element->childCount = 0;
  element->x = x;
  element->y = y;
  element->width = width;
  element->height = height;
  element->onClick = NULL;
  element->userData = NULL;
  element->itemCount = 0;

  if (id) {
    strncpy(element->id, id, sizeof(element->id) - 1);
    element->id[sizeof(element->id) - 1] = '\0';
  } else {
    fprintf(stderr, "Warning: Creating UIElement with NULL id.\n");
    element->id[0] = '\0';
  }

  if (initialText) {
    strncpy(element->text, initialText, sizeof(element->text) - 1);
    element->text[sizeof(element->text) - 1] = '\0';
  } else {
    element->text[0] = '\0';
  }

  return element;
}

void AddChild(UIElement *parent, UIElement *child) {
  if (!parent || !child) {
    fprintf(stderr, "Error: AddChild called with NULL parent or child.\n");
    return;
  }
  if (parent->type != HORIZONTAL && parent->type != VERTICAL) {
    // This is okay for the root element, but maybe warn for others
    if (parent->parent != NULL) { // Don't warn for root
      fprintf(
          stderr,
          "Warning: Adding child ID '%s' to a non-container parent ID '%s'.\n",
          child->id, parent->id);
    }
  }
  if (parent->childCount >= UIElement__MAX_CHILDREN) {
    fprintf(stderr,
            "Error: Parent '%s' cannot add more children (max %d reached).\n",
            parent->id, UIElement__MAX_CHILDREN);
    return;
  }
  child->parent = parent;
  parent->children[parent->childCount++] = child;
}

// Creates the actual WinAPI HWND for a given UIElement.
bool CreateElementHwnd(UIElement *element, HWND ownerHwnd,
                       HINSTANCE hInstance) {
  if (!element) {
    fprintf(stderr, "Error: CreateElementHwnd called with NULL element.\n");
    return false;
  }
  if (element->hwnd != NULL) {
    fprintf(stderr, "Warning: HWND for element '%s' already exists.\n",
            element->id);
    return true; // Already created
  }
  // Containers don't get their own HWND in this design
  if (element->type == HORIZONTAL || element->type == VERTICAL) {
    return true; // No HWND needed
  }

  if (!ownerHwnd) {
    fprintf(stderr,
            "Error: Cannot create HWND for element '%s' (Type: %d) without a "
            "valid ownerHwnd.\n",
            element->id, element->type);
    return false;
  }
  if (!hInstance) {
    fprintf(stderr,
            "Error: Cannot create HWND for element '%s' without a valid "
            "hInstance.\n",
            element->id);
    return false;
  }

  DWORD style = WS_CHILD | WS_VISIBLE;
  DWORD exStyle = 0;
  LPCSTR className = NULL;
  // Use element's stored text as initial window text
  LPCSTR windowText = element->text;

  switch (element->type) {
  case LABEL:
    className = WC_STATIC;        // Use predefined constants
    style |= SS_LEFT | SS_NOTIFY; // SS_NOTIFY useful for potential clicks
    break;
  case BUTTON:
    className = WC_BUTTON;
    style |= BS_PUSHBUTTON;
    break;
  case LIST:
    className = WC_LISTBOX;
    style |=
        LBS_NOTIFY | WS_VSCROLL | LBS_HASSTRINGS | WS_BORDER;
    exStyle |= WS_EX_CLIENTEDGE;
    windowText = ""; // Listbox text is items, not window text
    break;
  case EDIT:
    className = WC_EDIT;
    style |= ES_AUTOHSCROLL | ES_LEFT | WS_BORDER;
    exStyle |= WS_EX_CLIENTEDGE; // Optional, WS_BORDER often enough
    break;
  default:
    fprintf(stderr,
            "Error: Unknown element type %d for HWND creation (ID: '%s').\n",
            element->type, element->id);
    return false;
  }

  // Use element's initial desired position/size (LayoutChildren will adjust)
  element->hwnd = CreateWindowEx(
      exStyle, className,
      windowText, // Use element's text
      style, element->x, element->y, element->width, element->height,
      ownerHwnd, // Parent HWND
      NULL,      // No menu / Use control ID if needed later
      // (HMENU)(UINT_PTR)element->uniqueId
      hInstance,
      NULL // No extra creation data
  );

  if (element->hwnd) {
    // Store the UIElement pointer in the control window's user data
    SetWindowLongPtr(element->hwnd, GWLP_USERDATA, (LONG_PTR)element);

    // Apply a standard GUI font
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    if (hFont) {
      SendMessage(element->hwnd, WM_SETFONT, (WPARAM)hFont,
                  MAKELPARAM(true, 0));
    }
    return true;
  } else {
    DWORD error = GetLastError();
    fprintf(stderr,
            "Error: CreateWindowEx failed for element '%s' (Type: %d) with "
            "error code %lu.\n",
            element->id, element->type, error);
    return false;
  }
}

// Recursively creates HWNDs for an element and its children.
bool CreateElementHwndRecursive(UIElement *element, HWND ownerHwnd,
                                HINSTANCE hInstance) {
  if (!element)
    return false;

  // Create HWND for the current element (if applicable)
  if (!CreateElementHwnd(element, ownerHwnd, hInstance)) {
    // CreateElementHwnd prints errors, check if HWND was actually required
    if (element->type != HORIZONTAL && element->type != VERTICAL) {
      return false; // Failed to create a required HWND
    }
    // If it was a container, failure is okay (no HWND needed), continue.
  }

  // Recursively create for children
  // The owner HWND for children is always the main application window's HWND,
  // *not* the HWND of a container element (since containers don't have HWNDs).
  for (int i = 0; i < element->childCount; ++i) {
    if (!CreateElementHwndRecursive(element->children[i], ownerHwnd,
                                    hInstance)) {
      return false;
    }
  }

  return true;
}

int CalculatePreferredHeight(UIElement *element) {
  if (element == NULL)
    return 0;
  switch (element->type) {
  case LIST: {
    int calculatedHeight = element->itemCount * UIElement__DEFAULT_ITEM_HEIGHT;
    calculatedHeight += GetSystemMetrics(SM_CYBORDER) * 2 + 4; // Padding/border
    return max(calculatedHeight, UIElement__MIN_LIST_HEIGHT);
  }
  case VERTICAL: {
    int totalHeight = 0;
    for (int i = 0; i < element->childCount; i++) {
      totalHeight += CalculatePreferredHeight(element->children[i]);
      // Add spacing if desired (e.g., + 5)
    }
    return (totalHeight > 0) ? totalHeight : element->height;
  }
  case HORIZONTAL: {
    int maxHeight = 0;
    for (int i = 0; i < element->childCount; i++) {
      maxHeight =
          max(maxHeight, CalculatePreferredHeight(element->children[i]));
    }
    return (maxHeight > 0) ? maxHeight : element->height;
  }
  case LABEL:
    return element->height > 0 ? element->height : 20;
  case BUTTON:
    return element->height > 0 ? element->height : 25;
  case EDIT:
    return element->height > 0 ? element->height : 25;
  default:
    return element->height;
  }
}

void AddItemToList(UIElement *list, LPCSTR itemText) {
  if (list == NULL || list->type != LIST || list->hwnd == NULL)
    return;
  SendMessage(list->hwnd, LB_ADDSTRING, 0, (LPARAM)itemText);
  list->itemCount++;
  RefreshUILayout();
}

void SetEventHandler(UIElement *element, EventHandler handler, void *userData) {
  if (element == NULL)
    return;
  element->onClick = handler;
  element->userData = userData;
}

char *GetEditText(UIElement *edit) {
  if (edit == NULL || edit->type != EDIT || edit->hwnd == NULL)
    return NULL;
  int length = GetWindowTextLength(edit->hwnd);
  if (length == 0) {
    char *buffer = (char *)malloc(1);
    if (buffer)
      buffer[0] = '\0';
    return buffer;
  }
  char *buffer = (char *)malloc(length + 1);
  if (!buffer)
    return NULL;
  if (GetWindowText(edit->hwnd, buffer, length + 1) > 0) {
    return buffer;
  } else {
    free(buffer);
    return NULL;
  }
}

void ClearEditText(UIElement *edit) {
  if (edit && edit->type == EDIT && edit->hwnd) {
    SetWindowText(edit->hwnd, "");
  }
}

UIElement *FindElementById(UIElement *root, const char *id) {
  if (root == NULL || id == NULL)
    return NULL;
  if (strcmp(root->id, id) == 0) {
    return root;
  }
  for (int i = 0; i < root->childCount; i++) {
    UIElement *found = FindElementById(root->children[i], id);
    if (found)
      return found;
  }
  return NULL;
}

void FreeUIElementTree(UIElement *element) {
  if (!element)
    return;
  for (int i = 0; i < element->childCount; ++i) {
    FreeUIElementTree(element->children[i]);
    element->children[i] = NULL;
  }
  // We only free the struct memory. HWNDs are destroyed by the OS when the
  // top-level window closes.
  free(element);
}

void RefreshUILayout() {
  if (g_rootElement) {
    HWND topLevelWindow = NULL;
    if (g_rootElement->hwnd) {
      topLevelWindow = GetAncestor(g_rootElement->hwnd, GA_ROOTOWNER);
    }
    if (!topLevelWindow && g_rootElement) {
      topLevelWindow = g_rootElement->hwnd;
    }

    if (topLevelWindow) {
      RECT clientRect;
      GetClientRect(topLevelWindow, &clientRect);
      g_rootElement->width = clientRect.right - clientRect.left;
      // Height is not set here.
      // LayoutChildren determines it based on children.
      LayoutChildren(g_rootElement);
      InvalidateRect(topLevelWindow, NULL, TRUE);
    } else {
      LayoutChildren(g_rootElement);
    }
  }
}

void LayoutChildren(UIElement *container) {
  if (container == NULL || container->childCount <= 0) {
    return;
  }

  if (container->type == HORIZONTAL) {
    int currentX = container->x;
    int availableWidth = container->width;
    int perChildWidth = (container->childCount > 0)
                            ? (availableWidth / container->childCount)
                            : 0;
    int remainingWidth = (container->childCount > 0)
                             ? (availableWidth % container->childCount)
                             : 0;
    int containerInnerY = container->y;
    int containerHeight = container->height;

    for (int i = 0; i < container->childCount; i++) {
      UIElement *child = container->children[i];
      if (!child)
        continue;

      int currentChildWidth = perChildWidth + (i < remainingWidth ? 1 : 0);
      int childPrefHeight =
          CalculatePreferredHeight(child); // Still useful for vertical layout

      child->x = currentX;
      child->y = containerInnerY;
      child->width = currentChildWidth;
      child->height = containerHeight; // Use container's height

      // Position the actual HWND *if it exists*
      if (child->hwnd) {
        SetWindowPos(child->hwnd, NULL, child->x, child->y, child->width,
                     child->height,
                     SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
      }
      currentX += currentChildWidth;
      if (child->type == HORIZONTAL || child->type == VERTICAL) {
        LayoutChildren(child);
      }
    }
  } else if (container->type == VERTICAL) {
    int currentY = container->y;
    int containerInnerX = container->x;
    int containerWidth = container->width;

    for (int i = 0; i < container->childCount; i++) {
      UIElement *child = container->children[i];
      if (!child)
        continue;

      int childPrefHeight = CalculatePreferredHeight(child);

      child->x = containerInnerX;
      child->y = currentY;
      child->width = containerWidth;
      child->height = childPrefHeight; // Use preferred height

      // Position the actual HWND *if it exists*
      if (child->hwnd) {
        SetWindowPos(child->hwnd, NULL, child->x, child->y, child->width,
                     child->height,
                     SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
      }
      currentY += childPrefHeight;
      if (child->type == HORIZONTAL || child->type == VERTICAL) {
        LayoutChildren(child);
      }
    }
  } else {
    // Handle non-container parents with children if necessary
    for (int i = 0; i < container->childCount; i++) {
      if (container->children[i])
        LayoutChildren(container->children[i]);
    }
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_COMMAND:
    if (HIWORD(wParam) == BN_CLICKED) {
      HWND controlHwnd = (HWND)lParam;
      UIElement *element =
          (UIElement *)GetWindowLongPtr(controlHwnd, GWLP_USERDATA);
      if (element && element->onClick) {
        element->onClick(element->userData);
      }
    } else if (HIWORD(wParam) == LBN_SELCHANGE) {
      HWND controlHwnd = (HWND)lParam;
      UIElement *element =
          (UIElement *)GetWindowLongPtr(controlHwnd, GWLP_USERDATA);
      printf("List selection changed on: %s\n",
             element ? element->id : "UNKNOWN");
    }
    break;
  case WM_SIZE:
    if (g_rootElement) {
      g_rootElement->width = LOWORD(lParam);
      g_rootElement->height = HIWORD(lParam);
      g_rootElement->x = 0;
      g_rootElement->y = 0;
      LayoutChildren(g_rootElement);
      // Invalidate might still be needed if layout doesn't perfectly cover all
      // cases
      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
  case WM_GETMINMAXINFO: {
    MINMAXINFO *pMMI = (MINMAXINFO *)lParam;
    pMMI->ptMinTrackSize.x = 300;
    pMMI->ptMinTrackSize.y = 250;
  }
    return 0;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

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

///*///

struct VoidPointer{void* ptr};

struct UIWidget { UIElement *uiElement };

namespace VoidPointer
c_function __init__(payload:UIWidget)
  // Create a void* from UIWidget.
  // Used by 'SetOnClickCallback' of UIWidget.
  this->ptr = (void*) payload.uiElement;
endc_function
endnamespace

namespace UIWidget

c_function CreateUIWidgetFromVoidPtr(ptr: voidPtr) -> UIWidget:
  // FIXME: This is like a static function.
  // TODO: Have to do this because global c_functions dont have c function bodies.
  UIElement* element = (UIElement*)ptr;
  if (element == NULL) {
    fprintf(stderr, "Error: Could not create UIWidget from a void*.\n");
    exit(-1);
  }

  struct UIWidget w;
  w.uiElement = element;
  return w;
endc_function

c_function isValid() -> bool:
  if (this->uiElement == NULL) {
    fprintf(stderr, "Error: UIWidget is not valid (NULL uiElement).\n");
    return false;
  }
  return true;
endc_function

c_function FindElementById(id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = NULL;

  if (this->uiElement == NULL || id == NULL){
    fprintf(stderr, "Error: FindElementById called with NULL element or id.\n");
    return w;
  }

  UIElement *found = FindElementById(this->uiElement, id);
  if (found) {
    w.uiElement = found;
  } else {
    fprintf(stderr, "Warning: Element with ID '%s' not found.\n", id);
  }
  return w;
endc_function

c_function SetOnClickCallback(onClickFn:Fn(voidPtr)->void, payload:VoidPointer)
  this->uiElement->onClick = onClickFn;
  this->uiElement->userData = payload.ptr;
endc_function

c_function AddChild(p_child:UIWidget)
  UIElement* parent = this->uiElement;
  UIElement* child = p_child.uiElement;

  if (!parent || !child) {
    fprintf(stderr, "Error: AddChild called with NULL parent or child.\n");
    return;
  }

  if (parent->type != HORIZONTAL && parent->type != VERTICAL) {
    // This is okay for the root element, but maybe warn for others
    if (parent->parent != NULL) { // Don't warn for root
      fprintf(
          stderr,
          "Warning: Adding child ID '%s' to a non-container parent ID '%s'.\n",
          child->id, parent->id);
    }
  }

  if (parent->childCount >= UIElement__MAX_CHILDREN) {
    fprintf(stderr,
            "Error: Parent '%s' cannot add more children (max %d reached).\n",
            parent->id, UIElement__MAX_CHILDREN);
    return;
  }

  child->parent = parent;
  parent->children[parent->childCount++] = child;
endc_function

c_function CreateLabel(x:int, y:int, width:int, height:int, initialText:str, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(LABEL, x, y, width, height, (LPCSTR)initialText, (LPCSTR)id);
  return w;  
endc_function

c_function CreateList(x:int, y:int, width:int, height:int, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(LIST, x, y, width, height, (LPCSTR)"", (LPCSTR)id);
  return w;
endc_function

c_function CreateLineInput(x:int, y:int, width:int, height:int, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(EDIT, x, y, width, height, (LPCSTR)"", (LPCSTR)id);
  return w;
endc_function

c_function CreateButton(x:int, y:int, width:int, height:int, initialText:str, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(BUTTON, x, y, width, height, (LPCSTR)initialText, (LPCSTR)id);
  return w;
endc_function

c_function CreateVBox(x:int, y:int, width:int, height:int, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(VERTICAL, x, y, width, height, (LPCSTR)"", (LPCSTR)id);
  return w;
endc_function

c_function CreateHBox(x:int, y:int, width:int, height:int, id:str) -> UIWidget:
  struct UIWidget w;
  w.uiElement = CreateUIElement(HORIZONTAL, x, y, width, height, (LPCSTR)"", (LPCSTR)id);
  return w;
endc_function

c_function AddItemToList(itemText:str)
  if (this->uiElement->type != LIST) {
    fprintf(stderr, "Error: AddItemToList called on non-list element.\n");
    return;
  }

  if (!itemText) {
    fprintf(stderr, "Error: AddItemToList called with NULL itemText.\n");
    return;
  }

  if (strlen(itemText) == 0) {
    return;
  }

  AddItemToList(this->uiElement, itemText);
endc_function

c_function ClearEditText()
  if (this->uiElement->type != EDIT) {
    fprintf(stderr, "Error: ClearEditText called on non-edit element.\n");
    return;
  }
  ClearEditText(this->uiElement);
endc_function

c_function GetEditText() -> str:
  if (this->uiElement->type != EDIT) {
    fprintf(stderr, "Error: GetEditText called on non-edit element.\n");
    return NULL;
  }
  char *text = GetEditText(this->uiElement);
  return text;
  // TODO: This should be freed, so use ANILs string.
endc_function

c_function RemoveSelectedListItem()
  if (this->uiElement->type != LIST) {
    fprintf(stderr, "Error: RemoveSelectedListItem called on non-list element.\n");
    return;
  }

  int selectedIndex = SendMessage(this->uiElement->hwnd, LB_GETCURSEL, 0, 0);
  
  if (selectedIndex != LB_ERR) {
    SendMessage(this->uiElement->hwnd, LB_DELETESTRING, selectedIndex, 0);
    this->uiElement->itemCount--;
    RefreshUILayout();
  } else {
    fprintf(stderr, "Error: No item selected in the list.\n");
  }
endc_function

c_function GetTotalItemsInList() -> int:
  if (this->uiElement->type != LIST) {
    fprintf(stderr, "Error: GetTotalItemsInList called on non-list element.\n");
    return 0;
  }
  // return this->uiElement->itemCount;

  int itemCount = SendMessage(this->uiElement->hwnd, LB_GETCOUNT, 0, 0);
  if (itemCount == LB_ERR) {
    fprintf(stderr, "Error: Failed to get listbox item count.\n");
    return 0;
  }
  return itemCount;
endc_function

c_function GetListItemAtIndex(index:int) -> String:
  struct String text;

  bool success = false;
  if (this->uiElement->type != LIST) {
    fprintf(stderr, "Error: GetListItemAtIndex called on non-list element.\n");
  }

  if (index < 0 || index >= this->uiElement->itemCount) {
    fprintf(stderr, "Error: Index out of bounds for list element.\n");
  }

  char *itemText = (char *)malloc(UIElement__MAX_TEXT_LENGTH);
  if (!itemText) {
    fprintf(stderr, "Error: Failed to allocate memory for item text.\n");
  }else{
    success = true;
    SendMessage(this->uiElement->hwnd, LB_GETTEXT, index, (LPARAM)itemText);
  }

  if(success){
    // Use of mangled function directly.
    String__init__from_charptr(&text, itemText, strlen(itemText));
    free(itemText);
  } else{
    // Initialize an empty string if allocation failed or other error occurred.
    String__init__OVDstr(&text, "");
  }

  return text;
endc_function

function GetAllItemsInList() -> Vector<String>:
  // Returns a vector of strings containing all items in the list.
  let total_items = this.GetTotalItemsInList()
  let result = Vector<String>{total_items};

  for i in range(0..total_items){
    let item = this.GetListItemAtIndex(i)
    if item.len() > 0 {
      result.push(item)
    }
  }
  return result
endfunction

endnamespace


struct WinUIAppConfig{HINSTANCE hInstance, int nCmdShow};

struct WinUIApp{WinUIAppConfig winConfig, WinUIAppCoreData* appCoreData};

namespace WinUIApp
c_function GetRootWidget() -> UIWidget:
  struct UIWidget w;
  w.uiElement = this->appCoreData->rootElement;
  return w;
endc_function

// Initializes the main window, creates the root UI element, and sets up the
// WinUIAppCoreData structure.
c_function _InitializeMainWindow(p_title:str, width:int, height:int) -> WinUIAppCoreDataPtr:
  LPCSTR title = (LPCSTR)p_title;
  
  HINSTANCE hInstance = this->winConfig.hInstance;
  int nCmdShow = this->winConfig.nCmdShow;

  const char CLASS_NAME[] = "ANILUIWindowClass";

  // Register window class
  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.style = CS_HREDRAW | CS_VREDRAW; // Redraw on size change
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);

  if (!RegisterClass(&wc)) {
    MessageBox(NULL, "Window Registration Failed!", "Error!",
              MB_ICONEXCLAMATION | MB_OK);
    return NULL;
  }

  // Create main window
  HWND hwnd =
      CreateWindowEx(0,                   // Optional window styles.
                     CLASS_NAME,          // Window class
                     title,               // Window text
                     WS_OVERLAPPEDWINDOW, // Window style

                     // Position and size
                     CW_USEDEFAULT, CW_USEDEFAULT, width, height,

                     NULL,      // Parent window
                     NULL,      // Menu
                     hInstance, // Instance handle
                     NULL
      );

  if (hwnd == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
               MB_ICONEXCLAMATION | MB_OK);
    return NULL;
  }

  WinUIAppCoreData *appCoreData = (WinUIAppCoreData *)calloc(1, sizeof(WinUIAppCoreData));
  if (!appCoreData) {
    perror("Failed to allocate memory for WinUIAppCoreData");
    DestroyWindow(hwnd);
    return NULL;
  }

  appCoreData->hInstance = hInstance;
  appCoreData->mainHwnd = hwnd;

  // Create the root UI element (typically a vertical container)
  // Its x,y are 0,0 relative to the client area. Width/height will be set by
  // WM_SIZE.
  appCoreData->rootElement = CreateUIElement(VERTICAL, 0, 0, 0, 0, "", "root");
  if (!appCoreData->rootElement) {
    fprintf(stderr, "Error: Failed to create root UI element.\n");
    free(appCoreData);
    DestroyWindow(hwnd);
    return NULL;
  }
  // Important: Associate the main window's HWND with the root element
  // This allows layout to use it as the base container.
  appCoreData->rootElement->hwnd = hwnd;

  g_rootElement = appCoreData->rootElement;

  // Store pointer to our WinUIAppCoreData structure in the window's user data area.
  // This allows WndProc to access the application state.
  // It's CRUCIAL this happens AFTER creating the root element and assigning g_rootElement
  // if WndProc needs g_rootElement immediately (e.g., an early WM_SIZE).
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)appCoreData);

  return appCoreData;
endc_function

c_function Create(winConfig:WinUIAppConfig, title:str) -> int:
  this->winConfig = winConfig;
  this->appCoreData = WinUIApp_InitializeMainWindow(this, title, 500, 450);
  if (this->appCoreData == NULL) {
    return -1; // Initialization failed
  }
  return 1;
endc_function

c_function CreateControls() -> bool:
  // Create Windows Controls (HWNDs) for Children of Root
  WinUIAppCoreData *appCoreData = this->appCoreData;
  bool hwndsCreatedSuccessfully = true;
  // Iterate through the direct children of the root element
  for (int i = 0; i < appCoreData->rootElement->childCount; ++i) {
      // Start the recursive creation from each child.
      // The owner HWND for all these controls is the main window.
      if (!CreateElementHwndRecursive(appCoreData->rootElement->children[i],
                                      appCoreData->mainHwnd, // Owner is the main window
                                      appCoreData->hInstance)) {
          hwndsCreatedSuccessfully = false;
          fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n",
                  appCoreData->rootElement->children[i]->id);
          break; // Stop on the first failure
      }
  }

  return hwndsCreatedSuccessfully;
endc_function

c_function _Show()
  ShowWindow(this->appCoreData->mainHwnd, this->winConfig.nCmdShow);
endc_function

c_function _Update()
  UpdateWindow(this->appCoreData->mainHwnd);  // Sends initial WM_PAINT
endc_function

c_function _RunMessageLoop() -> int:
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    // Check > 0 for messages, 0 for WM_QUIT, -1 for error
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
endc_function

function Run() -> int:
  this._Show()
  this._Update()
  return this._RunMessageLoop()
endfunction

c_function CleanUp()
  WinUIAppCoreData *appCoreData = this->appCoreData;
  if (!appCoreData) {
    return;
  }

  // Free the UI element tree (structs)
  if (appCoreData->rootElement) {
    FreeUIElementTree(appCoreData->rootElement);
    appCoreData->rootElement = NULL;
  }

  // HWND is destroyed by the OS when PostQuitMessage is processed or
  // DestroyWindow called. No need to call DestroyWindow here typically, unless
  // cleanup is forced before loop ends.

  // Free the WinUIAppCoreData struct itself
  free(appCoreData);
endc_function

function __del__()
  this.CleanUp()
endfunction
endnamespace
///*///