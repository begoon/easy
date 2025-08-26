import pathlib
import sys
from dataclasses import dataclass
from typing import List, Literal, Optional, Tuple, Union, cast

import pytest

KEYWORDS = {
    "PROGRAM",
    "END",
    "FUNCTION",
    "RETURN",
    "EXIT",
    "INPUT",
    "OUTPUT",
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

SYMBOLS = {*"+ - * / ( ) [ ] ; , . : := = <> < <= > >= ||".split(" ")}


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
                        raise LexerError(f"unterminated /* */ comment at line {self.line}")
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

        if self.current() == ".":
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


Subroutine = Union["Function", "Procedure"]


@dataclass
class Segment:
    variables: List["Declaration"]
    subroutines: List[Subroutine]
    statements: "Statements"


@dataclass
class Declaration:
    names: List[str]
    type: Union[str, "Array"]


@dataclass
class Array:
    type: "Type"
    start: Optional["Expression"]
    end: "Expression"


Type = Literal["INTEGER", "REAL", "STRING", "BOOLEAN"] | Array


@dataclass
class Procedure:
    name: str
    parameters: List[Tuple[str, Union[str, Array]]]
    segment: Segment


@dataclass
class Function:
    name: str
    type: Union[str, Array]
    parameters: List[Tuple[str, Union[str, Array]]]
    segment: Segment


@dataclass
class Statements:
    statements: List["Statement"]


class Statement:
    pass


@dataclass
class Assign(Statement):
    name: str
    expression: "Expression"
    index: Optional["Expression"] = None


@dataclass
class If(Statement):
    cond: "Expression"
    then_branch: Segment
    else_branch: Optional[Segment]


@dataclass
class For(Statement):
    variable: "Variable"
    init: "Expression"
    do: Segment
    by: Optional["Expression"] = None
    to: Optional["Expression"] = None
    while_: Optional["Expression"] = None


@dataclass
class Select(Statement):
    expr: "Expression"
    cases: List[Tuple["Expression", Segment]]


@dataclass
class Input(Statement):
    variables: List[str]


@dataclass
class Output(Statement):
    expressions: List["Expression"]


@dataclass
class Call(Statement):
    name: str
    args: List["Expression"]


@dataclass
class Return(Statement):
    value: Optional["Expression"] = None


@dataclass
class Exit(Statement):
    pass


@dataclass
class Empty(Statement):
    pass


@dataclass
class Expression:
    pass


@dataclass
class FunctionCall(Expression):
    name: str
    args: List["Expression"]


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression


@dataclass
class ConcatenationOperation(Expression):
    parts: list[Expression]


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression


@dataclass
class Variable(Expression):
    name: str
    index: Optional[Expression] = None


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
    def __init__(self, message: str, token: Token, source: str):
        super().__init__(message)
        self.message = message
        self.token = token
        self.source = source

    def __str__(self) -> str:
        error_line = self.source.splitlines()[self.token.line - 1]
        return f"{self.message} at {self.token.line}:{self.token.col}\n{error_line}\n{' ' * (self.token.col - 1)}^"


class Parser:
    def __init__(self, tokens: List[Token], source: str):
        self.tokens = tokens
        self.i = 0
        self.source = source

    def error(self, message: str, token: Token) -> None:
        raise ParseError(message, token, self.source)

    def current(self) -> Token:
        return self.tokens[self.i]

    def eat(self, kind: Union[str, Tuple[str, ...]]) -> Token:
        kinds = (kind,) if isinstance(kind, str) else kind
        token = self.current()
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        expected = "/".join(kinds)
        self.error(f"expected '{expected}', found '{token.value}'", token)

    def accept(self, kind: Union[str, Tuple[str, ...]]) -> Optional[Token]:
        token = self.current()
        kinds = (kind,) if isinstance(kind, str) else kind
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        return None

    def peek(self) -> Token:
        current = self.current()
        return self.tokens[self.i + 1] if self.i + 1 < len(self.tokens) else Token("EOF", "", current.line, current.col)

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
        variables = self.variables_section()
        subroutines = self.subroutines()
        statements = self.statements()
        return Segment(variables, subroutines, statements)

    def variables_section(self) -> List[Declaration]:
        out: List[Declaration] = []
        while self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.type()
                self.eat(";")
                out.append(Declaration([name], type))
                continue
            if token.value == "(":
                self.eat("(")
                names = []
                while self.current().value != ")":
                    self.accept(",")
                    names.append(self.eat("IDENT").value)
                self.eat(")")
                type = self.type()
                self.eat(";")
                out.append(Declaration(names, type))
                continue
            self.error(f"expected variable and or '(', found '{token.value}' ", token)
        return out

    def type(self) -> str | Array | Type:
        token = self.current()
        if token.type in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
            self.i += 1
            return token.value
        if token.value == "ARRAY":
            self.i += 1
            self.eat("[")
            end = self.expression()
            start = None
            if self.accept(":"):
                start = end
                end = self.expression()
            self.eat("]")
            self.eat("OF")
            type = cast(Array, self.type())
            return Array(type, start, end)
        return self.eat("IDENT").value

    def subroutines(self) -> List[Procedure | Function]:
        out: List[Procedure | Function] = []
        while self.accept("FUNCTION"):
            name = self.eat("IDENT").value
            parameters: List[Tuple[str, Union[str, "Array"]]] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                self.eat(")")
            type = self.type()
            self.eat(":")
            segment = self.segment()
            segment.statements.statements.append(Return(0))
            self.eat("END")
            self.eat("FUNCTION")
            self.eat(name)
            self.eat(";")
            out.append(Function(name, type, parameters, segment))
        return out

    def parameters(self) -> List[Tuple[str, Union[str, "Array"]]]:
        out: List[Tuple[str, Union[str, "Array"]]] = []
        while True:
            name = self.eat("IDENT").value
            type = self.type()
            out.append((name, type))
            if not self.accept(","):
                break
        return out

    def statements(self) -> Statements:
        statements: List[Statement] = []
        while self.current().value in (
            "SET",
            "IF",
            "RETURN",
            "EXIT",
            "INPUT",
            "OUTPUT",
            "FOR",
            "SELECT",
            ";",
        ):
            statements.append(self.statement())
        return Statements(statements)

    def statement(self) -> Statement:
        token = self.current()
        if token.value == "SET":
            return self.assignment_statement()
        if token.value == "IF":
            return self.if_statement()
        if token.value == "FOR":
            return self.for_statement()
        if token.value == "SELECT":
            return self.select_statement()
        if token.value == "RETURN":
            return self.return_statement()
        if token.value == "EXIT":
            return self.exit_statement()
        if token.value == "INPUT":
            return self.input_statement()
        if token.value == "OUTPUT":
            return self.output_statement()
        self.eat(";")
        return Empty()

    def arguments(self) -> List[Expression]:
        args = [self.expression()]
        while self.accept(","):
            args.append(self.expression())
        return args

    def if_statement(self) -> If:
        self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.segment()
        else_branch = None
        if self.accept("ELSE"):
            else_branch = self.segment()
        self.eat("FI")
        self.eat(";")
        return If(cond, then_branch, else_branch)

    def for_statement(self) -> For:
        self.eat("FOR")
        variable = Variable(self.eat("IDENT").value)
        self.eat(":=")
        init = self.expression()
        by = None
        if self.accept("BY"):
            by = self.expression()
        to = None
        if self.accept("TO"):
            to = self.expression()
        while_ = None
        if self.accept("WHILE"):
            while_ = self.expression()
        self.eat("DO")
        do = self.segment()
        self.eat("END")
        self.eat("FOR")
        self.eat(";")
        return For(variable, init, do, by, to, while_)

    def select_statement(self) -> Select:
        self.eat("SELECT")
        expr = self.expression()
        self.eat("OF")
        cases = []
        while self.accept("CASE"):
            self.eat("(")
            cond = self.expression()
            self.eat(")")
            self.eat(":")
            body = self.segment()
            cases.append((cond, body))
        self.eat("END")
        self.eat("SELECT")
        self.eat(";")
        return Select(expr, cases)

    def return_statement(self, function: bool = True) -> Return:
        self.eat("RETURN")
        if function:
            value = self.expression()
        self.eat(";")
        return Return(value)

    def exit_statement(self) -> Exit:
        self.eat("EXIT")
        self.eat(";")
        return Exit()

    def input_statement(self) -> Input:
        self.eat("INPUT")
        variables = []
        variables.append(self.eat("IDENT").value)
        while self.accept(","):
            variables.append(self.eat("IDENT").value)
        self.eat(";")
        return Input(variables)

    def output_statement(self) -> Output:
        self.eat("OUTPUT")
        expressions = [self.expression()]
        while self.accept(","):
            expressions.append(self.expression())
        self.eat(";")
        return Output(expressions)

    def assignment_statement(self) -> Assign:
        self.eat("SET")
        name = self.eat("IDENT").value
        index = None
        if self.accept("["):
            index = self.expression()
            self.eat("]")
        self.eat(":=")
        expr = self.expression()
        self.eat(";")
        return Assign(name, expr, index)

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
            if operation.value == "=":
                operation = Token("=", "==", operation.line, operation.col)
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_concatenation(self) -> Expression:
        parts = [self.expression_adding()]
        while self.accept(("||",)):
            parts.append(self.expression_adding())
        if len(parts) == 1:
            return parts[0]
        return ConcatenationOperation(parts)

    def expression_adding(self) -> Expression:
        left = self.expression_multiplying()
        while operation := self.accept(("+", "-")):
            right = self.expression_multiplying()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_multiplying(self) -> Expression:
        left = self.expression_function_call()
        while operation := self.accept(("*", "/", "MOD")):
            right = self.expression_function_call()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_function_call(self) -> Expression | FunctionCall:
        name = self.current()
        if name.type == "IDENT" and self.peek().value == "(":
            self.i += 1
            self.eat("(")
            args = self.arguments()
            self.eat(")")
            return FunctionCall(name.value, args)
        return self.factor()

    def factor(self) -> Expression:
        token = self.current()
        if token.type == "IDENT":
            self.i += 1
            index = None
            if self.accept("["):
                index = self.expression()
                self.eat("]")
            return Variable(token.value, index)
        if token.type == "INTEGER":
            self.i += 1
            return IntegerLiteral(int(token.value))
        if token.type == "REAL":
            self.i += 1
            return RealLiteral(float(token.value))
        if token.type == "STRING":
            self.i += 1
            return StringLiteral(token.value)
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
        self.error(f"unexpected token {token.type}('{token.value}')", token)


def indent(s: str, n: int) -> str:
    pad = "    " * n
    return "\n".join(pad + line for line in s.splitlines())


def dump(node, level=0) -> str:
    if isinstance(node, Program):
        return indent(f"Program {node.name}\n{dump(node.block, level+1)}", level)
    if isinstance(node, Segment):
        parts = []
        if node.variables:
            parts.append("Variables:")
            for v in node.variables:
                parts.append(f"  {', '.join(v.names)}: {v.type}")
        if node.subroutines:
            parts.append("Functions:")
            for p in node.subroutines:
                params = ", ".join(f"{n}:{t}" for n, t in p.parameters)
                parts.append(indent(f"function {p.name}({params})\n" f"{dump(p.segment, level)}", 1))
        parts.append("Statements:")
        parts.append(indent(dump(node.statements, level + 1), 1))
        return indent("\n".join(parts), level)
    if isinstance(node, Statements):
        return "\n".join(dump(s, level + 1) for s in node.statements) if node.statements else "(empty)"
    if isinstance(node, Assign):
        return " ".join(
            [
                f"Assign {node.name}{"["+dump(node.index) + "]" if node.index else ""}",
                ":=",
                dump(node.expression, level + 1),
            ],
        )
    if isinstance(node, If):
        s = f"If {dump(node.cond)} Then\n"
        s += indent(f"{dump(node.then_branch)}\n", 1)
        if node.else_branch:
            s += "\nElse\n"
            s += indent(f"{dump(node.else_branch)}\n", 1)
        s += "\nFi"
        return s
    if isinstance(node, Call):
        return f"Call {node.name}({', '.join(dump(a) for a in node.args)})"
    if isinstance(node, Return):
        return f"Return {dump(node.value)}"
    if isinstance(node, Exit):
        return "Exit"
    if isinstance(node, Input):
        return f"Input({', '.join(dump(e) for e in node.variables)})"
    if isinstance(node, Output):
        return f"Output({', '.join(dump(e) for e in node.expressions)})"
    if isinstance(node, Empty):
        return "Empty"
    if isinstance(node, BinaryOperation):
        return f"({dump(node.left)} {node.operation} {dump(node.right)})"
    if isinstance(node, UnaryOperation):
        return f"({node.operation}{dump(node.expr)})"
    if isinstance(node, Variable):
        return node.name + (f"[{dump(node.index)}]" if node.index else "")
    if isinstance(node, IntegerLiteral):
        return str(node.value)
    if isinstance(node, RealLiteral):
        return str(node.value)
    if isinstance(node, StringLiteral):
        return repr(node.value)
    if isinstance(node, BoolLiteral):
        return "true" if node.value else "false"
    if isinstance(node, FunctionCall):
        return f"{node.name}({', '.join(dump(a) for a in node.args)})"
    if isinstance(node, Array):
        if node.start:
            return f"Array({dump(node.type)}, {dump(node.start)}, {dump(node.end)})"
        return f"Array({dump(node.type)}, {dump(node.end)})"
    if isinstance(node, For):
        header = "".join(
            [
                f"For {dump(node.variable)} := ",
                dump(node.init),
                " ",
                f"By {dump(node.by)} " if node.by else "",
                f"To {dump(node.to)} " if node.to else "",
                f"While {dump(node.while_)} " if node.while_ else "",
                "Do",
            ]
        )
        footer = "End For"
        return f"{header}\n{dump(node.do, 1)}\n{footer}"
    if isinstance(node, Select):
        s = f"Select {dump(node.expr)}\n"
        for cond, body in node.cases:
            cond = generate(cond)
            cond = cond[1:-1]
            s += f"  Case ({dump(cond)}):\n{dump(body, 2)}\n"
        s += "End Select"
        return s
    if isinstance(node, ConcatenationOperation):
        parts = [format_concat_part(v) for v in node.parts]
        return f"concat({len(parts)}, {', '.join(parts)})"
    return repr(node)


def format_concat_part(v: Expression) -> str:
    str = generate(v)
    if not str.startswith("'") and not str.startswith("concat"):
        str = f"str({str})"
    return str


def BOOL(v: str) -> str:
    return {"TRUE": "1", "FALSE": "0"}[v]


def TYPE(v: str) -> str:
    if isinstance(v, Array):
        return v
    return {"INTEGER": "int", "REAL": "double", "BOOLEAN": "int"}[v]


functions = ""


def generate(node, level=0) -> str:
    global functions

    if isinstance(node, Program):
        return indent("int main()\n{\n" + generate(node.block, level + 1) + "\n}", level)

    if isinstance(node, Segment):
        parts = []
        if node.variables:
            for v in node.variables:
                if isinstance(v.type, Array):
                    size = generate(v.type.end)
                    offset = generate(v.type.start) or "0"
                    parts.append(f"{TYPE(v.type.type)} {', '.join(v.names)}[{size} + {offset}];")
                else:
                    parts.append(f"{TYPE(v.type)} {', '.join(v.names)};")
        if node.subroutines:
            side_parts = []
            for subroutine in node.subroutines:
                params = ", ".join(f"{TYPE(t)} {n}" for n, t in subroutine.parameters)
                side_parts.append(
                    indent(
                        f"{TYPE(subroutine.type)} {subroutine.name}({params})\n{{\n"
                        f"{generate(subroutine.segment, level)}\n}}",
                        0,
                    )
                )
            functions += "\n".join(side_parts)
        parts.append(indent(generate(node.statements, level + 1), 0))
        return indent("\n".join(parts), level)
    if isinstance(node, Statements):
        return "\n".join(generate(s, level + 1) for s in node.statements) if node.statements else "(empty)"
    if isinstance(node, Assign):
        return (
            node.name
            + (f"[{generate(node.index)}]" if node.index else "")
            + " = "
            + generate(node.expression, level + 1)
            + ";"
        )
    if isinstance(node, If):
        cond = generate(node.cond)
        if cond.startswith("(") and cond.endswith(")"):
            cond = cond[1:-1]
        s = f"if ({cond})\n{{\n"
        s += indent(f"{generate(node.then_branch)}\n", 1)
        if node.else_branch:
            s += "\n}\nelse\n{\n"
            s += indent(f"{generate(node.else_branch)}\n", 1)
        s += "\n}"
        return s
    if isinstance(node, Call):
        raise NotImplementedError()
        return f"{node.name}({', '.join(generate(a) for a in node.args)});"
    if isinstance(node, Return):
        return f"return {generate(node.value)};"
    if isinstance(node, Exit):
        return "exit(0);"
    if isinstance(node, Input):
        inputs = []
        for variable in node.variables:
            inputs.append(f'scanf("%d", &{variable});')
        return "\n".join(inputs)
    if isinstance(node, Output):
        return 'printf("%s\\n", ' + ", ".join(generate(e).replace("'", '"') for e in node.expressions) + ");"
    if isinstance(node, Empty):
        return ";"
    if isinstance(node, BinaryOperation):
        return f"({generate(node.left)} {node.operation} {generate(node.right)})"
    if isinstance(node, UnaryOperation):
        return f"({node.operation}{generate(node.expr)})"
    if isinstance(node, Variable):
        return node.name + (node.index and f"[{generate(node.index)}]" or "")
    if isinstance(node, IntegerLiteral):
        return str(node.value)
    if isinstance(node, RealLiteral):
        return str(node.value)
    if isinstance(node, StringLiteral):
        return repr(node.value)
    if isinstance(node, BoolLiteral):
        return "1" if node.value else "0"
    if isinstance(node, FunctionCall):
        return f"{node.name}({', '.join(generate(a) for a in node.args)})"
    if isinstance(node, Array):
        if node.start:
            return f"Array({generate(node.type)}, {generate(node.start)}, {generate(node.end)})"
        return f"Array({generate(node.type)}, {generate(node.end)})"
    if isinstance(node, For):

        def init() -> str:
            return generate(node.variable) + " = " + generate(node.init)

        def condition() -> str:
            conditions = []
            if node.while_:
                conditions.append(generate(node.while_))
            if node.to:
                conditions.append(f"{generate(node.variable)} <= {generate(node.to)}")
            return "".join(conditions)

        def step() -> str:
            return f"{generate(node.variable)} += " + ("1" if not node.by else generate(node.by))

        return "".join(
            [
                "for (" + init() + "; " + condition() + "; " + step() + ")\n{",
                "\n",
                generate(node.do, 1),
                "\n",
                "}",
            ],
        )
    if isinstance(node, Select):
        s = ""
        for cond, body in node.cases:
            cond = generate(cond)
            s += f"\nif {cond}\n{{\n{generate(body, 1)}\n}}"
        return s.strip()
    if isinstance(node, Array):
        return TYPE(node.type) + "[" + generate(node.end) + "];"
    if isinstance(node, ConcatenationOperation):
        parts = [format_concat_part(v) for v in node.parts]
        return f"concat({len(parts)}, {', '.join(parts)})"
    return repr(node)


def parse(code: str) -> Program:
    lexer = Lexer(code)
    tokens = lexer.tokens()
    return Parser(tokens, code).program()


# ---


@pytest.mark.parametrize(
    "input, expected",
    [
        ("a", [("IDENT", "a"), ("EOF", "")]),
        ("a b", [("IDENT", "a"), ("IDENT", "b"), ("EOF", "")]),
        ("a /* b /* \n */ */ c", [("IDENT", "a"), ("IDENT", "c"), ("EOF", "")]),
    ],
)
def test_tokens(input, expected) -> None:
    lexer = Lexer(input)
    tokens = lexer.tokens()
    assert [(t.type, t.value) for t in tokens] == expected


@pytest.mark.parametrize(
    "input, expected",
    [
        ("a", "a"),
        ("a + b", "(a + b)"),
        ("+a", "(+a)"),
        ("-a", "(-a)"),
        ("a + b * с", "(a + (b * с))"),
        (
            '-b + (1 - 2) - LENGTH("3") * 7 XOR 1',
            "((((-b) + (1 - 2)) - (LENGTH('3') * 7)) XOR 1)",
        ),
        ('-b + (1 - 2) - adhoc("3")', "(((-b) + (1 - 2)) - adhoc('3'))"),
        ("NOT a < b", "(NOT(a < b))"),
        ("a || b", "concat(2, str(a), str(b))"),
        ('"a" || b', "concat(2, 'a', str(b))"),
        ('"a" || b || c', "concat(3, 'a', str(b), str(c))"),
        (
            'a || (b || SUBSTR("abc", 0, 2))',
            "concat(2, str(a), concat(2, str(b), str(SUBSTR('abc', 0, 2))))",
        ),
        (
            'a || b || SUBSTR("abc", 0, 2)',
            "concat(3, str(a), str(b), str(SUBSTR('abc', 0, 2)))",
        ),
    ],
)
def test_expression(input, expected) -> None:
    result = Parser(Lexer(input).tokens(), input).expression()
    if not isinstance(expected, str):
        expected = dump(expected)
    assert dump(result) == expected, f"\n{expected}\n!=\n{result}\n"


PROGRAM = """
PROGRAM Eratosthenes:

  DECLARE topnum INTEGER;

  FUNCTION abs(x REAL) REAL:
    IF x < 0 THEN RETURN -x; ELSE RETURN x; FI;
  END FUNCTION abs;

  FUNCTION integersqrt(a INTEGER) INTEGER:
    SELECT TRUE OF
      CASE (a < 0): OUTPUT "a < 0 in FUNCTION integersqrt."; EXIT;
      CASE (a = 0): RETURN 0;
      CASE (a > 0):
        DECLARE (x, ra) REAL;
        DECLARE epsilon REAL;
        DECLARE sqrt INTEGER;
        SET ra := FLOAT(a);
        SET epsilon := 0.0000001*ra;
        FOR x := ra/2. BY (ra/x-x)/2. WHILE abs(ra-x*x) > epsilon DO ; END FOR;
        FOR sqrt := FIX(x)-1 BY 1 WHILE (sqrt+1) * (sqrt+1) <= a DO ; END FOR;
        RETURN sqrt;
    END SELECT;
  END FUNCTION integersqrt;

  INPUT topnum;

  IF topnum > 0 THEN
    DECLARE sieve ARRAY[1:topnum] OF BOOLEAN;
    DECLARE (i, limit, count) INTEGER;
    FOR i := 1 TO topnum DO SET sieve[i] := TRUE; END FOR;
    SET limit := integersqrt(topnum)+1; /* Avoid repeating square root */
    FOR i := 2 TO limit DO
      IF sieve[i] THEN
        DECLARE j INTEGER;
        FOR j := 2*i BY i TO topnum DO SET sieve[j] := FALSE; END FOR;
      FI;
    END FOR;
    SET count := 0;
    FOR i := 1 TO topnum DO
      IF sieve[i] THEN
        SET count := count + 1;
        OUTPUT "Prime[" || count || "] = " || i;
      FI;
    END FOR;
  ELSE
    OUTPUT "Input value " || topnum || " non-positive.";
  FI;

  EXIT;
END PROGRAM Eratosthenes;
"""

PARSED_PROGRAM = """
Program Eratosthenes
  Variables:
    topnum: INTEGER
  Functions:
    function abs(x:REAL)
      Statements:
        If (x < 0) Then
          Statements:
            Return (-x)
        Else
          Statements:
            Return x
        Fi
        Return 0
    function integersqrt(a:INTEGER)
      Statements:
        Select TRUE
          Case ('a < 0'):
            Statements:
              Output('a < 0 in FUNCTION integersqrt.')
              Exit
          Case ('a == 0'):
            Statements:
              Return 0
          Case ('a > 0'):
            Variables:
              x, ra: REAL
              epsilon: REAL
              sqrt: INTEGER
            Statements:
              Assign ra := FLOAT(a)
              Assign epsilon := (1e-07 * ra)
              For x := (ra / 2.0) By (((ra / x) - x) / 2.0) While (abs((ra - (x * x))) > epsilon) Do
                Statements:
                  Empty
              End For
              For sqrt := (FIX(x) - 1) By 1 While (((sqrt + 1) * (sqrt + 1)) <= a) Do
                Statements:
                  Empty
              End For
              Return sqrt
        End Select
        Return 0
  Statements:
    Input('topnum')
    If (topnum > 0) Then
      Variables:
        sieve: Array(type='BOOLEAN', start=IntegerLiteral(value=1), end=Variable(name='topnum', index=None))
        i, limit, count: INTEGER
      Statements:
        For i := 1 To topnum Do
          Statements:
            Assign sieve[i] := TRUE
        End For
        Assign limit := (integersqrt(topnum) + 1)
        For i := 2 To limit Do
          Statements:
            If sieve[i] Then
              Variables:
                j: INTEGER
              Statements:
                For j := (2 * i) By i To topnum Do
                  Statements:
                    Assign sieve[j] := FALSE
                End For
            Fi
        End For
        Assign count := 0
        For i := 1 To topnum Do
          Statements:
            If sieve[i] Then
              Statements:
                Assign count := (count + 1)
                Output(concat(4, 'Prime[', str(count), '] = ', str(i)))
            Fi
        End For
    Else
      Statements:
        Output(concat(3, 'Input value ', str(topnum), ' non-positive.'))
    Fi
    Exit
"""


def test_program() -> None:
    result = Parser(Lexer(PROGRAM).tokens(), PROGRAM).program()
    assert dump(result) == PARSED_PROGRAM.strip()


def compile(source: str) -> Program:
    return parse(source)


def flag(argv: list[str], name: str) -> Optional[int]:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> Optional[str]:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) >= 2:
        input_file = pathlib.Path(sys.argv[1])

        source = input_file.read_text()
        compiled = generate(compile(source))

        output = arg(sys.argv, "-o") or input_file.with_suffix(".c")
        with open(output, "w") as f:
            if not flag(sys.argv, "-r"):
                preamble = pathlib.Path("preamble.c").read_text()
                f.write(preamble.strip() + "\n")
            if functions:
                f.write(functions.strip() + "\n")
            f.write(compiled.strip() + "\n")
    else:
        pytest.main(["-vvv", __file__])
