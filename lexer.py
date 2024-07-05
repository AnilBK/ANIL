from enum import Enum


class Token(Enum):
    LET = 0
    EQUALS = 1
    LEFT_SQUARE_BRACKET = 2
    RIGHT_SQUARE_BRACKET = 3
    SEMICOLON = 4
    COMMA = 5
    PERCENT = 6
    LEFT_CURLY = 7
    RIGHT_CURLY = 8
    STRUCT = 9
    MATCH = 10
    FOR = 11
    IF = 12
    IN = 13
    OPTION = 14
    SMALLER_THAN = 15
    GREATER_THAN = 16
    ENUMERATE = 17
    QUOTE = 18
    PLUS = 19
    LEFT_ROUND_BRACKET = 21
    RIGHT_ROUND_BRACKET = 22
    COLON = 23
    DOT = 24
    ASTERISK = 25
    MINUS = 26
    DEF = 27
    ENDDEF = 28
    CFUNCTION = 29
    ENDCFUNCTION = 30
    ELSE = 31
    TRUE = 32
    FALSE = 33
    CONSTEXPR = 34
    HASH = 35
    INCLUDE = 36
    AT = 37
    APPLY_HOOK = 38
    HOOK_BEGIN = 39
    HOOK_END = 40
    EXCLAMATION = 41
    FUNCTION = 42
    ENDFUNCTION = 43
    RETURN = 44
    NAMESPACE = 45
    ENDNAMESPACE = 46


CHARACTER_TOKENS = {
    "=": Token.EQUALS,
    "[": Token.LEFT_SQUARE_BRACKET,
    "]": Token.RIGHT_SQUARE_BRACKET,
    ";": Token.SEMICOLON,
    ",": Token.COMMA,
    "%": Token.PERCENT,
    "{": Token.LEFT_CURLY,
    "}": Token.RIGHT_CURLY,
    "<": Token.SMALLER_THAN,
    ">": Token.GREATER_THAN,
    '"': Token.QUOTE,
    "+": Token.PLUS,
    "(": Token.LEFT_ROUND_BRACKET,
    ")": Token.RIGHT_ROUND_BRACKET,
    ":": Token.COLON,
    ".": Token.DOT,
    "*": Token.ASTERISK,
    "-": Token.MINUS,
    "#": Token.HASH,
    "@": Token.AT,
    "!": Token.EXCLAMATION,
}


KEYWORD_TOKENS = {
    "let": Token.LET,
    "struct": Token.STRUCT,
    "match": Token.MATCH,
    "for": Token.FOR,
    "if": Token.IF,
    "in": Token.IN,
    "Option": Token.OPTION,
    "enumerate": Token.ENUMERATE,
    "def": Token.DEF,
    "c_function": Token.CFUNCTION,
    "endc_function": Token.ENDCFUNCTION,
    "enddef": Token.ENDDEF,
    "else": Token.ELSE,
    "True": Token.TRUE,
    "False": Token.FALSE,
    "constexpr": Token.CONSTEXPR,
    "include": Token.INCLUDE,
    "apply_hook": Token.APPLY_HOOK,
    "hook_begin": Token.HOOK_BEGIN,
    "hook_end": Token.HOOK_END,
    "function": Token.FUNCTION,
    "endfunction": Token.ENDFUNCTION,
    "return": Token.RETURN,
    "namespace": Token.NAMESPACE,
    "endnamespace": Token.ENDNAMESPACE,
}


def get_tokens(line):
    line = line.strip("\n")
    length = len(line)

    token = ""
    tokens = []

    inside_string = False

    def add_token_raw(p_token):
        tokens.append(p_token)

    def add_token(p_token):
        if p_token in KEYWORD_TOKENS.keys():
            tokens.append(KEYWORD_TOKENS[p_token])
        elif p_token in CHARACTER_TOKENS.keys():
            tokens.append(CHARACTER_TOKENS[p_token])
        else:
            # "=" the inner equals to shouldn't be tokenized.
            add_token_raw(p_token)

    escape_back_slash = False

    for i in range(length):
        char = line[i]

        if escape_back_slash:
            if char == '"':
                token += char
                escape_back_slash = False
            elif char == "\\":
                token += "\\\\"
                escape_back_slash = False
            else:
                token += "\\"
                token += char
                escape_back_slash = False
        elif char == '"':
            if inside_string:
                # End of string.
                inside_string = False

                # Single character tokens like = are tokenized by add_token(),
                # so we use the following method.
                # "=" the inner equals to shouldn't be tokenized.
                add_token_raw(token)

                token = ""
            else:
                # Start of string.
                inside_string = True
            tokens.append(Token.QUOTE)
        elif inside_string:
            # \"
            if char == "\\":
                escape_back_slash = True
                continue

            token += char
        elif char == " ":
            if token == "":
                continue
            add_token(token)
            token = ""
        else:
            if char in CHARACTER_TOKENS:
                if token != "":
                    add_token(token)
                tokens.append(CHARACTER_TOKENS[char])
                token = ""
                continue

            if token in CHARACTER_TOKENS:
                tokens.append(CHARACTER_TOKENS[token])
                token = ""
                continue
            token += char

    # Process the last token.
    if token != "":
        add_token(token)

    # print(tokens)
    return tokens


if __name__ == "__main__":

    line = '"="'
    tk = get_tokens(line)

    line = '" String Str " " {{ ] } t{10, 20};  '
    tk = get_tokens(line)
    print(tk)

    line = "  let arr = [ 1, 2, 3, 4 , 5 ]; } let"
    tk = get_tokens(line)
    print(tk)

    # let p1 = Point{10, 20};
    line = "let p1 = Point {10, 20};"
    tk = get_tokens(line)
    print(tk)
