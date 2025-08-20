import pathlib
from dataclasses import dataclass
from typing import List, Optional, Tuple, Union

KEYWORDS = {
    "PROGRAM",
    "END",
    "FUNCTION",
    "RETURN",
    "INTEGER",
    "REAL",
    "IF",
    "THEN",
    "ELSE",
    "FI",
    "WHILE",
    "FOR",
    "BY",
    "DO",
    "SELECT",
    "CASE",
    "DECLARE",
    "SET",
    "OUTPUT",
    "INPUT",
    "EXIT",
    "FIX",
    "FLOAT",
    "TRUE",
    "FALSE",
}

SYMBOLS = {
    "+",
    "-",
    "*",
    "/",
    "(",
    ")",
    "[",
    "]",
    ";",
    ",",
    ".",
    ":",
    ":=",
    "=",
    "<>",
    "<",
    "<=",
    ">",
    ">=",
    "||",
}


@dataclass
class Token:
    type: str
    value: str
    line: int
    col: int


class LexerError(Exception):
    pass


class Lexer:
    def __init__(self, text: str):
        self.text = text
        self.i = 0
        self.line = 1
        self.col = 1
        self.n = len(text)

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
                        raise LexerError(
                            f"unterminated /* */ comment at line {self.line}"
                        )
                    self.advance(2)
                    return True
                return False

            if comments():
                continue
            break

    def number(self) -> Token:
        line, col = self.line, self.col
        s = ""
        while self.current().isdigit():
            s += self.current()
            self.advance()

        if self.current() == "." and self.peek().isdigit():
            s += "."
            self.advance()
            while self.current().isdigit():
                s += self.current()
                self.advance()
            return Token("REAL", s, line, col)
        return Token("INTEGER", s, line, col)

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
        value = v.lower()
        if value in KEYWORDS:
            return Token(value.upper(), value, line, col)
        return Token("IDENT", v, line, col)

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
                # doubles quotes for escape: 'it''s'
                if self.current() == quote:
                    s += quote
                    self.advance()
                    continue
                break
            s += c
            self.advance()
        return Token("STRING", s, line, col)

    def symbol(self) -> Token:
        start, col = self.line, self.col
        # try 2-char symbols first
        two = self.current() + self.peek()
        if two in SYMBOLS:
            self.advance(2)
            return Token(two, two, start, col)
        one = self.current()
        if one in SYMBOLS:
            self.advance()
            return Token(one, one, start, col)
        raise LexerError(f"unknown symbol '{one}' at {start}:{col}")

    def tokens(self) -> List[Token]:
        out: List[Token] = []
        while True:
            self.skip_whitespace_and_comments()
            if self.i >= self.n:
                break
            ch = self.current()
            if ch.isdigit():
                out.append(self.number())
            elif ch.isalpha() or ch == "_":
                out.append(self.ident_or_keyword())
            elif ch in '"':
                out.append(self.string())
            else:
                out.append(self.symbol())
        out.append(Token("EOF", "", self.line, self.col))
        return out


@dataclass
class Program:
    name: str
    block: "Segment"


@dataclass
class Segment:
    variables: List["VariableDeclaration"]
    functions: List["FunctionDeclaration"]
    statements: "Statements"


@dataclass
class VariableDeclaration:
    names: List[str]
    type: str


@dataclass
class ProcedureDeclaration:
    name: str
    parameters: List[Tuple[str, str]]
    segment: Segment


@dataclass
class FunctionDeclaration:
    name: str
    type: str
    parameters: List[Tuple[str, str]]
    segment: Segment


@dataclass
class Statements:
    statements: List["Statement"]


class Statement:
    pass


@dataclass
class Builtin:
    name: str
    args: List["Expression"]


@dataclass
class Assign(Statement):
    name: str
    expression: "Expression"


@dataclass
class If(Statement):
    cond: "Expression"
    then_branch: Statement
    else_branch: Optional[Statement]


@dataclass
class While(Statement):
    cond: "Expression"
    body: Statement


@dataclass
class Call(Statement):
    name: str
    args: List["Expression"]


@dataclass
class BlockStatement(Statement):
    compound: Statements  # for begin..end blocks


@dataclass
class Empty(Statement):
    pass


class Expression:
    pass


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression


@dataclass
class Variable(Expression):
    name: str


@dataclass
class IntegerLiteral(Expression):
    value: int


@dataclass
class RealLiteral(Expression):
    value: float


@dataclass
class StringLiteral(Expression):
    value: str


@dataclass
class BoolLiteral(Expression):
    value: bool


class ParseError(Exception):
    pass


class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.i = 0

    def current(self) -> Token:
        return self.tokens[self.i]

    # consume a token and advance if it matches the expected kind,
    # otherwise raise ParseError.
    def eat(self, kind: Union[str, Tuple[str, ...]]) -> Token:
        kinds = (kind,) if isinstance(kind, str) else kind
        token = self.current()
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        expected = "/".join(kinds)
        raise ParseError(
            f"expected '{expected}', found '{token.value}' "
            f"at {token.line}:{token.col}"
        )

    # consume a token if it matches the expected kind, without advancing,
    # otherwise return None
    def accept(self, kind: Union[str, Tuple[str, ...]]) -> Optional[Token]:
        token = self.current()
        kinds = (kind,) if isinstance(kind, str) else kind
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        return None

    def peek(self) -> Token:
        return (
            self.tokens[self.i + 1]
            if self.i + 1 < len(self.tokens)
            else Token("EOF", "", self.line, self.col)
        )

    # program -> 'program' IDENT ':' segments end program IDENT ';'
    def program(self) -> Program:
        self.eat("PROGRAM")
        name = self.eat("IDENT").value
        self.eat(":")
        segments = self.segment()
        self.eat("END")
        self.eat("PROGRAM")
        self.eat(name)
        self.eat(";")
        self.eat("EOF")
        return Program(name, segments)

    def segment(self) -> Segment:
        variables_declarations = self.variables_section()
        subroutines_declarations = self.subroutines_declarations()
        instructions = self.instructions()
        return Segment(
            variables_declarations,
            subroutines_declarations,
            instructions,
        )

    def variables_section(self) -> List[VariableDeclaration]:
        out: List[VariableDeclaration] = []
        while self.accept("DECLARE"):
            if self.current().type == "IDENT":
                # DECLARE v: INTEGER;
                name = self.eat("IDENT").value
                type_name = self.type()
                self.eat(";")
                out.append(VariableDeclaration([name], type_name))
                continue
            if self.current().value == "(":
                # DECLARE (v, x) REAL;
                self.eat("(")
                names = []
                while self.current().value != ")":
                    self.accept(",")
                    names.append(self.eat("IDENT").value)
                self.eat(")")
                type_name = self.type()
                self.eat(";")
                out.append(VariableDeclaration(names, type_name))
                continue
            raise ParseError(
                f"expected variable and or '(', found "
                f"'{self.current().value}' "
                f"at {self.current().line}:{self.current().col}"
            )
        return out

    def type(self) -> str:
        token = self.current()
        if token.type in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
            self.i += 1
            return token.value
        # allow IDENT types (simple aliases)
        return self.eat("IDENT").value

    def subroutines_declarations(self) -> List[ProcedureDeclaration]:
        out: List[ProcedureDeclaration] = []
        while self.accept("FUNCTION"):
            name = self.eat("IDENT").value
            parameters: List[Tuple[str, str]] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                self.eat(")")
            type = self.type()
            self.eat(":")
            segment = self.segment()
            self.eat("END")
            self.eat("FUNCTION")
            self.eat(name)
            self.eat(";")
            out.append(
                FunctionDeclaration(
                    name,
                    type,
                    parameters,
                    segment,
                )
            )
        return out

    def parameters(self) -> List[Tuple[str, str]]:
        out: List[Tuple[str, str]] = []
        while True:
            name = self.eat("IDENT").value
            type = self.type()
            out.append((name, type))
            if not self.accept(","):
                break
        return out

    def instructions(self) -> Statements:
        statements = self.statements()
        return Statements(statements)

    def statements(self) -> List[Statement]:
        statements: List[Statement] = []
        while self.current().value in ("SET",):
            statements.append(self.statement())
        return statements

    def statement(self) -> Statement:
        token = self.current()
        if token.value == "SET":
            return self.assignment_statement()
        return Empty()

    def procedure_statement(self) -> Call:
        name = self.eat("IDENT").value
        args: List[Expression] = []
        if self.accept("("):
            if self.current().type != ")":
                args = self.arg_list()
            self.eat(")")
        return Call(name, args)

    def arg_list(self) -> List[Expression]:
        args = [self.expression()]
        while self.accept(","):
            args.append(self.expression())
        return args

    def if_statement(self) -> If:
        self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.statement()
        else_branch = None
        if self.accept("ELSE"):
            else_branch = self.statement()
        return If(cond, then_branch, else_branch)

    def while_statement(self) -> While:
        self.eat("WHILE")
        cond = self.expression()
        self.eat("DO")
        body = self.statement()
        return While(cond, body)

    def assignment_statement(self) -> Assign:
        self.eat("SET")
        name = self.eat("IDENT").value
        self.eat(":=")
        expr = self.expression()
        self.eat(";")
        return Assign(name, expr)

    # def factor(self) -> Expression:
    #     token = self.current()
    #     if token.type == "INTEGER":
    #         self.i += 1
    #         return IntegerLiteral(token.value)
    #     if token.type == "REAL":
    #         self.i += 1
    #         return RealLiteral(token.value)
    #     if token.type == "BOOLEAN":
    #         self.i += 1
    #         return BoolLiteral(token.value)
    #     if token.type == "STRING":
    #         self.i += 1
    #         return StringLiteral(token.value)
    #     if token.type == "IDENT":
    #         self.i += 1
    #         return Variable(token.value)
    #     if token.value == "(":
    #         self.eat("(")
    #         expr = self.expression()
    #         self.eat(")")
    #         return expr

    #     if token.value == "NOT":
    #         self.i += 1
    #         return UnaryOperation("NOT", self.factor())
    #     raise ParseError(
    #         f"unexpected token '{token.value}' at " f"{token.line}:{token.col}"
    #     )

    def expression(self) -> Expression:
        return self.expression_or_xor()

    def expression_or_xor(self) -> Expression:
        left = self.expression_and()
        while operation := self.accept(("|", "XOR")):
            right = self.expression_and()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_and(self) -> Expression:
        left = self.expression_not()
        while operation := self.accept(("&",)):
            right = self.expression_not()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_not(self) -> Expression:
        if self.accept("NOT"):
            return UnaryOperation("NOT", self.expression_not())
        return self.expression_relation()

    def expression_relation(self) -> Expression:
        left = self.expression_concatenation()
        while operation := self.accept(("<", ">", "=", "<=", ">=", "<>")):
            right = self.expression_concatenation()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_concatenation(self) -> Expression:
        left = self.expression_adding()
        while operation := self.accept(("||",)):
            right = self.expression_adding()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_adding(self) -> Expression:
        left = self.expression_multiplying()
        while operation := self.accept(("+", "-")):
            right = self.expression_multiplying()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_multiplying(self) -> Expression:
        left = self.expression_builtin()
        while operation := self.accept(("*", "/", "MOD")):
            right = self.expression_builtin()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_builtin(self) -> Expression:
        name = self.current().value
        if name in (
            "FLOOR",
            "LENGTH",
            "SUBSTR",
            "CHARACTER",
            "NUMBER",
            "FLOAT",
            "FIX",
        ):
            self.i += 1
            self.eat("(")
            args = [self.expression()]
            if name == "SUBSTR":
                self.eat(",")
                args.append(self.expression())
                self.eat(",")
                args.append(self.expression())
            self.eat(")")
            return Builtin(name, args)
        return self.factor()

    def expression_one(self) -> Expression | None:
        if v := self.expression_two():
            return v
        left = self.expression_one()
        if self.accept("&"):
            return BinaryOperation("&", left, self.expression_two())
        raise ParseError(
            f"expected '&' binary operator at "
            f"{self.current().line}:{self.current().col}"
        )

    def expression_two(self) -> Expression | None:
        if v := self.expression_three():
            return v
        if self.accept("NOT"):
            return UnaryOperation("NOT", self.expression_three())
        raise ParseError(
            f"expected 'NOT' unary operator at "
            f"{self.current().line}:{self.current().col}"
        )

    def expression_three(self) -> Expression | None:
        if v := self.expression_four():
            return v
        left = self.expression_three()
        operation = self.eat(("<", ">", "=", "<=", ">=", "<>")).value
        right = self.expression_four()
        return BinaryOperation(operation, left, right)

    def expression_four(self) -> Expression | None:
        if v := self.expression_five():
            return v
        left = self.expression_four()
        self.eat("||")
        right = self.expression_five()
        return BinaryOperation("||", left, right)

    def expression_five(self) -> Expression | None:
        if self.accept(("+", "-")):
            operation = self.current().value
            self.i += 1
            return UnaryOperation(operation, self.expression_six())
        next = self.peek()
        if next.value in ("+", "-"):
            left = self.expression_five()
            operation = self.eat(("+", "-")).value
            right = self.expression_six()
            return BinaryOperation(operation, left, right)
        return self.expression_six()

    def expression_six(self) -> Expression | None:
        if v := self.expression_seven():
            return v
        left = self.expression_six()
        self.eat(("*", "/", "MOD"))
        right = self.expression_seven()
        return BinaryOperation("&&", left, right)

    def expression_seven(self) -> Expression | None:
        return self.expression_eight()

    def expression_eight(self) -> Expression | None:
        token = self.current()
        if token.value == "(":
            self.i += 1
            v = self.expression()
            self.eat(")")
            return v
        if token.type == "INTEGER" or token.type == "REAL":
            self.i += 1
            return (
                IntegerLiteral(int(token.value))
                if token.type == "INTEGER"
                else RealLiteral(float(token.value))
            )
        name = self.eat("IDENT").value
        return Variable(name)

    # -------------------------------------
    def X():

        # simple_expression -> term (relop term)?
        left = self.simple_expression()
        token = self.current()
        if token.type in ("=", "<>", "<", "<=", ">", ">="):
            operation = token.type
            self.i += 1
            right = self.simple_expression()
            return BinaryOperation(operation, left, right)
        return left

    # simple_expression -> term (addop term)*
    # addop -> '+' | '-' | 'or'
    def simple_expression(self) -> Expression:
        node = self.term()
        while self.current().type in ("+", "-", "OR"):
            operation = self.current().type
            self.i += 1
            right = self.term()
            node = BinaryOperation(operation, node, right)
        return node

    # term -> factor (mulop factor)*
    # mulop -> '*' | '/' | 'div' | 'mod' | 'and'
    def term(self) -> Expression:
        node = self.factor()
        while self.current().type in ("*", "/", "DIV", "MOD", "AND"):
            operation = self.current().type
            self.i += 1
            right = self.factor()
            node = BinaryOperation(operation, node, right)
        return node

    def factor(self) -> Expression:
        token = self.current()
        if token.type == "IDENT":
            self.i += 1
            return Variable(token.value)
        if token.type == "INTEGER":
            self.i += 1
            return IntegerLiteral(int(token.value))
        if token.type == "REAL":
            self.i += 1
            return RealLiteral(float(token.value))
        if token.type == "STRING":
            self.i += 1
            return StringLiteral(token.value)
        # if token.type == "NOT":
        #     self.i += 1
        #     return UnaryOperation("NOT", self.factor())
        if token.type == "+":
            self.i += 1
            return UnaryOperation("+", self.factor())
        if token.type == "-":
            self.i += 1
            return UnaryOperation("-", self.factor())
        if token.type == "(":
            self.i += 1
            e = self.expression()
            self.eat(")")
            return e
        if token.type in ("TRUE", "FALSE"):
            self.i += 1
            return BoolLiteral(token.type == "TRUE")
        if token.type == "IDENT" and token.value.lower() in ("true", "false"):
            self.i += 1
            return BoolLiteral(token.value.lower() == "true")
        raise ParseError(
            f"unexpected token {token.type}('{token.value}') at "
            f"{token.line}:{token.col} in factor"
        )


def indent(s: str, n: int) -> str:
    pad = "  " * n
    return "\n".join(pad + line for line in s.splitlines())


def dump(node, level=0) -> str:
    if isinstance(node, Program):
        return indent(
            f"Program {node.name}\n{dump(node.block, level+1)}",
            level,
        )
    if isinstance(node, Segment):
        parts = []
        if node.variables:
            parts.append("Variables:")
            for v in node.variables:
                parts.append(f"  {', '.join(v.names)}: {v.type}")
        if node.functions:
            parts.append("Functions:")
            for p in node.functions:
                params = ", ".join(f"{n}:{t}" for n, t in p.parameters)
                parts.append(
                    indent(
                        f"function {p.name}({params})\n"
                        f"{dump(p.segment, level)}",
                        1,
                    )
                )
        parts.append("Statements:")
        parts.append(indent(dump(node.statements, level + 1), 1))
        return indent("\n".join(parts), level)
    if isinstance(node, Statements):
        return (
            "\n".join(dump(s, level + 1) for s in node.statements)
            if node.statements
            else "(empty)"
        )
    if isinstance(node, BlockStatement):
        return "BlockStatementt:\n" + indent(dump(node.compound, level + 1), 1)
    if isinstance(node, Assign):
        return f"Assign {node.name} := {dump(node.expression, level+1)}"
    if isinstance(node, If):
        s = f"If {dump(node.cond)} then {dump(node.then_branch)}"
        if node.else_branch:
            s += f" else {dump(node.else_branch)}"
        return s
    if isinstance(node, While):
        return f"While {dump(node.cond)} do {dump(node.body)}"
    if isinstance(node, Call):
        return f"Call {node.name}({', '.join(dump(a) for a in node.args)})"
    if isinstance(node, Empty):
        return "Empty"
    if isinstance(node, BinaryOperation):
        return f"({dump(node.left)} {node.operation} {dump(node.right)})"
    if isinstance(node, UnaryOperation):
        return f"({node.operation}{dump(node.expr)})"
    if isinstance(node, Variable):
        return node.name
    if isinstance(node, IntegerLiteral):
        return str(node.value)
    if isinstance(node, RealLiteral):
        return str(node.value)
    if isinstance(node, StringLiteral):
        return repr(node.value)
    if isinstance(node, BoolLiteral):
        return "true" if node.value else "false"
    if isinstance(node, Builtin):
        return f"{node.name}({', '.join(dump(a) for a in node.args)})"
    return repr(node)


def build_ast(code: str) -> Program:
    lexer = Lexer(code)
    tokens = lexer.tokens()
    return Parser(tokens).program()


def parse() -> Program:
    code = pathlib.Path("era-wip.easy").read_text()
    ast = build_ast(code)
    return ast


if __name__ == "__main__":
    print(dump(parse()))
