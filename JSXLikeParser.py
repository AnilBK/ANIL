class UIAttribute:
    def __init__(self, name, value):
        self.name = name
        self.value = value


class UIElement:
    def __init__(self, name):
        self.name = name
        self.id = ""
        self.attributes = []
        self.content = ""
        self.children = []

    def set_content(self, content):
        self.content = content

    def add_attribute(self, attribute):
        if attribute.name == "id":
            self.id = attribute.value
        self.attributes.append(attribute)

    def has_attribute(self, attribute_name):
        return any(attr.name == attribute_name for attr in self.attributes)

    def get_attribute_value(self, attribute_name):
        for attr in self.attributes:
            if attr.name == attribute_name:
                return attr.value
        raise ValueError(f'Attribute "{attribute_name}" not found.')

    def __str__(self):
        attrs = ", ".join(f"{attr.name}: {attr.value}" for attr in self.attributes)
        return f"{self.name}({attrs})"


ROOT_ELEMENT_NAME = "@root@"


class UIElementTree:
    def __init__(self):
        self.root = UIElement(ROOT_ELEMENT_NAME)
        self.root.id = ROOT_ELEMENT_NAME
        self.current = self.root
        self.stack = [self.root]

        self.payload_var_count = 0

    def add_element(self, element):
        self.current.children.append(element)
        self.stack.append(element)
        self.current = element

        if element.name == "App":
            self.root.id = element.get_attribute_value("rootElement")

    def close_tag(self, tag_name):
        if self.stack[-1].name != tag_name:
            raise ValueError(
                f"Tag mismatch: expected {self.stack[-1].name}, got {tag_name}"
            )
        self.stack.pop()
        self.current = self.stack[-1]

    def generate_code(self):
        CODEGEN_EXCEPTIONS = {"App"}

        def is_root_element(element):
            # root elements id is set to 'rootElement' attribute in the 'App' element.
            # So, we cant just use 'self.root' to check if the element is root or not.
            # Optionally, we can remove this check and use 'self.root.name' to check if the element is root or not.
            return element.id == ROOT_ELEMENT_NAME or element.name == ROOT_ELEMENT_NAME

        # Get all descendants including root.
        def get_descendants(node):
            result = [node]
            for child in node.children:
                result.extend(get_descendants(child))
            return result

        def generate_add_child_code(node):
            lines = []
            for child in node.children:
                if not is_root_element(child) and child.name not in CODEGEN_EXCEPTIONS:
                    lines.append(f"{node.id}.AddChild({child.id})")
                    lines.extend(generate_add_child_code(child))
            return lines

        all_elements = get_descendants(self.root)

        # <App id="_" name="App" rootElement="root_elem"></App>
        app_creation_help = '<App id="_" name="App" rootElement="root_elem"></App>'

        # Find an element called 'App'.
        # 'App' element stores basic information about the app,
        # such as the root element of the tree and the name of the app.
        # It is mandatory to have an 'App' element in the tree.
        app_element = next((e for e in all_elements if e.name == "App"), None)
        if not app_element:
            raise ValueError(
                f"App element not found. Create an App element(e.g: {app_creation_help})."
            )

        # root element of the tree is stored in 'rootElement' attribute
        # of the 'App' element.
        root_element = app_element.get_attribute_value("rootElement")
        if not root_element:
            raise ValueError(
                "rootElement attribute not found in App element. Add 'rootElement' attribute(e.g: {app_creation_help})."
            )

        app_name = app_element.get_attribute_value("name")
        if not app_name:
            raise ValueError(
                f"name attribute not found in App element. Add 'name' attribute(e.g: {app_creation_help}) ."
            )

        app_ui_var = f"{app_name}__ui"
        creation_map = {
            "Label": lambda e: f'let {e.id} = {app_ui_var}.CreateLabel(0, 0, 100, 25, "{e.content}", "{e.id}")',
            "Button": lambda e: f'let {e.id} = {app_ui_var}.CreateButton(0, 0, 60, 0, "{e.content}", "{e.id}")',
            "Input": lambda e: f'let {e.id} = {app_ui_var}.CreateLineInput(0, 0, 0, 0, "{e.id}")',
            "List": lambda e: f'let {e.id} = {app_ui_var}.CreateList(0, 0, 100, 25, "{e.id}")',
            "HBox": lambda e: f'let {e.id} = {app_ui_var}.CreateHBox(0, 0, 0, 30, "{e.id}")',
        }

        # See 'UI_TODO_App.c' to see how code should be generated for UI elements.
        code = [f"let {root_element} = {app_name}.GetRootWidget()"]
        code.append(f"let {app_ui_var} = UIWidget {{ }};")

        # Create UI elements.
        # let headerLabel = ui.CreateLabel(0, 0, 100, 25, "Todo Application", "headerLabel")
        for element in all_elements:
            if element.name in CODEGEN_EXCEPTIONS or is_root_element(element):
                continue
            if element.name in creation_map:
                code.append(creation_map[element.name](element))
            else:
                raise ValueError(
                    f"[UI Codegen] Creating Unsupported element type: {element.name}"
                )

        # root_elem.AddChild(headerLabel)
        code.extend(generate_add_child_code(self.root))

        def generate_onclick_code(self):
            code = []
            for element in all_elements:
                if element.name == "Button" and element.has_attribute("onclick"):
                    onclick = element.get_attribute_value("onclick")
                    if onclick:
                        # onclick can be "f()" or "f(payload)"
                        fn_name = onclick.split("(")[0]
                        fn_param = onclick.split("(")[1].split(")")[0]
            
                        # // Setup Event Handlers.
                        # // Pass the root element as userData so the handler can find other elements
                        # let payload = VoidPointer{root_elem};
                        # addButton.SetOnClickCallback(AddTodo, payload)

                        payload_var_name =  f"__payload_{self.payload_var_count}"
                        self.payload_var_count += 1

                        payload_code = f"let {payload_var_name} = VoidPointer{{ {fn_param} }}; \n"
                        code.append(payload_code)
                        code.append(f"{element.id}.SetOnClickCallback({fn_name}, {payload_var_name}) \n")

            if code:
                comment_line = ["\n", "\n", "#OnClick Callbacks.\n"]
                code = comment_line + code
            return code

        code.extend(generate_onclick_code(self))

        return code
