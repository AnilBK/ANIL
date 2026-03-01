class NameMangler:
    @staticmethod
    def mangle_function(struct_name: str, function_name: str) -> str:
        return f"{struct_name}{function_name}"

    @staticmethod
    def mangle_templated_class_type(
        p_base_class_type: str, p_templated_type: str
    ) -> str:
        """Mangle an instance of A<T> as A_T."""
        return p_base_class_type + "_" + p_templated_type

    @staticmethod
    def mangle_templated_class_fn_name(
        p_struct_type: str, p_fn_name: str, p_templated_data_type: str
    ) -> str:
        return p_struct_type + "_" + p_templated_data_type + p_fn_name

    @staticmethod
    def _mangle_type(type_name: str) -> str:
        # Convert type names to mangled form.
        # For example, "char*" becomes "str".
        type_map = {"char*": "str"}
        # For types not in the map, remove spaces (e.g., "struct String" -> "structString")
        return type_map.get(type_name, type_name.replace(" ", ""))

    @staticmethod
    def mangle_function_with_parameters(
        p_struct_type: str, p_fn_name: str, p_parameters: list
    ) -> str:
        # append(int) -> ListappendOVDint
        # append(char*) -> ListappendOVDstr
        function_name = NameMangler.mangle_function(p_struct_type, p_fn_name)
        function_name += "OVD"
        for param in p_parameters:
            function_name += NameMangler._mangle_type(param)
        return function_name
