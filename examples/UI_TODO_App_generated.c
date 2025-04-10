#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>

#define UIElement__MAX_CHILDREN 10
#define UIElement__MAX_TEXT_LENGTH 256
#define UIElement__DEFAULT_ITEM_HEIGHT 20 // Default height for list items
#define UIElement__MIN_LIST_HEIGHT 50     // Minimum height for the listbox

typedef enum { LABEL, BUTTON, LIST, EDIT, HORIZONTAL, VERTICAL } UIType;

struct UIElement;

// Function pointer type for event handlers
typedef void (*EventHandler)(struct UIElement *, void *);

typedef struct UIElement {
  UIType type;
  HWND hwnd; // Handle to the WinAPI control (NULL until explicitly created)
  struct UIElement *parent; // Pointer to the parent UIElement in the hierarchy
  struct UIElement *children[UIElement__MAX_CHILDREN];
  int childCount;
  int x, y, width, height;               // Desired/Current position/size
  char id[32];                           // Identifier for finding elements
  char text[UIElement__MAX_TEXT_LENGTH]; // Store element text (label, button
                                         // text, initial
  // edit text)
  EventHandler onClick; // Event handler for button clicks
  void *userData;       // Custom data for event handlers
  int itemCount;        // For list elements - track number of items
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
    style |= LBS_NOTIFY | WS_VSCROLL | LBS_HASSTRINGS | WS_BORDER;
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
  if (g_rootElement) {
    HWND topLevelWindow = NULL;
    if (g_rootElement->hwnd)
      topLevelWindow = GetAncestor(g_rootElement->hwnd, GA_ROOTOWNER);
    if (!topLevelWindow && g_rootElement)
      topLevelWindow = g_rootElement->hwnd;

    if (topLevelWindow) {
      RECT clientRect;
      GetClientRect(topLevelWindow, &clientRect);
      g_rootElement->width = clientRect.right - clientRect.left;
      // Maybe don't adjust height here, let layout handle it
      LayoutChildren(g_rootElement);
      InvalidateRect(topLevelWindow, NULL, true);
    } else {
      LayoutChildren(g_rootElement);
    }
  }
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
        element->onClick(element, element->userData);
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
///*///

// Initializes the main window, creates the root UI element, and sets up the
// WinUIAppCoreData structure.

///*///#include <stdbool.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// gcc -O2 UI_TODO_App_generated.c -o UI_TODO_App_generated -lgdi32 -lcomctl32
// -mwindows

// IMPORTS //

struct VoidPointer {
  void *ptr;
};

struct UIWidget {
  UIElement *uiElement;
};

struct WinUIAppConfig {
  HINSTANCE hInstance;
  int nCmdShow;
};

struct WinUIApp {
  struct WinUIAppConfig winConfig;
  WinUIAppCoreData *appCoreData;
};

void VoidPointer__init__(struct VoidPointer *this, struct UIWidget payload);

void UIWidgetSetOnClickCallback(struct UIWidget *this,
                                void (*onClickFn)(UIElementPtr, voidPtr),
                                struct VoidPointer payload);
void UIWidgetAddChild(struct UIWidget *this, struct UIWidget p_child);
struct UIWidget UIWidgetCreateLabel(struct UIWidget *this, int x, int y,
                                    int width, int height, char *initialText,
                                    char *id);
struct UIWidget UIWidgetCreateList(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
struct UIWidget UIWidgetCreateLineInput(struct UIWidget *this, int x, int y,
                                        int width, int height, char *id);
struct UIWidget UIWidgetCreateButton(struct UIWidget *this, int x, int y,
                                     int width, int height, char *initialText,
                                     char *id);
struct UIWidget UIWidgetCreateVBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
struct UIWidget UIWidgetCreateHBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
void UIWidgetAddItemToList(struct UIWidget *this, char *itemText);

struct UIWidget WinUIAppGetRootWidget(struct WinUIApp *this);
WinUIAppCoreDataPtr WinUIApp_InitializeMainWindow(struct WinUIApp *this,
                                                  char *p_title, int width,
                                                  int height);
int WinUIAppCreate(struct WinUIApp *this, struct WinUIAppConfig winConfig,
                   char *title);
bool WinUIAppCreateControls(struct WinUIApp *this);
void WinUIApp_Show(struct WinUIApp *this);
void WinUIApp_Update(struct WinUIApp *this);
int WinUIApp_RunMessageLoop(struct WinUIApp *this);
int WinUIAppRun(struct WinUIApp *this);
void WinUIAppCleanUp(struct WinUIApp *this);
void WinUIApp__del__(struct WinUIApp *this);

void AddTodo(UIElementPtr button, voidPtr userData);

void VoidPointer__init__(struct VoidPointer *this, struct UIWidget payload) {
  // Create a void* from UIWidget.
  // Used by 'SetOnClickCallback' of UIWidget.
  this->ptr = (void *)payload.uiElement;
}

void UIWidgetSetOnClickCallback(struct UIWidget *this,
                                void (*onClickFn)(UIElementPtr, voidPtr),
                                struct VoidPointer payload) {
  this->uiElement->onClick = onClickFn;
  this->uiElement->userData = payload.ptr;
}

void UIWidgetAddChild(struct UIWidget *this, struct UIWidget p_child) {
  UIElement *parent = this->uiElement;
  UIElement *child = p_child.uiElement;

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

struct UIWidget UIWidgetCreateLabel(struct UIWidget *this, int x, int y,
                                    int width, int height, char *initialText,
                                    char *id) {
  struct UIWidget w;
  w.uiElement = CreateUIElement(LABEL, x, y, width, height, (LPCSTR)initialText,
                                (LPCSTR)id);
  return w;
}

struct UIWidget UIWidgetCreateList(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement =
      CreateUIElement(LIST, x, y, width, height, (LPCSTR) "", (LPCSTR)id);
  return w;
}

struct UIWidget UIWidgetCreateLineInput(struct UIWidget *this, int x, int y,
                                        int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement =
      CreateUIElement(EDIT, x, y, width, height, (LPCSTR) "", (LPCSTR)id);
  return w;
}

struct UIWidget UIWidgetCreateButton(struct UIWidget *this, int x, int y,
                                     int width, int height, char *initialText,
                                     char *id) {
  struct UIWidget w;
  w.uiElement = CreateUIElement(BUTTON, x, y, width, height,
                                (LPCSTR)initialText, (LPCSTR)id);
  return w;
}

struct UIWidget UIWidgetCreateVBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement =
      CreateUIElement(VERTICAL, x, y, width, height, (LPCSTR) "", (LPCSTR)id);
  return w;
}

struct UIWidget UIWidgetCreateHBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement =
      CreateUIElement(HORIZONTAL, x, y, width, height, (LPCSTR) "", (LPCSTR)id);
  return w;
}

void UIWidgetAddItemToList(struct UIWidget *this, char *itemText) {
  if (this->uiElement->type != LIST) {
    fprintf(stderr, "Error: AddItemToList called on non-list element.\n");
    return;
  }
  AddItemToList(this->uiElement, itemText);
}

struct UIWidget WinUIAppGetRootWidget(struct WinUIApp *this) {
  struct UIWidget w;
  w.uiElement = this->appCoreData->rootElement;
  return w;
}

WinUIAppCoreDataPtr WinUIApp_InitializeMainWindow(struct WinUIApp *this,
                                                  char *p_title, int width,
                                                  int height) {
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
  HWND hwnd = CreateWindowEx(0,                   // Optional window styles.
                             CLASS_NAME,          // Window class
                             title,               // Window text
                             WS_OVERLAPPEDWINDOW, // Window style

                             // Position and size
                             CW_USEDEFAULT, CW_USEDEFAULT, width, height,

                             NULL,      // Parent window
                             NULL,      // Menu
                             hInstance, // Instance handle
                             NULL);

  if (hwnd == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
               MB_ICONEXCLAMATION | MB_OK);
    return NULL;
  }

  WinUIAppCoreData *appCoreData =
      (WinUIAppCoreData *)calloc(1, sizeof(WinUIAppCoreData));
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

  // Store pointer to our WinUIAppCoreData structure in the window's user data
  // area. This allows WndProc to access the application state. It's CRUCIAL
  // this happens AFTER creating the root element and assigning g_rootElement if
  // WndProc needs g_rootElement immediately (e.g., an early WM_SIZE).
  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)appCoreData);

  return appCoreData;
}

int WinUIAppCreate(struct WinUIApp *this, struct WinUIAppConfig winConfig,
                   char *title) {
  this->winConfig = winConfig;
  this->appCoreData = WinUIApp_InitializeMainWindow(this, title, 500, 450);
  if (this->appCoreData == NULL) {
    return -1; // Initialization failed
  }
  return 1;
}

bool WinUIAppCreateControls(struct WinUIApp *this) {
  // Create Windows Controls (HWNDs) for Children of Root
  WinUIAppCoreData *appCoreData = this->appCoreData;
  bool hwndsCreatedSuccessfully = true;
  // Iterate through the direct children of the root element
  for (int i = 0; i < appCoreData->rootElement->childCount; ++i) {
    // Start the recursive creation from each child.
    // The owner HWND for all these controls is the main window.
    if (!CreateElementHwndRecursive(
            appCoreData->rootElement->children[i],
            appCoreData->mainHwnd, // Owner is the main window
            appCoreData->hInstance)) {
      hwndsCreatedSuccessfully = false;
      fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n",
              appCoreData->rootElement->children[i]->id);
      break; // Stop on the first failure
    }
  }

  return hwndsCreatedSuccessfully;
}

void WinUIApp_Show(struct WinUIApp *this) {
  ShowWindow(this->appCoreData->mainHwnd, this->winConfig.nCmdShow);
}

void WinUIApp_Update(struct WinUIApp *this) {
  UpdateWindow(this->appCoreData->mainHwnd); // Sends initial WM_PAINT
}

int WinUIApp_RunMessageLoop(struct WinUIApp *this) {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    // Check > 0 for messages, 0 for WM_QUIT, -1 for error
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
}

int WinUIAppRun(struct WinUIApp *this) {
  WinUIApp_Show(this);
  WinUIApp_Update(this);
  int return_value = WinUIApp_RunMessageLoop(this);
  return return_value;
}

void WinUIAppCleanUp(struct WinUIApp *this) {
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
}

void WinUIApp__del__(struct WinUIApp *this) { WinUIAppCleanUp(this); }

HWND hSubmitButton;
struct Form1Output {
  char __dummy;
} Form1Output;

void AddTodoHandler(UIElement *button, void *userData) {
  // Cast userData back to the expected type (the root UIElement)
  UIElement *root = (UIElement *)userData;
  if (!root) {
    fprintf(stderr,
            "Error: AddTodoHandler called with NULL userData (root).\n");
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
    if (!editElement)
      fprintf(stderr, "Error: Could not find element 'todoInput' from root "
                      "passed to AddTodoHandler.\n");
    if (!listElement)
      fprintf(stderr, "Error: Could not find element 'todoList' from root "
                      "passed to AddTodoHandler.\n");
  }
}

///*///

void AddTodo(UIElementPtr button, voidPtr userData) {
  AddTodoHandler(button, userData);
}
///*///

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
    hSubmitButton =
        CreateWindowW(L"Button", L"Submit", WS_VISIBLE | WS_CHILD, 10, 10, 100,
                      25, hwnd, (HMENU)1000, NULL, NULL);
    break;

  case WM_COMMAND:

    if (HIWORD(wParam) == EN_CHANGE) { // Text input changed
      switch (LOWORD(wParam)) {}
    }

    if (LOWORD(wParam) == 1000) { // Submit Button Clicked

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

  // RedirectIOToConsole();

  ///*///  main()

  // hInstance and nCmdShow are C types above and not registered in ANIL yet,
  // so we use a struct to pass them.
  struct WinUIAppConfig AppConfig;
  AppConfig.hInstance = hInstance;
  AppConfig.nCmdShow = nCmdShow;

  struct WinUIApp App;
  int result_code = WinUIAppCreate(&App, AppConfig, "Todo Application");

  if (result_code == -1) {
    // Initialization failed
    return -1;
  }

  struct UIWidget ui;
  struct UIWidget headerLabel = UIWidgetCreateLabel(
      &ui, 0, 0, 100, 25, "Todo Application", "headerLabel");
  struct UIWidget todoList = UIWidgetCreateList(&ui, 0, 0, 0, 150, "todoList");
  struct UIWidget inputRow = UIWidgetCreateHBox(&ui, 0, 0, 0, 30, "inputRow");
  struct UIWidget todoInput =
      UIWidgetCreateLineInput(&ui, 0, 0, 0, 0, "todoInput");
  struct UIWidget addButton =
      UIWidgetCreateButton(&ui, 0, 0, 60, 0, "Add TODO", "addButton");

  struct UIWidget root_elem = WinUIAppGetRootWidget(&App);
  UIWidgetAddChild(&root_elem, headerLabel);
  UIWidgetAddChild(&root_elem, todoList);
  UIWidgetAddChild(&root_elem, inputRow);
  UIWidgetAddChild(&inputRow, todoInput);
  UIWidgetAddChild(&inputRow, addButton);

  // Create Windows Controls (HWNDs) for Children of Root.
  bool create_status = WinUIAppCreateControls(&App);

  if (create_status == false) {
    fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n",
            root_elem.uiElement->id);
    WinUIAppCleanUp(&App);
    return -1;
  }

  // Setup Event Handlers.
  // Pass the root element as userData so the handler can find other elements
  struct VoidPointer payload;
  VoidPointer__init__(&payload, root_elem);
  UIWidgetSetOnClickCallback(&addButton, AddTodo, payload);

  UIWidgetAddItemToList(&todoList, "Complete UI Framework");
  UIWidgetAddItemToList(&todoList, "Implement JSX like syntax");

  int exitCode = WinUIAppRun(&App);

  WinUIApp__del__(&App);
  ///*///

  return 0;
}
