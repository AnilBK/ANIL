class GuiItem:
    def __init__(self, name, var_type, has_options:bool):
        self.name = name
        self.var_type = var_type
        self.has_options = has_options
        self.associated_gui_variable_names = []

# This class handles generating GUI from input variables.
class InputVariablesGUI:
    def __init__(self):
        self.g_x = 10  
        self.g_y = 10
        self.hwnd_variables_list = []
        self.gui_code = []
        self.gui_item_options = []
        self.default_value = ""

    def add_gui_item_option(self, option):
        self.gui_item_options.extend(option)

    def register_default_value(self, value):
        self.default_value = value

    def process_int_variable(self, p_var_name):
        has_options = len(self.gui_item_options) > 0
        item = GuiItem(p_var_name, "int", has_options)

        self.gui_code.append(f'// {p_var_name} Label')
        self.gui_code.append(f'h{p_var_name}Label = CreateWindowW(L"Static", L"{p_var_name}:", WS_VISIBLE | WS_CHILD, {self.g_x}, {self.g_y}, 100, 25, hwnd, NULL, NULL, NULL);')
        item.associated_gui_variable_names = [f"h{p_var_name}Label"]

        if has_options:
            self.gui_code.append(f'// {p_var_name} Input (with Dropdown field)')
            self.gui_code.append(f'h{p_var_name}Dropdown =  CreateWindowW(L"Combobox", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, {self.g_x} + 100, {self.g_y}, 200, 100, hwnd, NULL, NULL, NULL);')
            item.associated_gui_variable_names.append(f"h{p_var_name}Dropdown")

            self.gui_code.append(f'// Add integer values to the dropdown')
            for option in self.gui_item_options:
                self.gui_code.append(f'SendMessageW(h{p_var_name}Dropdown, CB_ADDSTRING, 0, (LPARAM)L"{option}");')

            self.gui_code.append(f'//Set default value for integer dropdown (index 0)')
            self.gui_code.append(f'SendMessageW(h{p_var_name}Dropdown, CB_SETCURSEL, (WPARAM)0, 0);')
        else:
            self.gui_code.append(f'// {p_var_name} Input (Number field)')
            self.gui_code.append(f'h{p_var_name}Input = CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, {self.g_x} + 100, {self.g_y}, 200, 25, hwnd, NULL, NULL, NULL);')
            
            if self.default_value != "":
                self.gui_code.append(f'// Set default value for {p_var_name} Input')
                self.gui_code.append(f'SetWindowTextW(h{p_var_name}Input, L"{self.default_value}");')

            item.associated_gui_variable_names.append(f"h{p_var_name}Input")

        self.gui_item_options = []
        self.hwnd_variables_list.append(item)
        self.g_y += 40

    def process_bool_variable(self, p_var_name):
        item = GuiItem(p_var_name, "bool", False)
        item.associated_gui_variable_names = [f"h{p_var_name}CheckboxLabel"]

        self.gui_code.append(f'// {p_var_name} Checkbox Label(To the Left)')
        self.gui_code.append(f'h{p_var_name}CheckboxLabel = CreateWindowW(L"Static", L"{p_var_name}:", WS_VISIBLE | WS_CHILD, {self.g_x}, {self.g_y}, 100, 25, hwnd, NULL, NULL, NULL);')

        self.gui_code.append(f'// {p_var_name} Checkbox(To the Right)')
        self.gui_code.append(f'h{p_var_name}Checkbox = CreateWindowW(L"Button", L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, {self.g_x} + 100, {self.g_y}, 200, 25, hwnd, NULL, NULL, NULL);')
        item.associated_gui_variable_names.append(f"h{p_var_name}Checkbox")

        if self.default_value == "true":
            self.gui_code.append(f'// Set default value for {p_var_name} Checkbox to checked')
            self.gui_code.append(f'SendMessage(h{p_var_name}Checkbox, BM_SETCHECK, BST_CHECKED, 0);');

        self.hwnd_variables_list.append(item)
        self.g_y += 40

    def process_variable(self, p_var_name, p_var_data_type):
        if p_var_data_type == "int":
            self.process_int_variable(p_var_name)
        elif p_var_data_type == "bool":
            self.process_bool_variable(p_var_name)
            
        # Clear default value.
        # Default value was set before calling process_variable().
        self.default_value = ""

    def get_window_code(self):
        code = f"""
            WNDCLASSW wc = {{0}};
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.hInstance = hInst;
            wc.lpszClassName = L"FormWindow";
            wc.lpfnWndProc = WindowProcedure;

            if (!RegisterClassW(&wc)) {{
            return -1;
            }}

            CreateWindowW(L"FormWindow", L"Form", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                100, 100, 400, {self.g_y + 100}, NULL, NULL, NULL, NULL);

            MSG msg = {{0}};
            while (GetMessage(&msg, NULL, 0, 0)) {{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }}
        """

        # Code to copy from global struct to our local variables.
        copy_data_code = "\n"
        for item in self.hwnd_variables_list:
            # age = Form1Output.age;
            copy_data_code += f"{item.name} = Form1Output.{item.name};\n"
        
        return [code, copy_data_code]
    
    def get_gui_assignment_code(self):
        global_gui_assignment_code = ""
        for item in self.hwnd_variables_list:
            if item.var_type == "bool":
                # bool item can be queried from the gui item as,
                # int isChecked = SendMessage(hCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
                global_gui_assignment_code += f"Form1Output.{item.name} = SendMessage(h{item.name}Checkbox, BM_GETCHECK, 0, 0) == BST_CHECKED;"
            elif item.var_type == "int":
                if f"h{item.name}Input" in item.associated_gui_variable_names:
                    """
                    wchar_t buffer[256];  // Buffer to store the text from the input field
                    GetWindowTextW(hage2Input, buffer, 256);  // Retrieve text from the edit control
                    int age = _wtoi(buffer);  // Convert the text (wide string) to an integer
                    """
                    global_gui_assignment_code += f"wchar_t {item.name}_tmp_buffer[256]; "
                    global_gui_assignment_code += f"GetWindowTextW(h{item.name}Input, {item.name}_tmp_buffer, 256); "
                    global_gui_assignment_code += f"Form1Output.{item.name} = _wtoi({item.name}_tmp_buffer);\n\n"
                elif f"h{item.name}Dropdown" in item.associated_gui_variable_names:
                    """
                    // Get the selected index from the dropdown
                    int index = (int)SendMessageW(hageDropdown, CB_GETCURSEL, 0, 0);

                    if (index != CB_ERR) {
                        wchar_t buffer[256];  // Buffer to store the text from the selected item
                        // Get the text of the selected item
                        SendMessageW(hageDropdown, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer);

                        int age = _wtoi(buffer);  // Convert the wide string to an integer

                        // Use the 'age' value as needed
                        wprintf(L"Selected age: %d\n", age);
                    }
                    """

                    code = f"""
                    // Get the selected index from the dropdown
                    int {item.name}_tmp_index = (int)SendMessageW(h{item.name}Dropdown, CB_GETCURSEL, 0, 0);

                    if ({item.name}_tmp_index != CB_ERR) {{
                        wchar_t {item.name}_tmp_buffer[256];  // Buffer to store the text from the selected item
                        // Get the text of the selected item
                        SendMessageW(h{item.name}Dropdown, CB_GETLBTEXT, (WPARAM){item.name}_tmp_index, (LPARAM){item.name}_tmp_buffer);

                        Form1Output.{item.name} = _wtoi({item.name}_tmp_buffer);  // Convert the wide string to an integer
                    }} 
                    \n\n
                    """
                    global_gui_assignment_code += code
        return global_gui_assignment_code

    def get_hwnd_variable_declaration_code(self):
        names = []
        # A global struct to hold all the values from gui nodes of the current_form.
        struct_code = "struct Form1Output{"

        for item in self.hwnd_variables_list:
            gui_item_names = item.associated_gui_variable_names
            names.extend(gui_item_names)
            # int age;
            struct_code += f"{item.var_type} {item.name};"
        struct_code += "}Form1Output;"

        name_joined = ",".join(names)

        # HWND hNameLabel, hNameInput, hAgeLabel, hAgeInput, hCheckbox, hSubmitButton, hFruitLabel, hFruitDropdown;
        return f"HWND {name_joined} , hSubmitButton; {struct_code}\n\n"
    
    def get_gui_nodes_creation_code(self):
        codes = ""
        for code in self.gui_code:
            codes += f"{code}\n"
        
        # // Submit Button
        codes += f'hSubmitButton = CreateWindowW(L"Button", L"Submit", WS_VISIBLE | WS_CHILD, {self.g_x}, {self.g_y}, 100, 25, hwnd, (HMENU)1, NULL, NULL);'
        self.g_y += 40
        return f"{codes}\n"
    
