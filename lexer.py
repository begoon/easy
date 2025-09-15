from dataclasses import dataclass
from pathlib import Path

KEYWORDS = {
    "PROGRAM",
    "BEGIN",
    "END",
    #
    "TYPE",
    "IS",
    "DECLARE",
    "FUNCTION",
    "PROCEDURE",
    "SET",
    #
    "CALL",
    "RETURN",
    "EXIT",
    #
    "INPUT",
    "OUTPUT",
    #
    "INTEGER",
    "REAL",
    "BOOLEAN",
    "STRING",
    #
    "IF",
    "THEN",
    "ELSE",
    "FI",
    #
    "FOR",
    "WHILE",
    "BY",
    "DO",
    #
    "SELECT",
    "CASE",
    "OTHERWISE",
    #
    "TRUE",
    "FALSE",
}

SYMBOLS = {*"+ - * / | & ( ) [ ] ; , . : := = <> < <= > >= ||".split(" ")}


@dataclass
class InputText:
    filename: str
    text: str

    def __init__(self, *, text: str = "", filename: str | Path = None):
        if text:
            self.text = text
            self.filename = ""
        else:
            self.text = Path(filename).read_text()
            self.filename = str(filename)


@dataclass
class Token:
    type: str
    value: str

    line: int
    col: int

    input: InputText

    def __str__(self) -> str:
        v = "<" + self.value
        if self.type != self.value:
            v += f"|{self.type}"
        v += f"|{self.input.filename}:{self.line}:{self.col}"
        return v

    def __repr__(self) -> str:
        return str(self)


class LexerError(Exception):
    pass


class Lexer:
    def __init__(self, input: InputText):
        self.input = input
        self.text = input.text
        self.i = 0
        self.line = 1
        self.col = 1
        self.n = len(self.text)

    def peek(self, k=1) -> str:
        j = self.i + k
        return self.text[j] if j < self.n else ""

    def current(self) -> str:
        return self.text[self.i] if self.i < self.n else ""

    def advance(self, k=1):
        for _ in range(k):
            if self.i < self.n:
                ch = self.text[self.i]
                self.i += 1
                if ch == "\n":
                    self.line += 1
                    self.col = 1
                else:
                    self.col += 1

    def skip_whitespace_and_comments(self):
        while True:
            # whitespace
            while self.current() and self.current().isspace():
                self.advance()

            # comments /* ... */
            def comments():
                if self.current() == "/" and self.peek() == "*":
                    self.advance(2)
                    while not (self.current() == "*" and self.peek() == "/"):
                        if comments():
                            continue
                        self.advance()
                    if self.i >= self.n:
                        raise LexerError(f"unterminated /* */ comment at line {self.line}")
                    self.advance(2)
                    return True
                return False

            if comments():
                continue

            # "//" comments
            if self.current() == "/" and self.peek() == "/":
                self.advance(2)
                while (c := self.current()) and c != "\n":
                    self.advance()
                self.advance()
                continue

            break

    def number(self) -> Token:
        line, col = self.line, self.col
        s = ""
        while self.current().isdigit():
            s += self.current()
            self.advance()

        if self.current() == "." or self.current() == "e":
            s += self.current()
            self.advance()
            while self.current().isdigit() or self.current() in "+-eE":
                s += self.current()
                self.advance()
            return Token("REAL", s, line, col, self.input)
        return Token("INTEGER", s, line, col, self.input)

    def ident_or_keyword(self) -> Token:
        line, col = self.line, self.col
        v = ""
        ch = self.current()
        if ch.isalpha() or ch == "_":
            v += ch
            self.advance()
            while self.current().isalnum() or self.current() == "_":
                v += self.current()
                self.advance()
        value = v
        if value in KEYWORDS:
            return Token("KEYWORD", value, line, col, self.input)
        return Token("IDENT", v, line, col, self.input)

    def string(self) -> Token:
        line, col = self.line, self.col
        quote = self.current()
        self.advance()
        s = ""
        while True:
            c = self.current()
            if not c:
                raise LexerError(f"unterminated string at line {line}")
            if c == quote:
                self.advance()
                # doubles quotes for escape: "it""s"
                if self.current() == quote:
                    s += quote
                    self.advance()
                    continue
                break
            s += c
            self.advance()
        return Token("STRING", s, line, col, self.input)

    def symbol(self) -> Token:
        start, col = self.line, self.col
        # try 2-char symbols first
        two = self.current() + self.peek()
        if two in SYMBOLS:
            self.advance(2)
            return Token("SYMBOL", two, start, col, self.input)
        one = self.current()
        if one in SYMBOLS:
            self.advance()
            return Token("SYMBOL", one, start, col, self.input)
        raise LexerError(f"unknown symbol '{one}' at {self.input.filename}:{start}:{col}")

    def tokens(self) -> list[Token]:
        v: list[Token] = []
        while True:
            self.skip_whitespace_and_comments()
            if self.i >= self.n:
                break
            ch = self.current()
            if ch.isdigit():
                v.append(self.number())
            elif ch.isalpha() or ch == "_":
                v.append(self.ident_or_keyword())
            elif ch in '"':
                v.append(self.string())
            else:
                v.append(self.symbol())
        v.append(Token("EOF", "", self.line, self.col, self.input))
        return v
