#include <windows.h>

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND hNameLabel, hNameInput, hAgeLabel, hAgeInput, hCheckbox, hSubmitButton, hFruitLabel, hFruitDropdown;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"FormWindow";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"FormWindow", L"Form Example", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                  100, 100, 400, 400, NULL, NULL, NULL, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            // Name Label
            hNameLabel = CreateWindowW(L"Static", L"Name:", WS_VISIBLE | WS_CHILD,
                                       50, 50, 100, 25, hwnd, NULL, NULL, NULL);
            
            // Name Input (TextBox)
            hNameInput = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                       150, 50, 200, 25, hwnd, NULL, NULL, NULL);

            // Age Label
            hAgeLabel = CreateWindowW(L"Static", L"Age:", WS_VISIBLE | WS_CHILD,
                                      50, 100, 100, 25, hwnd, NULL, NULL, NULL);
            
            // Age Input (Number field)
            hAgeInput = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                      150, 100, 200, 25, hwnd, NULL, NULL, NULL);
            
            // Set a default value for the age input
            SetWindowTextW(hAgeInput, L"25");

            // Checkbox for Boolean (Auto Check Enabled)
            hCheckbox = CreateWindowW(L"Button", L"Check if true", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                      150, 150, 200, 25, hwnd, NULL, NULL, NULL);
            
            // Fruit Label
            hFruitLabel = CreateWindowW(L"Static", L"Favorite Fruit:", WS_VISIBLE | WS_CHILD,
                                        50, 200, 100, 25, hwnd, NULL, NULL, NULL);
            
            // Dropdown (Combo Box) for Favorite Fruits
            hFruitDropdown = CreateWindowW(L"Combobox", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                                           150, 200, 200, 100, hwnd, NULL, NULL, NULL);

            // Add items to the dropdown
            SendMessageW(hFruitDropdown, CB_ADDSTRING, 0, (LPARAM)L"Apple");
            SendMessageW(hFruitDropdown, CB_ADDSTRING, 0, (LPARAM)L"Banana");
            SendMessageW(hFruitDropdown, CB_ADDSTRING, 0, (LPARAM)L"Orange");
            SendMessageW(hFruitDropdown, CB_ADDSTRING, 0, (LPARAM)L"Grapes");
            SendMessageW(hFruitDropdown, CB_ADDSTRING, 0, (LPARAM)L"Mango");

            // Set a default value for the dropdown (index 2, which is "Orange")
            SendMessageW(hFruitDropdown, CB_SETCURSEL, (WPARAM)2, 0);

            // Submit Button
            hSubmitButton = CreateWindowW(L"Button", L"Submit", WS_VISIBLE | WS_CHILD,
                                          150, 300, 100, 25, hwnd, (HMENU)1, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Submit Button Clicked
                wchar_t name[30], age[10], selectedFruit[30];
                GetWindowTextW(hNameInput, name, 30);
                GetWindowTextW(hAgeInput, age, 10);

                int isChecked = SendMessage(hCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;

                // Get selected fruit from the combo box
                int selectedFruitIndex = SendMessageW(hFruitDropdown, CB_GETCURSEL, 0, 0);
                SendMessageW(hFruitDropdown, CB_GETLBTEXT, selectedFruitIndex, (LPARAM)selectedFruit);

                wchar_t message[200];
                wsprintfW(message, L"Name: %s\nAge: %s\nFavorite Fruit: %s\nCheckbox: %s",
                          name, age, selectedFruit, isChecked ? L"Checked" : L"Not Checked");

                MessageBoxW(hwnd, message, L"Submit Result", MB_OK);
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
