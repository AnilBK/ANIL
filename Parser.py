import lexer
from ErrorHandler import ErrorHandler


class Parser:
    def __init__(self, line: str) -> None:
        self.tokens = lexer.get_tokens(line)
        self.checkpoint_stack = []

    def has_tokens_remaining(self) -> bool:
        return len(self.tokens) > 0

    def current_token(self):
        return self.tokens[0]

    def peek_token(self):
        if len(self.tokens) >= 1:
            return self.tokens[1]
        else:
            return None

    def next_token(self):
        _ = self.tokens.pop(0)

    def get_token(self):
        return self.tokens.pop(0)

    def check_token(self, token: lexer.Token, p_custom_msg=None) -> bool:
        if not self.has_tokens_remaining():
            token_str = lexer.token_to_str(token)
            error_msg = (
                f'Expected {token}("{token_str}"), but no more tokens remaining.'
            )
            if p_custom_msg != None:
                error_msg += "\n" + p_custom_msg
            ErrorHandler().raise_error(error_msg)
        return self.current_token() == token

    def match_token(self, token: lexer.Token, p_custom_msg=None):
        if self.check_token(token, p_custom_msg):
            return True
        else:
            token_str = lexer.token_to_str(token)

            obtained_tk = self.current_token()
            obtained_tk_str = lexer.token_to_str(obtained_tk)
            if obtained_tk_str is None:
                obtained_tk_str = ""
            else:
                obtained_tk_str = f'("{obtained_tk_str}")'

            error_msg = f'Expected {token}("{token_str}") but got {obtained_tk}{obtained_tk_str}.'
            if p_custom_msg != None:
                error_msg += "\n" + p_custom_msg
            ErrorHandler().raise_error(error_msg)

    def consume_token(self, p_token: lexer.Token):
        """
        Expects the 'p_token' token.
        And, advances the parser to next token.
        Basically, we check for 'p_token' and then goto next token.
        """
        self.match_token(p_token)
        self.next_token()

    def extract_string_literal(self) -> str:
        """
        Extract the string defined inside the quotes "...".
        And, advances the parser to the next token.
        It expects the following Tokens: Token.QUOTE, Actual string, Token.QUOTE.
        """
        self.consume_token(lexer.Token.QUOTE)

        string = self.get_token()

        self.match_token(lexer.Token.QUOTE, "Quote Should be Closed.")
        self.next_token()

        return string
    
    # Checkpoint tokens.
    # While parsing, we consume tokens. If we find that we are parsing wrong thing(say expression),
    # we roll back to the point where, we need to reparse again with different kind of (expression)parser.
    # Since, consuming token removes the token, we create these checkpoints to recover the tokens that have been consumed.
    def save_checkpoint(self):
        self.checkpoint_stack.append(list(self.tokens))

    def rollback_checkpoint(self):
        if self.checkpoint_stack:
            self.tokens.clear()
            self.tokens = self.checkpoint_stack.pop()
              
    def clear_checkpoint(self):
        self.checkpoint_stack.clear()
