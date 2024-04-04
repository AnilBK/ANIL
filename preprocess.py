from typing import Optional
import Parser
import lexer

source_file = "struct_source.c"
# source_file = "string_source.c"
# source_file = "iterator_test.c"
output_file_name = source_file.split(".")[0] + "_generated.c"

file = open(source_file, "r")
Lines = file.readlines()
file.close()

LinesCache = []

inside_match_loop = False
inside_for_loop = False
inside_if = False

match_condition_count = 0
match_variable_name = ""
match_type = "variable"  # variable,struct

match_struct_type_info = []

struct_definations = []
instanced_struct_names = []
GlobalStructInitCode = ""

string_variable_names = []

optional_types_to_register = set()


def extract_string_between_quotes(input_string):
    start_index = input_string.find('"')
    if start_index == -1:
        return None  # No opening double quote found

    end_index = input_string.find('"', start_index + 1)
    if end_index == -1:
        return None  # No closing double quote found

    return input_string[start_index + 1 : end_index]


class Struct:
    def __init__(self, p_struct_name: str, p_members: list) -> None:
        self.name = p_struct_name
        self.members = p_members


def get_struct_info(p_struct_name: str) -> Optional[Struct]:
    for defined_struct in struct_definations:
        if defined_struct.name == p_struct_name:
            return defined_struct
    return None


class DataType:
    def __init__(self, p_name: str, p_type: str):
        self.name = p_name
        self.type = p_type


class Function:
    def __init__(self, p_fn_name: str, p_fn_arguments: list, p_fn_defn: str) -> None:
        self.fn_name = p_fn_name
        self.fn_arguments = p_fn_arguments
        self.fn_defn = p_fn_defn.strip("\n")

    def data_types_of_all_arguments(self):
        types = [arg.type for arg in self.fn_arguments]
        return types


Functions = []


def call_function(p_fn_name: str, passed_arguments: list):
    list_of_types_of_args = [arg.type for arg in passed_arguments]
    for function in Functions:
        fn_args_list = function.data_types_of_all_arguments()
        if not (fn_args_list == list_of_types_of_args):
            continue

        if function.fn_name == p_fn_name:
            fn_defn_str = function.fn_defn
            for idx, args in enumerate(function.fn_arguments):
                fn_defn_str = fn_defn_str.replace(args.name, passed_arguments[idx].name)
            return fn_defn_str

    print(f"Function \"{p_fn_name} isn't defined.")
    exit(0)


is_inside_new_code = False
is_inside_struct_impl = False

for Line in Lines:
    if "// clang-format off" in Line:
        continue
    elif "// clang-format on" in Line:
        continue

    if "///*///" in Line:
        is_inside_new_code = not is_inside_new_code

    if not is_inside_new_code:
        # Normal C code, so just write that.
        LinesCache.append(Line)
        continue

    Line = Line.strip(" ")



    if inside_for_loop and "%}" in Line:
        inside_for_loop = False
        LinesCache.append("}\n")
    elif inside_if and "%}" in Line:
        inside_if = False
        LinesCache.append("}\n")
    elif inside_match_loop:
        if "%}" in Line:
            inside_match_loop = False
            # LinesCache.append("}\n")
        elif match_type == "variable":
            # 1 => printf("One");
            rhs = Line.split("=>")
            match_condition_value = rhs[0].strip()
            statements = rhs[1].strip()

            is_else_condition = False
            if match_condition_count == 0:
                LinesCache.append("if ")
            else:
                if match_condition_value == "_":
                    is_else_condition = True
                    LinesCache.append("else")
                else:
                    LinesCache.append("else if")

            if is_else_condition:
                LinesCache.append(f"{{\n")
            else:
                # 3 | 4 | 5
                if "|" in match_condition_value:
                    values_to_test = match_condition_value.split("|")

                    statement_list = []
                    for value in values_to_test:
                        statement_list.append(f"{match_variable_name} == {value}")
                    combined_statement = " || ".join(statement_list)
                    LinesCache.append(f" ({combined_statement}){{\n")
                elif "..." in match_condition_value:
                    # 5 ... 10
                    operands = match_condition_value.split("...")
                    start_range = operands[0]
                    end_range = operands[1]
                    LinesCache.append(
                        f" ( {match_variable_name} >= {start_range} && {match_variable_name} <= {end_range} ){{\n"
                    )
                else:
                    LinesCache.append(
                        f" ( {match_variable_name} == {match_condition_value}){{\n"
                    )
            LinesCache.append(f"    {statements} \n}}\n")

            match_condition_count += 1
        elif match_type == "struct":
            # (0, 0) => printf("Origin.");
            rhs = Line.split("=>")
            match_condition = rhs[0].strip()
            statements = rhs[1].strip()

            type, name = match_struct_type_info
            # print(match_struct_type_info)

            match_condition = match_condition.split("(")[1]
            match_condition = match_condition.split(")")[0]
            conds = match_condition.split(",")

            # Get all the members for the given struct type.
            StructInfo = get_struct_info(type)
            if StructInfo is None:
                raise ValueError("Struct undefined.")
            members = StructInfo.members

            cond_struct_members_pair = list(zip(conds, members))
            condition_list = []
            assignment_list = []

            # (x,y,z)
            are_all_conditions_assignments = all(
                conds.strip().isnumeric() for conds, memb in cond_struct_members_pair
            )

            for conds, memb in cond_struct_members_pair:
                conds = conds.strip()
                if conds == "_":
                    continue
                elif conds.isnumeric():
                    condition_list.append(f"({name}.{memb} ==  {conds})")
                    # print(f"if({name}.{memb} ==  {conds}){{")
                else:
                    assignment_list.append(f"int {conds} = {name}.{memb};")
                    # print(f"float {conds} = {name}.{memb};")

            print(" && ".join(condition_list))
            conditions_joined = " && ".join(condition_list)

            # Bug: #(x,y,z) condition works only on the last arm of match.
            # (x,y,z) is final pattern same as using _.
            # so there can be only one such condition ;D
            # if are_all_conditions_assignments:
            #    LinesCache.append("else{")
            if match_condition_count == 0:
                LinesCache.append("if")
            else:
                LinesCache.append("else if")

            if len(condition_list) == 0:
                # (x,y,z), (_,_,_)
                conditions_joined = "true"
            LinesCache.append(f"({conditions_joined})\n{{\n")

            if len(assignment_list) > 0:
                assignment_joined = "\n".join(assignment_list)
                LinesCache.append(f"    {assignment_joined} \n\n")
            LinesCache.append(f"    {statements} \n}}\n")

            match_condition_count += 1
    elif is_inside_struct_impl and not "endfunc" in Line:
        GlobalStructInitCode += Line
    elif "Option" in Line:
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.OPTION)
        parser.next_token()
        parser.match_token(lexer.Token.SMALLER_THAN)
        parser.next_token()
        data_type = parser.get_token()
        print(f"Optional data type = {data_type}")
        parser.match_token(lexer.Token.GREATER_THAN)
        parser.next_token()
        var_name = parser.get_token()
        print(f"Optional data var name = {var_name}")
        parser.match_token(lexer.Token.SEMICOLON)

        optional_types_to_register.add(data_type)
        # Optional_int
        data_type_to_write = "Optional_" + data_type
        LinesCache.append(f"{data_type_to_write} {var_name};\n")
    elif 'print(f "' in Line:
        #   % print(f "Hello {meaning}");
        print("Procesing [Print]")
        rhs = Line.split("print(f ")[1]
        # print(rhs)
        string = rhs.split('");')
        actual_str = string[0].strip('"')
        print(actual_str)

        braces_open = False
        str_text = ""
        extracted_var_name_list = []
        extracted_var_name = ""
        for char in actual_str:
            if char == "{":
                braces_open = True
                extracted_var_name = ""
            elif char == "}":
                braces_open = False
                str_text += "%d"
                extracted_var_name_list.append(extracted_var_name)
            elif braces_open:
                extracted_var_name += char
            else:
                str_text += char

        print(str_text)
        print(extracted_var_name_list)
        str_to_write = f'printf("{str_text} "'
        if len(extracted_var_name_list) != 0:
            str_to_write += "," + ",".join(extracted_var_name_list)
        str_to_write += ");\n"
        print(str_to_write)
        LinesCache.append(str_to_write)
    elif "match" in Line:
        # % match x{
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.MATCH)
        parser.next_token()
        match_variable_name = parser.get_token()
        print(f"Matching variable {match_variable_name}")
        parser.match_token(lexer.Token.LEFT_CURLY)

        inside_match_loop = True
        match_condition_count = 0

        is_Struct = False
        user_struct_type = ""
        type = ""
        name = ""
        # [struct_type,struct_name]
        for s in instanced_struct_names:
            type, name = s
            if name == match_variable_name:
                is_Struct = True
                user_struct_type = type
                break

        if is_Struct:
            match_struct_type_info = [type, name]
            match_type = "struct"  # variable,struct
        else:
            match_type = "variable"  # variable,struct
    elif "let" in Line and "= [" in Line:
        # let arr = [ 1, 2, 3, 4, 5 ];
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.LET)
        parser.next_token()
        array_name = parser.get_token()
        print(f"var name = {array_name}")
        parser.match_token(lexer.Token.EQUALS)
        parser.next_token()
        parser.match_token(lexer.Token.LEFT_SQUARE_BRACKET)
        parser.next_token()

        array_values = []

        while parser.current_token() != lexer.Token.RIGHT_SQUARE_BRACKET:
            arr_value = parser.get_token()
            array_values.append(arr_value)
            print(f"Array Value : {arr_value}")
            if parser.current_token() == lexer.Token.COMMA:
                parser.next_token()

        parser.next_token()
        parser.match_token(lexer.Token.SEMICOLON)

        array_element_count = len(array_values)
        array_elements_str = ",".join(array_values)

        LinesCache.append(f"int {array_name}[] = {{ {array_elements_str} }};\n")
        LinesCache.append(
            f"unsigned int {array_name}_array_size = {array_element_count};\n\n"
        )
    elif "let" in Line and "{" in Line:
        # let p1 = Point{10, 20};
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.LET)
        parser.next_token()
        struct_name = parser.get_token()
        print(f"struct name = {struct_name}")
        parser.match_token(lexer.Token.EQUALS)
        parser.next_token()

        struct_type = parser.get_token()
        print(f"struct type = {struct_type}")

        StructInfo = get_struct_info(struct_type)
        print(StructInfo)
        if StructInfo is None:
            raise ValueError("Struct undefined.")
        instanced_struct_names.append([struct_type, struct_name])

        parser.match_token(lexer.Token.LEFT_CURLY)
        parser.next_token()

        values_list = []

        while parser.current_token() != lexer.Token.RIGHT_CURLY:
            arr_value = parser.get_token()
            values_list.append(arr_value)
            print(f"Array Value : {arr_value}")
            if parser.current_token() == lexer.Token.COMMA:
                parser.next_token()

        parser.next_token()
        parser.match_token(lexer.Token.SEMICOLON)

        code = f"struct {struct_type} {struct_name};\n"

        struct_var_values_pairs = list(zip(StructInfo.members, values_list))
        for struct_var, values in struct_var_values_pairs:
            code += f"{struct_name}.{struct_var} = {values};\n"

        LinesCache.append(code)
    elif "let" in Line and '"' in Line:
        # let str = "Hello World";
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.LET)
        parser.next_token()
        string_name = parser.get_token()
        print(f"Obtained String name :{string_name}")
        parser.match_token(lexer.Token.EQUALS)
        parser.next_token()
        parser.match_token(lexer.Token.QUOTE)
        parser.next_token()
        string = parser.get_token()
        print(f"Obtained String : {string}")
        parser.match_token(lexer.Token.QUOTE)

        LinesCache.append(f'char {string_name}[100] = "{string}";\n')

        string_variable_names.append(string_name)
    elif "if" in Line and "in" in Line and '"' in Line:
        # if "string" in str {
        inside_if = True
        parser = Parser.Parser(Line)
        parser.match_token(lexer.Token.IF)
        parser.next_token()
        parser.match_token(lexer.Token.QUOTE)
        parser.next_token()
        string = parser.get_token()
        print(f"Obtained String : {string}")

        parser.match_token(lexer.Token.QUOTE)
        parser.next_token()

        parser.match_token(lexer.Token.IN)
        parser.next_token()

        var_to_check_against = parser.get_token()
        print(f"var to check against= {var_to_check_against}")

        if var_to_check_against in string_variable_names:
            parser.match_token(lexer.Token.LEFT_CURLY)

            gen_code = call_function(
                "operator:in",
                [DataType(f'"{string}"', ""), DataType(var_to_check_against, "")],
            )
            LinesCache.append(f"\nif({gen_code}){{\n")
            # LinesCache.append(
            # f'\nif(strstr({var_to_check_against}, "{string}") != NULL ){{\n'
            # )

        else:
            print("TODO!")
            exit(0)
    elif "if" in Line and "==" in Line:
        # if str == "Hello"
        inside_if = True
        parser = Parser.Parser(Line)
        parser.match_token(lexer.Token.IF)
        parser.next_token()
        var_to_check_against = parser.get_token()
        print(f"Obtained String : {var_to_check_against}")

        parser.match_token(lexer.Token.EQUALS)
        parser.next_token()
        parser.match_token(lexer.Token.EQUALS)
        parser.next_token()

        parser.match_token(lexer.Token.QUOTE)
        parser.next_token()

        string = extract_string_between_quotes(Line)

        if var_to_check_against in string_variable_names:
            gen_code = call_function(
                "operator:==",
                [DataType(var_to_check_against, ""), DataType(f'"{string}"', "")],
            )
            LinesCache.append(f"\nif({gen_code}){{\n")
        else:
            print("TODO!")
            exit(0)
    elif "if" in Line:
        # % if var_to_check in var_to_check_against {
        inside_if = True
        # %,if,var_to_check,in,var_to_check_against,{
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.IF)
        parser.next_token()
        var_to_check = parser.get_token()
        print(f"var to check = {var_to_check}")

        parser.match_token(lexer.Token.IN)
        parser.next_token()

        var_to_check_against = parser.get_token()
        print(f"var to check against= {var_to_check_against}")

        parser.match_token(lexer.Token.LEFT_CURLY)

        gen_code = call_function(
            "operator:in",
            [DataType(var_to_check, ""), DataType(var_to_check_against, "Array")],
        )
        LinesCache.append(f"\nif({gen_code}){{\n")

        # LinesCache.append(
        #    f"\nif(array_contains({var_to_check_against},{var_to_check_against}_array_size,{var_to_check})){{\n"
        # )
    elif "for" in Line and not ("enumerate" in Line):
        inside_for_loop = True
        # % for value in arr{
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.FOR)
        parser.next_token()
        ranged_index_item_variable = parser.get_token()

        parser.next_token()
        array_name = parser.get_token()
        print(f"Ranged for with array name : {array_name}")
        print(f"Ranged Indexed item : {ranged_index_item_variable}")

        parser.match_token(lexer.Token.LEFT_CURLY)

        if array_name in string_variable_names:
            LinesCache.append(
                f"char *iterator = {array_name};"
                f"while (*iterator != '\\0') {{"
                f"char value = *iterator;"
                f"iterator++;"
            )
        else:
            LinesCache.append(
                f"for (unsigned int i = 0; i < {array_name}_array_size; i++){{\n"
                f"int {ranged_index_item_variable} = {array_name}[i];\n"
            )
    elif "for" in Line and ("enumerate" in Line):
        inside_for_loop = True
        # % for value in arr{
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.FOR)
        parser.next_token()
        ranged_index_item_variable = parser.get_token()

        parser.match_token(lexer.Token.COMMA)
        parser.next_token()

        current_array_value_variable = parser.get_token()
        print(f"Ranged array value variable : {current_array_value_variable}")

        # in
        parser.next_token()
        parser.match_token(lexer.Token.ENUMERATE)

        parser.next_token()
        array_name = parser.get_token()
        print(f"Ranged for with array name : {array_name}")
        print(f"Ranged Indexed item : {ranged_index_item_variable}")

        parser.match_token(lexer.Token.LEFT_CURLY)

        LinesCache.append(
            f"Iterator {array_name}_iter = create_iterator_from_array({array_name}, {array_name}_array_size); \n"
            f"Enumerator {array_name}_enumerator;\n"
            f"{array_name}_enumerator.index = -1;\n\n"
            f"while (enumerate(&{array_name}_iter, &{array_name}_enumerator)) {{\n"
            f"int {ranged_index_item_variable} = {array_name}_enumerator.index;\n"
            f"int {current_array_value_variable} = {array_name}_enumerator.val;\n"
        )

    elif "struct" in Line:
        # % struct Point { x, y };
        parser = Parser.Parser(Line)

        parser.match_token(lexer.Token.STRUCT)
        parser.next_token()
        struct_name = parser.get_token()

        print(f"Found Struct Defination : {struct_name}")

        parser.match_token(lexer.Token.LEFT_CURLY)
        parser.next_token()

        struct_members_list = []

        while parser.current_token() != lexer.Token.RIGHT_CURLY:
            struct_member = parser.get_token()
            struct_members_list.append(struct_member)
            print(f"Struct Member : {struct_member}")
            if parser.current_token() == lexer.Token.COMMA:
                parser.next_token()

        parser.next_token()
        parser.match_token(lexer.Token.SEMICOLON)

        struct_data = Struct(struct_name, struct_members_list)
        struct_definations.append(struct_data)

        code = f"struct {struct_name} {{\n"
        for mem in struct_members_list:
            code += f"int {mem};\n"
        code += f"}};\n\n"

        GlobalStructInitCode += code
        # LinesCache.append(code)
        # [struct_name,x,y,z..]

    elif "=" in Line and not "fn" in Line:
        # str += "World"
        parser = Parser.Parser(Line)

        var_name = parser.get_token()
        print(f"Obtained var name : {var_name}")

        if var_name in string_variable_names:
            parser.match_token(lexer.Token.PLUS)
            parser.next_token()
            parser.match_token(lexer.Token.EQUALS)
            parser.next_token()
            parser.match_token(lexer.Token.QUOTE)

            string = extract_string_between_quotes(Line)
            print(f"Obtained String : {string}")

            gen_code = call_function(
                "operator:+", [DataType(var_name, ""), DataType(f'"{string}"', "")]
            )
            LinesCache.append(f"{gen_code}\n")
            # LinesCache.append(f'strcat({var_name}, "{string}");\n')
    elif "fn" in Line:
        # if we use equals to then the if condition above executes.
        # TODO : Fix that.
        # fn (dst) + (src) > strcat(dst, src);
        parser = Parser.Parser(Line)
        parser.match_token(lexer.Token.FN)

        parser.next_token()
        parser.match_token(lexer.Token.LEFT_ROUND_BRACKET)

        parser.next_token()
        param1 = parser.current_token()
        print(f"Obtained First param : {param1}")

        parser.next_token()
        parser.match_token(lexer.Token.RIGHT_ROUND_BRACKET)

        parser.next_token()

        print(parser.current_token())

        operator = ""
        if parser.current_token() == lexer.Token.IN:
            operator = "in"
            parser.next_token()
        elif parser.current_token() == lexer.Token.PLUS:
            operator = "+"
            parser.next_token()
        elif parser.current_token() == lexer.Token.EQUALS:
            parser.next_token()
            if parser.current_token() == lexer.Token.EQUALS:
                operator = "=="
                parser.next_token()
        else:
            print("Parsing Function operator failed.")
            exit(0)

        # parser.next_token()
        parser.match_token(lexer.Token.LEFT_ROUND_BRACKET)

        parser.next_token()
        param2 = parser.current_token()
        print(f"Obtained Second param : {param2}")

        data_type = ""

        parser.next_token()
        if parser.current_token() == lexer.Token.COLON:
            parser.next_token()
            data_type = parser.current_token()
            parser.next_token()

        parser.match_token(lexer.Token.RIGHT_ROUND_BRACKET)

        parser.next_token()
        parser.match_token(lexer.Token.GREATER_THAN)

        fn_defn_str = Line.split(">")[1]
        print(f"Function Defination : {fn_defn_str}")

        param2_data_type = DataType(param2, data_type)
        param1_data_type = DataType(param1, "")

        fn = Function(
            f"operator:{operator}", [param1_data_type, param2_data_type], fn_defn_str
        )
        Functions.append(fn)

        gen_code = call_function(
            f"operator:{operator}", [DataType("Hello", ""), DataType("World", "")]
        )
        print(f"{gen_code}")
    elif "call" in Line:
        # call p1 say
        # void say(struct Point* this){
        #   printf("%d %d", this->x, this->y);
        # }
        # Change to ::
        # say(&p1);
        #
        parser = Parser.Parser(Line)
        # Skip "call", its not a keyword, just a placeholder to identify a function call.
        parser.next_token()

        struct_name = parser.current_token()

        parser.next_token()
        fn_name = parser.current_token()

        print(f"struct_name : {struct_name}, fn_name : {fn_name}")

        LinesCache.append(f"{fn_name}(&{struct_name});\n")
    elif "impl" in Line:
        # impl Point say
        # void say(struct Point *this) { printf("x : %d , y : %d \n", this->x, this->y); }

        parser = Parser.Parser(Line)
        # Skip "impl", its not a keyword, just a placeholder to identify a function call.
        parser.next_token()

        struct_name = parser.current_token()

        parser.next_token()
        fn_name = parser.current_token()

        print(f"struct_name : {struct_name}, fn_name : {fn_name}")

        code = f"void {fn_name}(struct {struct_name} *this) {{\n"
        GlobalStructInitCode += code
        is_inside_struct_impl = True
    elif "endfunc" in Line:
        if is_inside_struct_impl:
            GlobalStructInitCode += "}\n\n"
            is_inside_struct_impl = False
        else:
            print("End impl without being in impl block.")
            exit(0)
    else:
        LinesCache.append(Line)


print(get_struct_info("Point"))

OptionalInitCode = ""
for data_type in optional_types_to_register:
    # // typedef Option(int) Optional_int;
    OptionalInitCode += f"typedef Option({data_type}) Optional_{data_type};\n"

for i in range(len(LinesCache)):
    if "// OPTIONAL_INIT_CODE //" in LinesCache[i]:
        LinesCache[i] = OptionalInitCode
    elif "// STRUCT_DEFINATIONS //" in LinesCache[i]:
        LinesCache[i] = GlobalStructInitCode


outputFile = open(output_file_name, "w")
for Line in LinesCache:
    outputFile.writelines(Line)
outputFile.close()

import subprocess

try:
    subprocess.run(["clang-format", "-i", output_file_name], check=True)
    print(f"Successfully formatted {output_file_name} using clang-format.")
except subprocess.CalledProcessError as e:
    print(f"Error running clang-format: {e}")
