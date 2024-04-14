import lexer


class Parser:
    def __init__(self, line: str) -> None:
        self.tokens = lexer.get_tokens(line)

    def has_tokens_remaining(self) -> bool:
        return len(self.tokens) > 0

    def current_token(self):
        return self.tokens[0]

    def next_token(self):
        _ = self.tokens.pop(0)

    def get_token(self):
        return self.tokens.pop(0)

    def check_token(self, token: lexer.Token) -> bool:
        return self.current_token() == token

    def match_token(self, token: lexer.Token, p_custom_msg=None):
        if self.check_token(token):
            return True
        else:
            error_msg = f"Expected token {token} but got {self.current_token()}."
            if p_custom_msg != None:
                error_msg += "\n" + p_custom_msg
            raise ValueError(error_msg)

    def extract_string_literal(self) -> str:
        """
        Extract the string defined inside the quotes "...".
        And, advances the parser to the next token.
        It expects the following Tokens: Token.QUOTE, Actual string, Token.QUOTE.
        """
        self.match_token(lexer.Token.QUOTE)
        self.next_token()

        string = self.get_token()
        self.match_token(lexer.Token.QUOTE, "Quote Should be Closed.")
        self.next_token()

        return string
