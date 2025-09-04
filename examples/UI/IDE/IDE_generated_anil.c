

///////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UIElement__MAX_CHILDREN 10
#define UIElement__MAX_TEXT_LENGTH 256
#define UIElement__DEFAULT_ITEM_HEIGHT 20 // Default height for list items
#define UIElement__MIN_LIST_HEIGHT 50     // Minimum height for the listbox
#define UIElement__MIN_TEXTAREA_HEIGHT 50 // Minimum height for the editor
#define UIElement__MAX_FILTER_LENGTH 512  // Max length for file dialog filters

typedef enum {
  LABEL,
  BUTTON,
  LIST,
  EDIT,
  TEXTAREA,
  HORIZONTAL,
  VERTICAL
} UIType;

struct UIElement;

// Function pointer type for event handlers
typedef void (*EventHandler)(void *);

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
  EventHandler onClick;    // Event handler for button clicks
  void *userData;          // Custom data for event handlers
  int itemCount;           // For list elements - track number of items
  char filePath[MAX_PATH]; // For file picker button - store selected file path
  bool isFilePicker;
  char filterString[UIElement__MAX_FILTER_LENGTH]; // File filter for file
                                                   // picker dialogs
  int filterStringLength; // Current length of the filter string
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

HWND g_MainWindowHWND = NULL;

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
  element->filterStringLength = 0;
  // Ensure the filter string is double-null-terminated initially.
  element->filterString[0] = '\0';
  element->filterString[1] = '\0';

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
  case TEXTAREA:
    className = WC_EDIT;
    style |=
        ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_BORDER;
    exStyle |= WS_EX_CLIENTEDGE;
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
  case TEXTAREA:
    return element->height > 0 ? element->height : 150;
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

void SetEditText(UIElement *edit, char *text) {
  if (edit && (edit->type == EDIT || edit->type == TEXTAREA) && edit->hwnd) {
    SetWindowText(edit->hwnd, text);
  }
}

char *GetEditText(UIElement *edit) {
  if (edit == NULL || (edit->type != EDIT && edit->type != TEXTAREA) ||
      edit->hwnd == NULL)
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
  if (edit && (edit->type == EDIT || edit->type == TEXTAREA) && edit->hwnd) {
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
    // Step 1: Discovery Pass.
    // Go through children to sum up the height of all fixed-size elements
    // and count the number of flexible elements.
    int fixedHeightSum = 0;
    int flexibleChildCount = 0;
    for (int i = 0; i < container->childCount; i++) {
      UIElement *child = container->children[i];
      if (child) {
        // Define which types are flexible. For now, just the text area.
        // We could expand this to a flag like `child->isFlexible` later.
        if (child->type == TEXTAREA) {
          flexibleChildCount++;
        } else {
          fixedHeightSum += CalculatePreferredHeight(child);
        }
      }
    }

    // Step 2: Calculation.
    // Determine the space remaining for flexible children.
    int remainingHeight = container->height - fixedHeightSum;
    if (remainingHeight < 0) {
      // Prevent negative heights if fixed items overflow.
      remainingHeight = 0;
    }

    int heightPerFlexibleChild =
        (flexibleChildCount > 0) ? (remainingHeight / flexibleChildCount) : 0;

    // Enforce a minimum height for flexible children.
    if (heightPerFlexibleChild < UIElement__MIN_TEXTAREA_HEIGHT) {
      heightPerFlexibleChild = UIElement__MIN_TEXTAREA_HEIGHT;
    }

    // Step 3: Application Pass.
    // Go through children again to set their final position and size.
    int currentY = container->y;
    int containerInnerX = container->x;
    int containerWidth = container->width;

    for (int i = 0; i < container->childCount; i++) {
      UIElement *child = container->children[i];
      if (!child)
        continue;

      int childHeight;
      if (child->type == TEXTAREA) {
        childHeight = heightPerFlexibleChild;
      } else {
        childHeight = CalculatePreferredHeight(child);
      }

      child->x = containerInnerX;
      child->y = currentY;
      child->width = containerWidth;
      child->height = childHeight;

      if (child->hwnd) {
        SetWindowPos(child->hwnd, NULL, child->x, child->y, child->width,
                     child->height,
                     SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
      }

      currentY += childHeight; // Move to the next position for the next child
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

void UIElement_AddFileFilter(UIElement *element, const char *description,
                             const char *pattern) {
  if (!element || !element->isFilePicker)
    return;

  size_t desc_len = strlen(description);
  size_t patt_len = strlen(pattern);

  // Check if there is enough space: desc + null + pattern + null + final extra
  // null.
  if (element->filterStringLength + desc_len + 1 + patt_len + 1 + 1 >
      UIElement__MAX_FILTER_LENGTH) {
    fprintf(stderr, "Error: Not enough space to add new file filter.\n");
    return;
  }

  // Append description.
  memcpy(element->filterString + element->filterStringLength, description,
         desc_len);
  element->filterStringLength += desc_len;
  element->filterString[element->filterStringLength++] = '\0';

  // Append pattern.
  memcpy(element->filterString + element->filterStringLength, pattern,
         patt_len);
  element->filterStringLength += patt_len;
  element->filterString[element->filterStringLength++] = '\0';

  // Add the final double null terminator for the whole list.
  element->filterString[element->filterStringLength] = '\0';
}

void OpenFilePickerDialog(void *filePickerElement) {
  struct UIElement *element = (struct UIElement *)(filePickerElement);
  if (element == NULL) {
    fprintf(stderr, "Error: Creating Widget failed.\n");
    return;
  }

  OPENFILENAME ofn;       // Common dialog box structure
  char szFile[260] = {0}; // Buffer for file name

  // Initialize OPENFILENAME
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = g_MainWindowHWND; // Use the main window handle
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = element->filterStringLength > 0 ? element->filterString
                                                    : "All Files\0*.*\0\0";
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetOpenFileName(&ofn) == TRUE) {
    strncpy(element->filePath, szFile, sizeof(element->filePath) - 1);
    element->filePath[sizeof(element->filePath) - 1] = '\0';
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

      if (element) {
        // For file pickers, open the dialog.
        if (element->type == BUTTON && element->isFilePicker) {
          OpenFilePickerDialog(element);
          fprintf(stderr, "Opened File Picker.\n");
        }

        // Filepickers also have onClick handlers.
        // i.e they trigger after a file is picked.
        if (element->onClick) {
          fprintf(stderr, "Normal Button OnClick handler.\n");
          element->onClick(element->userData);
        }
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
  FILE *fp;
  freopen_s(&fp, "CONOUT$", "w", stdout);
  freopen_s(&fp, "CONOUT$", "w", stderr);

  // Redirect STDIN to the console
  freopen_s(&fp, "CONIN$", "r", stdin);

  // Optional: You can set the console title if you like
  SetConsoleTitle(TEXT("Console Window"));
}

// Initializes the main window, creates the root UI element, and sets up the
// WinUIAppCoreData structure.

#include <commctrl.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct String {
  char *arr;
  int length;
  int capacity;
  bool is_constexpr;
};

struct Vector_String {
  struct String *arr;
  int size;
  int capacity;
};

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

struct File {
  FILE *file_ptr;
};

char *Stringc_str(struct String *this);
size_t Stringlen(struct String *this);
char String__getitem__(struct String *this, int index);
size_t Stringlength_of_charptr(struct String *this, char *p_string);
void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length);
void Stringinit__STATIC__(struct String *this, char *text, int p_text_length);
void String__init__OVDstr(struct String *this, char *text);
void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length);
void String__init__OVDstructString(struct String *this, struct String text);
void Stringclear(struct String *this);
void String_allocate_more(struct String *this, int n);
void Stringprint(struct String *this);
void StringprintLn(struct String *this);
void String__del__(struct String *this);
bool Stringstartswith(struct String *this, char *prefix);
struct String Stringsubstr(struct String *this, int start, int length);
struct String Stringstrip(struct String *this);
struct Vector_String Stringsplit(struct String *this, char delimeter);
bool String__contains__(struct String *this, char *substring);
bool String__eq__(struct String *this, char *pstring);
void String__add__(struct String *this, char *pstring);
void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length);
void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring);
void String__reassign__OVDstr(struct String *this, char *pstring);
void Stringset_to_file_contents(struct String *this, char *pfilename);
struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename);

void VoidPointer__init__(struct VoidPointer *this, struct UIWidget payload);

struct String UIWidgetgetFilePath(struct UIWidget *this);
struct UIWidget UIWidgetCreateUIWidgetFromVoidPtr(struct UIWidget *this,
                                                  voidPtr ptr);
bool UIWidgetisValid(struct UIWidget *this);
struct UIWidget UIWidgetFindElementById(struct UIWidget *this, char *id);
void UIWidgetSetOnClickCallback(struct UIWidget *this,
                                void (*onClickFn)(voidPtr),
                                struct VoidPointer payload);
void UIWidgetAddChild(struct UIWidget *this, struct UIWidget p_child);
struct UIWidget UIWidgetCreateLabel(struct UIWidget *this, int x, int y,
                                    int width, int height, char *initialText,
                                    char *id);
struct UIWidget UIWidgetCreateList(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
struct UIWidget UIWidgetCreateLineInput(struct UIWidget *this, int x, int y,
                                        int width, int height, char *id);
struct UIWidget UIWidgetCreateFilePicker(struct UIWidget *this, int x, int y,
                                         int width, int height, char *id);
void UIWidgetAddFileFilter(struct UIWidget *this, char *description,
                           char *pattern);
struct UIWidget UIWidgetCreateButton(struct UIWidget *this, int x, int y,
                                     int width, int height, char *initialText,
                                     char *id);
struct UIWidget UIWidgetCreateVBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
struct UIWidget UIWidgetCreateHBox(struct UIWidget *this, int x, int y,
                                   int width, int height, char *id);
struct UIWidget UIWidgetCreateTextArea(struct UIWidget *this, int x, int y,
                                       int width, int height, char *id);
void UIWidgetAddItemToList(struct UIWidget *this, char *itemText);
void UIWidgetClearEditText(struct UIWidget *this);
void UIWidgetSetEditText(struct UIWidget *this, struct String text);
struct String UIWidgetGetEditText(struct UIWidget *this);
void UIWidgetRemoveSelectedListItem(struct UIWidget *this);
int UIWidgetGetTotalItemsInList(struct UIWidget *this);
struct String UIWidgetGetListItemAtIndex(struct UIWidget *this, int index);
struct Vector_String UIWidgetGetAllItemsInList(struct UIWidget *this);
struct String UIWidgetOpenFilePickerAndReadContents(struct UIWidget *this);
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

void File__init__(struct File *this, char *p_file_name);
void Filewrite(struct File *this, char *p_content);
void Filewriteline(struct File *this, char *p_content);
void File__del__(struct File *this);
struct UIWidget CreateUIWidgetFromVoidPtr(voidPtr ptr);
void Compile(voidPtr userData);
void Execute(voidPtr userData);
void Load(voidPtr userData);
void OpenFileAndLoadToEditor(voidPtr userData);
void SaveEditorContentsToFile(voidPtr userData);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow);
size_t Vector_Stringlen(struct Vector_String *this);
void Vector_String__init__(struct Vector_String *this, int capacity);
void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index);
void Vector_String_call_destructor_for_all_elements(struct Vector_String *this);
void Vector_String_reset(struct Vector_String *this);
void Vector_String__del__(struct Vector_String *this);
struct String Vector_String__getitem__(struct Vector_String *this, int index);
void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value);
void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value);
void Vector_Stringvalidate_index(struct Vector_String *this, int index);
struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s);
void Vector_String_set(struct Vector_String *this, int index,
                       struct String value);
void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value);
void Vector_Stringallocate_more(struct Vector_String *this, int n);
void Vector_String_grow_if_required(struct Vector_String *this);
void Vector_String_push(struct Vector_String *this, struct String value);
void Vector_Stringpush(struct Vector_String *this, struct String value);
struct String Vector_Stringpop(struct Vector_String *this);
void Vector_String_shift_left_from(struct Vector_String *this, int index);
void Vector_Stringremove_at(struct Vector_String *this, int index);
void Vector_String_clear(struct Vector_String *this);
void Vector_Stringclear(struct Vector_String *this);
bool Vector_String__contains__(struct Vector_String *this, struct String value);
void Vector_Stringprint(struct Vector_String *this);

char *Stringc_str(struct String *this) { return this->arr; }

size_t Stringlen(struct String *this) { return this->length; }

char String__getitem__(struct String *this, int index) {
  return *(this->arr + index);
}

size_t Stringlength_of_charptr(struct String *this, char *p_string) {
  // This should be some kind of static method.
  return strlen(p_string);
}

void String__init__from_charptr(struct String *this, char *text,
                                int p_text_length) {
  // p_text_length : Length of the string without the null terminator.
  this->arr = (char *)malloc((p_text_length + 1) * sizeof(char));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  strncpy(this->arr, text, p_text_length);
  this->arr[p_text_length] = '\0';

  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = false;
}

void Stringinit__STATIC__(struct String *this, char *text, int p_text_length) {
  // WARNING: Only the compiler should write a call to this function.
  // The compiler uses this initialization function to create a temporary String
  // object when a string literal is passed to a function that expects a String
  // object.
  this->arr = text;
  this->length = p_text_length;
  this->capacity = p_text_length + 1;
  this->is_constexpr = true;
}

void String__init__OVDstr(struct String *this, char *text) {
  size_t p_text_length = Stringlength_of_charptr(this, text);
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstrint(struct String *this, char *text,
                             int p_text_length) {
  String__init__from_charptr(this, text, p_text_length);
}

void String__init__OVDstructString(struct String *this, struct String text) {
  size_t p_text_length = Stringlen(&text);
  String__init__from_charptr(this, Stringc_str(&text), p_text_length);
}

void Stringclear(struct String *this) {
  this->arr = (char *)realloc(this->arr, sizeof(char));
  this->arr[0] = '\0';
  this->length = 0;
  this->capacity = 1;
}

void String_allocate_more(struct String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n + 1;

  char *new_arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
  if (!new_arr) {
    fprintf(stderr, "Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  } else {
    this->arr = new_arr;
    this->capacity = new_capacity;
  }
}

void Stringprint(struct String *this) { printf("%s", this->arr); }

void StringprintLn(struct String *this) { printf("%s\n", this->arr); }

void String__del__(struct String *this) {
  if (!this->is_constexpr) {
    free(this->arr);
  }
}

bool Stringstartswith(struct String *this, char *prefix) {
  return strncmp(this->arr, prefix, strlen(prefix)) == 0;
}

struct String Stringsubstr(struct String *this, int start, int length) {
  struct String text;
  String__init__from_charptr(&text, &this->arr[start], length);
  return text;
}

struct String Stringstrip(struct String *this) {
  char *begin = this->arr;
  char *end = begin + Stringlen(this) - 1;

  // Remove leading whitespaces.
  while (isspace(*begin)) {
    begin++;
  }

  // Remove trailing whitespaces.
  while (end > begin && isspace(*end)) {
    end--;
  }

  // Length of the substring between 'begin' and 'end' inclusive.
  int new_length = end - begin + 1;

  struct String text;
  String__init__from_charptr(&text, begin, new_length);
  return text;
}

struct Vector_String Stringsplit(struct String *this, char delimeter) {
  // NOTE : Because of this function, before import String, we require import
  // Vector.
  struct Vector_String split_result;
  Vector_String__init__(&split_result, 2);

  int index = 0;
  int segment_start = 0;

  size_t tmp_len_0 = Stringlen(this);
  for (size_t i = 0; i < tmp_len_0; i++) {
    char character = String__getitem__(this, i);

    if (character == delimeter) {

      if (segment_start < index) {
        struct String segment =
            Stringsubstr(this, segment_start, index - segment_start);
        Vector_Stringpush(&split_result, segment);
        String__del__(&segment);
      }
      segment_start = index + 1;
    }
    index = index + 1;
  }

  if (segment_start < Stringlen(this)) {
    struct String remaining_segment =
        Stringsubstr(this, segment_start, Stringlen(this) - segment_start);
    Vector_Stringpush(&split_result, remaining_segment);
    String__del__(&remaining_segment);
  }

  return split_result;
}

bool String__contains__(struct String *this, char *substring) {
  return strstr(this->arr, substring) != NULL;
}

bool String__eq__(struct String *this, char *pstring) {
  return strcmp(this->arr, pstring) == 0;
}

void String__add__(struct String *this, char *pstring) {
  size_t new_length = this->length + strlen(pstring) + 1;

  if (new_length > this->capacity) {
    size_t new_capacity;
    if (this->capacity == 0) {
      new_capacity = new_length * 2;
    } else {
      new_capacity = this->capacity;
      while (new_capacity <= new_length) {
        new_capacity *= 2;
      }
    }
    this->arr = (char *)realloc(this->arr, new_capacity * sizeof(char));
    this->capacity = new_capacity;
  }

  if (this->arr == NULL) {
    fprintf(stderr, "Memory Re-Allocation Error.\n");
    exit(EXIT_FAILURE);
  }

  strcat(this->arr, pstring);
  this->length = new_length;
}

void Stringreassign_internal(struct String *this, char *pstring,
                             int p_text_length) {
  if (this->arr != NULL) {
    free(this->arr);
  }

  String__init__from_charptr(this, pstring, p_text_length);
}

void String__reassign__OVDstructString(struct String *this,
                                       struct String pstring) {
  char *src = Stringc_str(&pstring);
  size_t p_text_length = Stringlen(&pstring);
  Stringreassign_internal(this, src, p_text_length);
}

void String__reassign__OVDstr(struct String *this, char *pstring) {
  size_t p_text_length = Stringlength_of_charptr(this, pstring);
  Stringreassign_internal(this, pstring, p_text_length);
}

void Stringset_to_file_contents(struct String *this, char *pfilename) {
  // Read from the file & store the contents to this string.

  // TODO: Implement this function in ANIL itself, because the function below is
  // a mangled function name.
  Stringclear(this);

  FILE *ptr = fopen(pfilename, "r");
  if (ptr == NULL) {
    printf("File \"%s\" couldn't be opened.\n", pfilename);
    return;
  }

  char myString[256];
  bool has_data = false;

  while (fgets(myString, sizeof(myString), ptr)) {
    String__add__(this, myString);
    has_data = true;
  }

  fclose(ptr);

  if (!has_data) {
    // Double-clear just in case
    Stringclear(this);
  }
}

struct Vector_String StringreadlinesFrom(struct String *this, char *pfilename) {
  Stringset_to_file_contents(this, pfilename);
  struct Vector_String result = Stringsplit(this, '\n');
  return result;
}

void VoidPointer__init__(struct VoidPointer *this, struct UIWidget payload) {
  // Create a void* from UIWidget.
  // Used by 'SetOnClickCallback' of UIWidget.
  this->ptr = (void *)payload.uiElement;
}

struct String UIWidgetgetFilePath(struct UIWidget *this) {
  struct String path;
  if (this->uiElement) { // && this->uiElement->type == BUTTON) {
    // Use the new field in UIElement to get the file path.
    String__init__from_charptr(&path, this->uiElement->filePath,
                               strlen(this->uiElement->filePath));
    return path;
  } else {
    fprintf(stderr,
            "Error: getFilePath called on non-file-picker button element.\n");
    String__init__OVDstr(&path, "");
    return path;
  }
}

struct UIWidget UIWidgetCreateUIWidgetFromVoidPtr(struct UIWidget *this,
                                                  voidPtr ptr) {
  // FIXME: This is like a static function.
  // TODO: Have to do this because global c_functions dont have c function
  // bodies.
  UIElement *element = (UIElement *)ptr;
  if (element == NULL) {
    fprintf(stderr, "Error: Could not create UIWidget from a void*.\n");
    exit(-1);
  }

  struct UIWidget w;
  w.uiElement = element;
  return w;
}

bool UIWidgetisValid(struct UIWidget *this) {
  if (this->uiElement == NULL) {
    fprintf(stderr, "Error: UIWidget is not valid (NULL uiElement).\n");
    return false;
  }
  return true;
}

struct UIWidget UIWidgetFindElementById(struct UIWidget *this, char *id) {
  struct UIWidget w;
  w.uiElement = NULL;

  if (this->uiElement == NULL || id == NULL) {
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
}

void UIWidgetSetOnClickCallback(struct UIWidget *this,
                                void (*onClickFn)(voidPtr),
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

struct UIWidget UIWidgetCreateFilePicker(struct UIWidget *this, int x, int y,
                                         int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement = CreateUIElement(BUTTON, x, y, width, height,
                                (LPCSTR) "Open File", (LPCSTR)id);
  w.uiElement->isFilePicker = true;
  return w;
}

void UIWidgetAddFileFilter(struct UIWidget *this, char *description,
                           char *pattern) {
  if (this->uiElement) {
    UIElement_AddFileFilter(this->uiElement, description, pattern);
  }
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

struct UIWidget UIWidgetCreateTextArea(struct UIWidget *this, int x, int y,
                                       int width, int height, char *id) {
  struct UIWidget w;
  w.uiElement =
      CreateUIElement(TEXTAREA, x, y, width, height, (LPCSTR) "", (LPCSTR)id);
  return w;
}

void UIWidgetAddItemToList(struct UIWidget *this, char *itemText) {
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
}

void UIWidgetClearEditText(struct UIWidget *this) {
  if (this->uiElement->type != EDIT) {
    fprintf(stderr, "Error: ClearEditText called on non-edit element.\n");
    return;
  }
  ClearEditText(this->uiElement);
}

void UIWidgetSetEditText(struct UIWidget *this, struct String text) {
  if (this->uiElement &&
      (this->uiElement->type == EDIT || this->uiElement->type == TEXTAREA)) {
    SetEditText(this->uiElement, Stringc_str(&text));
  } else {
    fprintf(stderr,
            "Error: SetEditText called on non-edit/text_editor element.\n");
  }
}

struct String UIWidgetGetEditText(struct UIWidget *this) {
  struct String EditText;

  if (this->uiElement &&
      (this->uiElement->type != EDIT && this->uiElement->type != TEXTAREA)) {
    fprintf(stderr, "Error: GetEditText called on non-edit element.\n");

    // Just return an empty string.
    String__init__OVDstr(&EditText, "");
    return EditText;
  }

  char *text = GetEditText(this->uiElement);

  String__init__from_charptr(&EditText, text, strlen(text));

  // 'GetEditText' provides a buffer that needs to be freed after use.
  free(text);

  return EditText;
}

void UIWidgetRemoveSelectedListItem(struct UIWidget *this) {
  if (this->uiElement->type != LIST) {
    fprintf(stderr,
            "Error: RemoveSelectedListItem called on non-list element.\n");
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
}

int UIWidgetGetTotalItemsInList(struct UIWidget *this) {
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
}

struct String UIWidgetGetListItemAtIndex(struct UIWidget *this, int index) {
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
  } else {
    success = true;
    SendMessage(this->uiElement->hwnd, LB_GETTEXT, index, (LPARAM)itemText);
  }

  if (success) {
    // Use of mangled function directly.
    String__init__from_charptr(&text, itemText, strlen(itemText));
    free(itemText);
  } else {
    // Initialize an empty string if allocation failed or other error occurred.
    String__init__OVDstr(&text, "");
  }

  return text;
}

struct Vector_String UIWidgetGetAllItemsInList(struct UIWidget *this) {
  // Returns a vector of strings containing all items in the list.
  int total_items = UIWidgetGetTotalItemsInList(this);
  struct Vector_String result;
  Vector_String__init__(&result, total_items);

  for (size_t i = 0; i < total_items; i++) {
    struct String item = UIWidgetGetListItemAtIndex(this, i);

    if (Stringlen(&item) > 0) {
      Vector_Stringpush(&result, item);
    }
    String__del__(&item);
  }
  return result;
}

struct String UIWidgetOpenFilePickerAndReadContents(struct UIWidget *this) {
  struct String fileContents;
  String__init__OVDstr(&fileContents, "");

  HWND hwndOwner = this->uiElement->hwnd; // The main window owns the dialog
  char szFile[MAX_PATH] = {0};

  OPENFILENAMEA ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwndOwner;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = this->uiElement->filterStringLength > 0
                        ? this->uiElement->filterString
                        : "All Files\0*.*\0\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileNameA(&ofn) == TRUE) {
    HANDLE hFile =
        CreateFileA(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
      DWORD dwFileSize = GetFileSize(hFile, NULL);
      if (dwFileSize != INVALID_FILE_SIZE) {
        char *buffer = (char *)malloc(dwFileSize + 1);
        if (buffer) {
          DWORD dwBytesRead = 0;
          if (ReadFile(hFile, buffer, dwFileSize, &dwBytesRead, NULL) &&
              dwBytesRead == dwFileSize) {
            buffer[dwFileSize] = '\0';
            Stringreassign_internal(&fileContents, buffer, dwFileSize);
          } else {
            MessageBox(hwndOwner, "Error reading file content.", "File Error",
                       MB_OK | MB_ICONERROR);
            String__init__OVDstr(&fileContents, "");
          }
          free(buffer);
        } else {
          MessageBox(hwndOwner, "Memory allocation failed.", "Error",
                     MB_OK | MB_ICONERROR);
          Stringclear(&fileContents);
        }
      } else {
        MessageBox(hwndOwner, "Could not get file size.", "File Error",
                   MB_OK | MB_ICONERROR);
        Stringclear(&fileContents);
      }
      CloseHandle(hFile);
    } else {
      MessageBox(hwndOwner, "Could not open selected file.", "File Error",
                 MB_OK | MB_ICONERROR);
      Stringclear(&fileContents);
    }
  }

  return fileContents;
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

  g_MainWindowHWND = hwnd;

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

void File__init__(struct File *this, char *p_file_name) {
  // Change mode from "w" to "wb".
  // This prevents the C runtime from translating '\n' to '\r\n',
  // which was causing double newlines while writing the content of Windows
  // control, because the input string from the Windows control already
  // contained '\r\n'.
  this->file_ptr = fopen(p_file_name, "wb");
  if (this->file_ptr == NULL) {
    printf("Failed to open file %s.\n", p_file_name);
    exit(0);
  }
}

void Filewrite(struct File *this, char *p_content) {
  fprintf(this->file_ptr, "%s", p_content);
}

void Filewriteline(struct File *this, char *p_content) {
  // Write a line to the file with terminating newline.
  fprintf(this->file_ptr, "%s\n", p_content);
}

void File__del__(struct File *this) { fclose(this->file_ptr); }

size_t Vector_Stringlen(struct Vector_String *this) { return this->size; }

void Vector_String__init__(struct Vector_String *this, int capacity) {
  // if we want to use instanced template type in fn body, we use following
  // syntax.
  // @ TEMPLATED_DATA_TYPE @
  this->arr = (struct String *)malloc(capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }
  this->size = 0;
  this->capacity = capacity;
}

void Vector_String_call_destructor_for_element(struct Vector_String *this,
                                               int index) {
  // If element at 'index' has a destructor, then emit a destructor call.
  // Otherwise emit nothing.
  // Evaluated at compile time.
  String__del__(&this->arr[index]);
}

void Vector_String_call_destructor_for_all_elements(
    struct Vector_String *this) {
  for (size_t i = 0; i < Vector_Stringlen(this); i++) {
    Vector_String_call_destructor_for_element(this, i);
  }
}

void Vector_String_reset(struct Vector_String *this) {
  free(this->arr);
  this->arr = NULL;
  this->size = 0;
  this->capacity = 0;
}

void Vector_String__del__(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_reset(this);
}

struct String Vector_String__getitem__(struct Vector_String *this, int index) {
  if (index < 0) {
    index += this->size;
  }
  // Vector<String> Specialization:
  // Returns &T ie &String, which means the return type is reference type.
  // So, the returned String isn't freed by the destructor.
  // for x in Vector<String>{}
  // x calls __getitem__() and is a String. Typically x should be freed at the
  // end of the loop. Since __getitem__() is a reference return type, it isn't
  // freed.
  return *(this->arr + index);
}

void Vector_String_set_ith_item(struct Vector_String *this, int index,
                                struct String value) {
  // NOTE: We assume that the index is valid.
  this->arr[index] = value;
}

void Vector_Stringpush_unchecked(struct Vector_String *this,
                                 struct String value) {
  this->arr[this->size++] = value;
}

void Vector_Stringvalidate_index(struct Vector_String *this, int index) {
  if (index < 0 || index >= this->size) {
    fprintf(stderr, "Index out of bounds: index = %d, size = %d.\n", index,
            this->size);
    exit(EXIT_FAILURE);
  }
}

struct String Vector_String_copy_string(struct Vector_String *this,
                                        struct String s) {
  struct String string_copy;
  String__init__OVDstructString(&string_copy, s);
  return string_copy;
}

void Vector_String_set(struct Vector_String *this, int index,
                       struct String value) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_set_ith_item(this, index, value);
}

void Vector_String__setitem__(struct Vector_String *this, int index,
                              struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_set(this, index, Vector_String_copy_string(this, value));
}

void Vector_Stringallocate_more(struct Vector_String *this, int n) {
  if (n <= 0) {
    // Prevent unnecessary reallocation or negative increments.
    return;
  }

  size_t new_capacity = this->capacity + n;
  struct String *new_arr =
      (struct String *)realloc(this->arr, new_capacity * sizeof(struct String));

  if (!new_arr) {
    fprintf(stderr, "Vector<>::allocate_more(): Memory reallocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->arr = new_arr;
  this->capacity = new_capacity;
}

void Vector_String_grow_if_required(struct Vector_String *this) {

  if (this->size >= this->capacity) {

    if (this->capacity > 0) {
      Vector_Stringallocate_more(this, this->capacity);
    } else {
      // Avoid 0 capacity.
      Vector_Stringallocate_more(this, 1);
    }
  }
}

void Vector_String_push(struct Vector_String *this, struct String value) {
  Vector_String_grow_if_required(this);
  Vector_Stringpush_unchecked(this, value);
}

void Vector_Stringpush(struct Vector_String *this, struct String value) {
  // Vector<String> Specialization:
  // Duplicate a string object, to prevent dangling pointers,
  // as when a string moves out of a scope, it is freed.
  Vector_String_push(this, Vector_String_copy_string(this, value));
}

struct String Vector_Stringpop(struct Vector_String *this) {
  if (this->size == 0) {
    fprintf(stderr, "Pop from empty Vector.\n");
    exit(EXIT_FAILURE);
  }
  return this->arr[--this->size];
}

void Vector_String_shift_left_from(struct Vector_String *this, int index) {
  // NOTE: The index is assumed to be valid(i.e not negative and within bounds).
  for (int i = index; i < this->size - 1; i++) {
    this->arr[i] = this->arr[i + 1];
  }
}

void Vector_Stringremove_at(struct Vector_String *this, int index) {

  if (index < 0) {
    index = index + this->size;
  }
  Vector_Stringvalidate_index(this, index);
  Vector_String_call_destructor_for_element(this, index);
  Vector_String_shift_left_from(this, index);
  this->size -= 1;
}

void Vector_String_clear(struct Vector_String *this) {
  free(this->arr);

  this->capacity = 1;
  this->arr = (struct String *)malloc(this->capacity * sizeof(struct String));

  if (this->arr == NULL) {
    fprintf(stderr, "Vector<>::_clear(): Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  this->size = 0;
}

void Vector_Stringclear(struct Vector_String *this) {
  Vector_String_call_destructor_for_all_elements(this);
  Vector_String_clear(this);
}

bool Vector_String__contains__(struct Vector_String *this,
                               struct String value) {
  size_t tmp_len_1 = Vector_Stringlen(this);
  for (size_t i = 0; i < tmp_len_1; i++) {
    struct String string = Vector_String__getitem__(this, i);

    if (Stringlen(&string) == Stringlen(&value)) {

      if (String__eq__(&string, Stringc_str(&value))) {
        return true;
      }
    }
  }
  return false;
}

void Vector_Stringprint(struct Vector_String *this) {
  printf("Vector<String> (size = %d, capacity = %d) : [", this->size,
         this->capacity);
  for (size_t i = 0; i < this->size; ++i) {
    printf("\"%s\"", this->arr[i].arr);
    if (i < this->size - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

struct UIWidget CreateUIWidgetFromVoidPtr(voidPtr ptr) {
  struct UIWidget w;
  struct UIWidget widget = UIWidgetCreateUIWidgetFromVoidPtr(&w, ptr);
  return widget;
}

void Compile(voidPtr userData) {
  struct UIWidget root = CreateUIWidgetFromVoidPtr(userData);
}

void Execute(voidPtr userData) {
  struct UIWidget root = CreateUIWidgetFromVoidPtr(userData);
}

void Load(voidPtr userData) {
  struct UIWidget root = CreateUIWidgetFromVoidPtr(userData);

  struct UIWidget editor = UIWidgetFindElementById(&root, "filePickerButton");

  if (UIWidgetisValid(&editor)) {
    struct String fileName = UIWidgetgetFilePath(&editor);

    struct String fileContents;
    String__init__OVDstrint(&fileContents, "", 0);
    Stringset_to_file_contents(&fileContents, Stringc_str(&fileName));

    struct UIWidget codeEditor = UIWidgetFindElementById(&root, "codeEditor");

    if (UIWidgetisValid(&codeEditor)) {
      UIWidgetSetEditText(&codeEditor, fileContents);
    }
    String__del__(&fileContents);
    String__del__(&fileName);
  }
}

void OpenFileAndLoadToEditor(voidPtr userData) {
  struct UIWidget root = CreateUIWidgetFromVoidPtr(userData);

  struct UIWidget editor = UIWidgetFindElementById(&root, "codeEditor");

  if (UIWidgetisValid(&editor)) {
    struct String fileContents = UIWidgetOpenFilePickerAndReadContents(&editor);
    UIWidgetSetEditText(&editor, fileContents);
    String__del__(&fileContents);
  }
}

void SaveEditorContentsToFile(voidPtr userData) {
  struct UIWidget root = CreateUIWidgetFromVoidPtr(userData);

  struct UIWidget editor = UIWidgetFindElementById(&root, "codeEditor");

  if (UIWidgetisValid(&editor)) {
    struct String fileContents = UIWidgetGetEditText(&editor);

    struct File outputFile;
    File__init__(&outputFile, "out.c");
    Filewrite(&outputFile, Stringc_str(&fileContents));
    File__del__(&outputFile);
    String__del__(&fileContents);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  struct WinUIAppConfig AppConfig;
  AppConfig.hInstance = hInstance;
  AppConfig.nCmdShow = nCmdShow;

  struct WinUIApp App;
  int result_code = WinUIAppCreate(&App, AppConfig, "ANIL IDE");

  if (result_code == -1) {
    // Initialization failed
    WinUIApp__del__(&App);

    return -1;
  }

  struct UIWidget root_elem = WinUIAppGetRootWidget(&App);
  struct UIWidget App__ui;
  struct UIWidget headerLabel =
      UIWidgetCreateLabel(&App__ui, 0, 0, 100, 25, "My Own IDE", "headerLabel");
  struct UIWidget codeEditor =
      UIWidgetCreateTextArea(&App__ui, 0, 0, 0, 30, "codeEditor");
  struct UIWidget actionRow =
      UIWidgetCreateHBox(&App__ui, 0, 0, 0, 30, "actionRow");
  struct UIWidget filePickerButton =
      UIWidgetCreateFilePicker(&App__ui, 0, 0, 0, 30, "filePickerButton");
  struct UIWidget loadButton =
      UIWidgetCreateButton(&App__ui, 0, 0, 60, 0, "Load File", "loadButton");
  struct UIWidget saveButton =
      UIWidgetCreateButton(&App__ui, 0, 0, 60, 0, "Save File", "saveButton");
  struct UIWidget compileButton =
      UIWidgetCreateButton(&App__ui, 0, 0, 60, 0, "Compile", "compileButton");
  struct UIWidget executeButton =
      UIWidgetCreateButton(&App__ui, 0, 0, 60, 0, "Execute", "executeButton");
  UIWidgetAddChild(&root_elem, headerLabel);
  UIWidgetAddChild(&root_elem, codeEditor);
  UIWidgetAddChild(&root_elem, actionRow);
  UIWidgetAddChild(&actionRow, filePickerButton);
  UIWidgetAddChild(&actionRow, loadButton);
  UIWidgetAddChild(&actionRow, saveButton);
  UIWidgetAddChild(&actionRow, compileButton);
  UIWidgetAddChild(&actionRow, executeButton);
  UIWidgetAddFileFilter(&filePickerButton, "ANIL & C Files (*.anil;*.c)",
                        "*.anil;*.c");
  UIWidgetAddFileFilter(&filePickerButton, "Anil Files (*.anil)", "*.anil");
  UIWidgetAddFileFilter(&filePickerButton, "C Source Files (*.c)", "*.c");

  // OnClick Callbacks.
  struct VoidPointer __payload_0;
  VoidPointer__init__(&__payload_0, root_elem);
  UIWidgetSetOnClickCallback(&filePickerButton, Load, __payload_0);
  struct VoidPointer __payload_1;
  VoidPointer__init__(&__payload_1, root_elem);
  UIWidgetSetOnClickCallback(&loadButton, Load, __payload_1);
  struct VoidPointer __payload_2;
  VoidPointer__init__(&__payload_2, root_elem);
  UIWidgetSetOnClickCallback(&saveButton, SaveEditorContentsToFile,
                             __payload_2);
  struct VoidPointer __payload_3;
  VoidPointer__init__(&__payload_3, root_elem);
  UIWidgetSetOnClickCallback(&compileButton, Compile, __payload_3);
  struct VoidPointer __payload_4;
  VoidPointer__init__(&__payload_4, root_elem);
  UIWidgetSetOnClickCallback(&executeButton, Execute, __payload_4);

  // Create Windows Controls (HWNDs) for Children of Root.
  bool create_status = WinUIAppCreateControls(&App);

  if (create_status == false) {
    fprintf(stderr, "Failed to create HWND tree starting from child '%s'.\n",
            root_elem.uiElement->id);
    WinUIAppCleanUp(&App);
    WinUIApp__del__(&App);

    return -1;
  }

  int exitCode = WinUIAppRun(&App);

  WinUIApp__del__(&App);

  return 0;
}
