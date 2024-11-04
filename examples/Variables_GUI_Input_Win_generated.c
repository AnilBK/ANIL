#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// IMPORTS //

HWND hcar_ageLabel, hcar_ageDropdown, hcar_mileageLabel, hcar_mileageInput,
    his_electricCheckboxLabel, his_electricCheckbox, hSubmitButton;
struct Form1Output {
  int car_age;
  int car_mileage;
  bool is_electric;
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
    // car_age Label
    hcar_ageLabel = CreateWindowW(L"Static", L"car_age:", WS_VISIBLE | WS_CHILD,
                                  10, 10, 100, 25, hwnd, NULL, NULL, NULL);
    // car_age Input (with Dropdown field)
    hcar_ageDropdown = CreateWindowW(
        L"Combobox", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 10 + 100,
        10, 200, 100, hwnd, NULL, NULL, NULL);
    // Add integer values to the dropdown
    SendMessageW(hcar_ageDropdown, CB_ADDSTRING, 0, (LPARAM)L"5");
    SendMessageW(hcar_ageDropdown, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessageW(hcar_ageDropdown, CB_ADDSTRING, 0, (LPARAM)L"15");
    // Set default value for integer dropdown (index 0)
    SendMessageW(hcar_ageDropdown, CB_SETCURSEL, (WPARAM)0, 0);
    // car_mileage Label
    hcar_mileageLabel =
        CreateWindowW(L"Static", L"car_mileage:", WS_VISIBLE | WS_CHILD, 10, 50,
                      100, 25, hwnd, NULL, NULL, NULL);
    // car_mileage Input (Number field)
    hcar_mileageInput = CreateWindowW(
        L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 10 + 100,
        50, 200, 25, hwnd, NULL, NULL, NULL);
    // Set default value for car_mileage Input
    SetWindowTextW(hcar_mileageInput, L"200");
    // is_electric Checkbox Label(To the Left)
    his_electricCheckboxLabel =
        CreateWindowW(L"Static", L"is_electric:", WS_VISIBLE | WS_CHILD, 10, 90,
                      100, 25, hwnd, NULL, NULL, NULL);
    // is_electric Checkbox(To the Right)
    his_electricCheckbox =
        CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                      10 + 100, 90, 200, 25, hwnd, NULL, NULL, NULL);
    // Set default value for is_electric Checkbox to checked
    SendMessage(his_electricCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    hSubmitButton = CreateWindowW(L"Button", L"Submit", WS_VISIBLE | WS_CHILD,
                                  10, 130, 100, 25, hwnd, (HMENU)1, NULL, NULL);
    break;

  case WM_COMMAND:
    if (LOWORD(wParam) == 1) { // Submit Button Clicked

      // Get the selected index from the dropdown
      int car_age_tmp_index =
          (int)SendMessageW(hcar_ageDropdown, CB_GETCURSEL, 0, 0);

      if (car_age_tmp_index != CB_ERR) {
        wchar_t car_age_tmp_buffer[256]; // Buffer to store the text from the
                                         // selected item
        // Get the text of the selected item
        SendMessageW(hcar_ageDropdown, CB_GETLBTEXT, (WPARAM)car_age_tmp_index,
                     (LPARAM)car_age_tmp_buffer);

        Form1Output.car_age =
            _wtoi(car_age_tmp_buffer); // Convert the wide string to an integer
      }

      wchar_t car_mileage_tmp_buffer[256];
      GetWindowTextW(hcar_mileageInput, car_mileage_tmp_buffer, 256);
      Form1Output.car_mileage = _wtoi(car_mileage_tmp_buffer);

      Form1Output.is_electric =
          SendMessage(his_electricCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
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

  int car_age = 5;
  int car_mileage = 200;
  bool is_electric = true;

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
                100, 400, 230, NULL, NULL, NULL, NULL);

  MSG msg = {0};
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  car_age = Form1Output.car_age;
  car_mileage = Form1Output.car_mileage;
  is_electric = Form1Output.is_electric;

  printf("Car age is %d years.\n", car_age);
  printf("Car mileage is %d miles.\n", car_mileage);
  printf("Is the car electric? %d.\n", is_electric);

  ///*///

  return 0;
}