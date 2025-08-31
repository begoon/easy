import pathlib
import sys
from dataclasses import dataclass
from typing import Literal, Optional, Tuple, Union, cast

import pytest

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
    "FIX",
    "FLOAT",
    #
    "TRUE",
    "FALSE",
}

SYMBOLS = {*"+ - * / | & ( ) [ ] ; , . : := = <> < <= > >= ||".split(" ")}

functions = ""


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
                # doubles quotes for escape: "it""s"
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

    def tokens(self) -> list[Token]:
        out: list[Token] = []
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
class Node:
    def c(self) -> str:
        print(self)
        raise NotImplementedError()

    def str(self):
        print(self)
        raise NotImplementedError()


@dataclass
class Program(Node):
    name: str
    segment: "Segment"

    def str(self) -> str:
        return f"Program {self.name}" + "\n" + indent(self.segment.str(), 1)

    def c(self) -> str:
        return "int main()" + "\n" + "{\n" + indent(self.segment.c(), 1) + "\n}"


Subroutine = Union["Function", "Procedure"]


@dataclass
class Segment:
    types: list["TypeIs"]
    variables: list["Declare"]
    subroutines: list[Subroutine]
    statements: "Statements"

    def str(self) -> str:
        parts = []
        if self.types:
            parts.append("Types:")
            for t in self.types:
                parts.append(indent(f"{t.name}: {t.definition.str()}", 1))
        if self.variables:
            parts.append("Variables:")
            for v in self.variables:
                type = v.type.str() if isinstance(v.type, Node) else v.type
                names = ", ".join(v.names)
                parts.append(indent(f"{names}: {type}", 1))
        if self.subroutines:
            parts.append("Subroutines:")
            for p in self.subroutines:
                parts.append(indent(p.str(), 1))
        parts.append("Statements:")
        parts.append(self.statements.str())
        return "\n".join(parts)

    def c(self) -> str:
        parts = []
        if self.variables:
            for v in self.variables:
                if isinstance(v.type, Array):
                    for name in v.names:
                        parts.append(f"{(v.type.c(name))};")
                elif isinstance(v.type, Structure):
                    for name in v.names:
                        parts.append(f"{(v.type.c())} {name} = {{0}};")
                else:
                    names = ", ".join(v.names)
                    parts.append(f"{(TYPE(v.type))} {names} = {{0}};")
        if self.subroutines:
            side_parts = []
            for subroutine in self.subroutines:
                side_parts.append(subroutine.c())
            global functions
            functions += "\n".join(side_parts)
        parts.append(self.statements.c())
        return "\n".join(parts)


@dataclass
class Statement(Node):
    pass


@dataclass
class Expression(Node):
    pass


@dataclass
class Label(Node):
    name: str

    def c(self) -> str:
        return f"{self.name}: "

    def str(self) -> str:
        return f"LABEL {self.name}"


@dataclass
class Declare:
    names: list[str]
    type: Union[str, "Array"]


@dataclass
class TypeIs:
    name: str
    definition: Union[str, "Array", "Structure"]

    def c(self) -> str:
        definition = self.definition
        if isinstance(definition, Array):
            type = definition.c(self.name)
        elif isinstance(definition, Structure):
            type = definition.c() + " " + self.name
        else:
            type = TYPE(definition)
        return "typedef " + type + " " + self.name + ";"


@dataclass
class Array(Node):
    type: "Type"
    start: Optional[Expression]
    end: Expression

    def c(self, name: str) -> str:
        assert name, "Array.c() requires a name parameters"
        start = self.start.c() if self.start else "0"
        end = self.end.c()
        return f"{TYPE(self.type)} {name}[{start} + {end}]"

    def str(self) -> str:
        start = self.start.str() + ":" if self.start else ""
        end = self.end.str()
        return f"ARRAY[{start}{end}] OF {self.type}"


@dataclass
class Field:
    name: str
    type: Union[str, Array]


@dataclass
class Structure:
    fields: list[Field]

    def str(self) -> str:
        return " ".join(
            ["STRUCTURE", " ".join(f"FIELD {field.name} IS {field.type}" for field in self.fields), "END STRUCTURE"]
        )

    def c(self) -> str:
        return " ".join(["struct {", " ".join(f"{TYPE(field.type)} {field.name};" for field in self.fields), "}"])


Type = Literal["INTEGER", "REAL", "BOOLEAN", "STRING"] | Array


@dataclass
class Procedure(Node):
    name: str
    parameters: list[Tuple[str, Union[str, Array]]]
    segment: Segment

    def str(self) -> str:
        parameters = ", ".join(name for name, _ in self.parameters)
        return f"PROCEDURE {self.name}({parameters})" + "\n" + indent(self.segment.str(), 1)

    def c(self) -> str:
        parameters = ", ".join(f"{TYPE(type)} {name}" for name, type in self.parameters)
        return f"void {self.name}({parameters})" + "{\n" + indent(self.segment.c(), 1) + "\n}"


@dataclass
class Function(Node):
    name: str
    type: Union[str, Array]
    parameters: list[Tuple[str, Union[str, Array]]]
    segment: Segment

    def str(self) -> str:
        return (
            f"FUNCTION {self.name}({', '.join(f'{type} {name}' for name, type in self.parameters)}) : {self.type}"
            + "\n"
            + indent(self.segment.str(), 1)
        )

    def c(self) -> str:
        return (
            f"{TYPE(self.type)} {self.name}({', '.join(f'{TYPE(type)} {name}' for name, type in self.parameters)})\n"
            + "{\n"
            + indent(self.segment.c(), 1)
            + "\n}"
        )


@dataclass
class Statements(Node):
    statements: list[Statement]

    def str(self) -> str:
        return "\n".join(indent(s.str(), 1) for s in self.statements) if self.statements else "(empty)"

    def c(self) -> str:
        return "\n".join(indent(s.c(), 0) for s in self.statements) if self.statements else "(empty)"


@dataclass
class Assign(Statement):
    name: str
    expression: Expression
    index: Optional[Expression] = None

    def str(self) -> str:
        return f"ASSIGN {self.name}{'[' + self.index.str() + ']' if self.index else ''} := {self.expression.str()}"

    def c(self) -> str:
        global variables
        type = variables.get(self.name)
        if type == "STRING":
            return f"strcpy({self.name}.data, {self.expression.c()});"
        return f"{self.name}{'[' + self.index.c() + ']' if self.index else ''} = {self.expression.c()};"


@dataclass
class If(Statement):
    cond: Expression
    then_branch: Segment
    else_branch: Optional[Segment]

    def str(self) -> str:
        s = f"IF {self.cond.str()} THEN\n"
        s += indent(f"{self.then_branch.str()}\n", 1)
        if self.else_branch:
            s += "\nELSE\n"
            s += indent(f"{self.else_branch.str()}\n", 1)
        s += "\nFI"
        return s

    def c(self) -> str:
        cond = self.cond.c()
        if cond.startswith("(") and cond.endswith(")"):
            cond = cond[1:-1]
        s = f"if ({cond})\n{{\n"
        s += indent(f"{self.then_branch.c()}\n", 1)
        if self.else_branch:
            s += "\n}\nelse\n{\n"
            s += indent(f"{self.else_branch.c()}\n", 1)
        s += "\n}"
        return s


@dataclass
class For(Statement):
    variable: "Variable"
    init: Expression
    do: Segment
    by: Optional[Expression] = None
    to: Optional[Expression] = None
    while_: Optional[Expression] = None

    def str(self) -> str:
        return (
            f"FOR {self.variable.str()} "
            f":= {self.init.str()} "
            f"{f'BY {self.by.str()} ' if self.by else ''}"
            f"{f'TO {self.to.str()} ' if self.to else ''}"
            f"{f'WHILE {self.while_.str()} ' if self.while_ else ''}"
            f"DO\n"
            f"{indent(self.do.str(), 1)}"
            "\n"
            "END FOR"
        )

    def c(self) -> str:
        return (
            f"for ({self.variable.c()} = {self.init.c()}; {self.condition()}; {self.step()})\n"
            "{\n" + indent(self.do.c(), 1) + "\n}"
        )

    def condition(self) -> str:
        conditions = []
        if self.while_:
            conditions.append(self.while_.c())
        if self.to:
            conditions.append(f"{self.variable.c()} <= {self.to.c()}")
        return "".join(conditions)

    def step(self) -> str:
        return f"{self.variable.c()} += " + ("1" if not self.by else self.by.c())


@dataclass
class Select(Statement):
    expr: "Expression"
    cases: list[Tuple["Expression", Segment]]

    def str(self) -> str:
        s = f"SELECT {self.expr.str()}\n"
        for cond, body in self.cases:
            if cond is not None:
                cond = cond.str()
                s += f"  CASE {cond}:\n"
            else:
                s += "  OTHERWISE:\n"
            s += f"{indent(body.str(), 2)}\n"
        s += "END SELECT"
        return s

    def c(self) -> str:
        s = ""
        for i, [cond, body] in enumerate(self.cases, 0):
            if cond is not None:
                cond = cond.c()
                if i > 0:
                    s += " else"
                s += f"\nif {cond}\n"
            else:
                s += "\nelse\n"
            s += f"{{\n{indent(body.c(), 1)}\n}}"
        return s.strip()


@dataclass
class Input(Statement):
    variables: list[str]

    def str(self) -> str:
        return f"INPUT({', '.join(str(e) for e in self.variables)})"

    def c(self) -> str:
        inputs = []
        for variable in self.variables:
            if variable not in variables:
                raise Exception(f"undeclared variable '{variable}'")
            type = variables[variable]
            if type == "STRING":
                inputs.append(f'scanf("%s", {variable}.data);')
            else:
                assert type == "INTEGER", f"unexpected variable type in INPUT '{variable}': {type}"
                inputs.append(f'scanf("%d", &{variable});')
        return "\n".join(inputs)


@dataclass
class Output(Statement):
    expressions: list[Expression]

    def str(self) -> str:
        return f"OUTPUT({', '.join(e.str() for e in self.expressions)})"

    def c(self) -> str:
        args = ", ".join(e.c() for e in self.expressions)
        return f"output({len(self.expressions)}, {args});"


@dataclass
class Repeat(Statement):
    label: str

    def str(self) -> str:
        return f"REPEAT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class Repent(Statement):
    label: str

    def str(self) -> str:
        return f"REPENT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class Begin(Statement):
    body: Segment
    label: Optional[str] = None

    def str(self) -> str:
        s = f"BEGIN\n{indent(self.body.str(), 1)}\nEND"
        if self.label:
            s += " " + self.label
        s += ";"
        return s

    def c(self) -> str:
        s = f"{{\n{indent(self.body.c(), 1)}\n}}"
        if self.label:
            s += "\n" + self.label + ":\n"
        return s


@dataclass
class Call(Statement):
    name: str
    args: list["Expression"]

    def str(self) -> str:
        args = ", ".join(arg.str() for arg in self.args)
        return f"CALL {self.name}({args})"

    def c(self) -> str:
        args = ", ".join(arg.c() for arg in self.args)
        return f"{self.name}({args});"


@dataclass
class Return(Statement):
    value: Optional[Expression] = None

    def str(self) -> str:
        value = self.value.str() if isinstance(self.value, Expression) else self.value
        return "RETURN" + f" {value}" if self.value is not None else ""

    def c(self) -> str:
        value = self.value.c() if isinstance(self.value, Expression) else self.value
        return "return" + (f" {value}" if self.value is not None else "") + ";"


@dataclass
class Exit(Statement):
    def str(self) -> str:
        return "EXIT"

    def c(self) -> str:
        return "exit(0);"


@dataclass
class Empty(Statement):
    def str(self) -> str:
        return "Empty"

    def c(self) -> str:
        return ";"


@dataclass
class FunctionCall(Expression):
    name: str
    args: list[Expression]

    def str(self) -> str:
        return f"{self.name}({', '.join(a.str() for a in self.args)})"

    def c(self) -> str:
        return f"{self.name}({', '.join(a.c() for a in self.args)})"


@dataclass
class ProcedureCall(Expression):
    name: str
    args: list[Expression]

    def str(self) -> str:
        args = ", ".join(arg.str() for arg in self.args)
        return f"CALL {self.name}({args})"

    def c(self) -> str:
        args = ", ".join(arg.c() for arg in self.args)
        return f"{self.name}({args});"


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression

    def str(self) -> str:
        return f"({self.left.str()} {self.operation} {self.right.str()})"

    def c(self) -> str:
        return f"({self.left.c()} {self.operation} {self.right.c()})"


@dataclass
class ConcatenationOperation(Expression):
    parts: list[Expression]

    def str(self) -> str:
        parts = [ConcatenationOperation.format_part(v) for v in self.parts]
        return f"concat({len(parts)}, {', '.join(parts)})"

    def c(self) -> str:
        parts = [ConcatenationOperation.format_part(v) for v in self.parts]
        return f"concat({len(parts)}, {', '.join(parts)})"

    @staticmethod
    def format_part(v: Expression) -> str:
        str = v.c()
        is_string_literal = isinstance(v, StringLiteral)
        is_concatenation = isinstance(v, ConcatenationOperation)
        is_character_function = isinstance(v, FunctionCall) and v.name == "CHARACTER"
        skip_str = any([is_string_literal, is_concatenation, is_character_function])
        if isinstance(v, Variable):
            global variables
            type = variables.get(v.name)
            if type == "STRING":
                return f"{v.name}.data"
            return f"str({str})"
        return str if skip_str else f"str({str})"


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression

    def str(self) -> str:
        return f"({self.operation}{self.expr.str()})"

    def c(self) -> str:
        return f"({self.operation}{self.expr.c()})"


@dataclass
class Variable(Expression):
    name: str
    index: Optional[Expression] = None

    def str(self) -> str:
        return self.name + (f"[{self.index.str()}]" if self.index else "")

    def c(self) -> str:
        return self.name + (self.index and f"[{self.index.c()}]" or "")


@dataclass
class IntegerLiteral(Expression):
    value: int

    def str(self) -> str:
        return str(self.value)

    def c(self) -> str:
        return str(self.value)


@dataclass
class RealLiteral(Expression):
    value: float

    def str(self) -> str:
        return str(self.value)

    def c(self) -> str:
        return str(self.value)


@dataclass
class StringLiteral(Expression):
    value: str

    def str(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return f'"{self.value}"'


@dataclass
class BoolLiteral(Expression):
    value: bool

    def str(self) -> str:
        return "true" if self.value else "false"

    def c(self) -> str:
        return "1" if self.value else "0"


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
    def __init__(self, tokens: list[Token], source: str):
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
        types = self.types_section()
        variables = self.variables_section()
        subroutines = self.subroutines()
        statements = self.statements()
        return Segment(types, variables, subroutines, statements)

    def types_section(self) -> list[TypeIs]:
        out: list[TypeIs] = []
        while self.accept("TYPE"):
            name = self.eat("IDENT").value
            self.eat("IS")
            definition = self.type()
            self.eat(";")
            out.append(TypeIs(name, definition))
            types[name] = definition
        return out

    def variables_section(self) -> list[Declare]:
        global variables

        out: list[Declare] = []
        while self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.type()
                self.eat(";")
                out.append(Declare([name], type))
                variables[name] = type
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
                out.append(Declare(names, type))
                for name in names:
                    variables[name] = type
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
        if token.value == "STRUCTURE":
            self.i += 1

            fields = []
            self.eat("FIELD")
            name = self.eat("IDENT").value
            self.eat("IS")
            type = self.type()
            fields.append(Field(name, type))

            while self.accept(","):
                name = self.eat("IDENT").value
                self.eat("IS")
                type = self.type()
                fields.append(Field(name, type))

            self.eat("END")
            self.eat("STRUCTURE")
            return Structure(fields)
        return self.eat("IDENT").value

    def subroutines(self) -> list[Procedure | Function]:
        out: list[Procedure | Function] = []
        while token := self.accept(("FUNCTION", "PROCEDURE")):
            name = self.eat("IDENT").value
            parameters: list[Tuple[str, Union[str, "Array"]]] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                self.eat(")")

            type = None
            if token.value == "FUNCTION":
                type = self.type()

            self.eat(":")
            segment = self.segment()

            if token.value == "FUNCTION":
                segment.statements.statements.append(Return(0))

            self.eat("END")
            self.eat(token.value)
            self.eat(name)
            self.eat(";")

            if token.value == "PROCEDURE":
                assert type is None, f"{type=}"
                out.append(Procedure(name, parameters, segment))
            else:
                out.append(Function(name, type, parameters, segment))
        return out

    def parameters(self) -> list[Tuple[str, Union[str, Array]]]:
        out: list[Tuple[str, Union[str, Array]]] = []
        while True:
            name = self.eat("IDENT").value
            type = self.type()
            out.append((name, type))
            if not self.accept(","):
                break
        return out

    def statements(self) -> Statements:
        statements: list[Statement] = []
        STATEMENTS = (
            "SET",
            "CALL",
            "IF",
            "RETURN",
            "EXIT",
            "INPUT",
            "OUTPUT",
            "FOR",
            "SELECT",
            "REPEAT",
            "REPENT",
            "BEGIN",
            ";",
        )

        def is_label():
            current = self.current()
            return (current.type == "IDENT" and current.value != "OTHERWISE") and self.peek().value == ":"

        while self.current().value in STATEMENTS or is_label():
            if is_label():
                label = self.eat("IDENT").value
                self.eat(":")
                statements.append(Label(label))
            else:
                statements.append(self.statement())
        return Statements(statements)

    def statement(self) -> Statement:
        token = self.current()
        if token.value == "SET":
            return self.assignment_statement()
        if token.value == "CALL":
            return self.call_statement()
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
        if token.value == "REPEAT":
            return self.repeat_statement()
        if token.value == "REPENT":
            return self.repent_statement()
        if token.value == "BEGIN":
            return self.begin_statement()
        self.eat(";")
        return Empty()

    def arguments(self) -> list[Expression]:
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
        if self.accept("OTHERWISE"):
            self.eat(":")
            body = self.segment()
            cases.append((None, body))
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

    def repeat_statement(self) -> Statements:
        self.eat("REPEAT")
        label = self.eat("IDENT").value
        self.eat(";")
        return Repeat(label)

    def repent_statement(self) -> Statements:
        self.eat("REPENT")
        label = self.eat("IDENT").value
        self.eat(";")
        return Repent(label)

    def begin_statement(self) -> Statements:
        self.eat("BEGIN")
        body = self.segment()
        self.eat("END")
        label = self.accept("IDENT")
        self.eat(";")
        return Begin(body, label and label.value)

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

    def call_statement(self) -> Call:
        self.eat("CALL")
        name = self.eat("IDENT").value
        args = []
        if self.accept("("):
            args = self.arguments()
            self.eat(")")
        self.eat(";")
        return Call(name, args)

    def expression(self) -> Expression:
        return self.expression_or_xor()

    def expression_or_xor(self) -> Expression:
        left = self.expression_and()
        while operation := self.accept(("|", "XOR")):
            right = self.expression_and()
            operation.value = {"|": "||", "XOR": "^"}.get(operation.value, operation.value)
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_and(self) -> Expression:
        left = self.expression_not()
        while operation := self.accept(("&",)):
            right = self.expression_not()
            operation.value = "&&"
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
            operation.value = {"=": "==", "<>": "!="}.get(operation.value, operation.value)
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
            operation.value = {"MOD": "%"}.get(operation.value, operation.value)
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


types: dict[str, str | Array | Structure] = {}
variables: dict[str, str | Array | Structure] = {}


def TYPE(v: str) -> str:
    if isinstance(v, Array):
        return v
    if isinstance(v, Structure):
        return v.str()
    if v in types:
        return v
    type = {"INTEGER": "int", "REAL": "double", "BOOLEAN": "int", "STRING": "STR"}.get(v)
    if not type:
        raise ValueError(f"unknown type '{v}'")
    return type


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
            "((((-b) + (1 - 2)) - (LENGTH('3') * 7)) ^ 1)",
        ),
        ("-b + (1 - 2) - func(" "3" ")", "(((-b) + (1 - 2)) - func(" "3" "))"),
        ("NOT a < b", "(NOT(a < b))"),
        ("a || b", "concat(2, str(a), str(b))"),
        ('"a" || b', 'concat(2, "a", str(b))'),
        ('"a" || b || c', 'concat(3, "a", str(b), str(c))'),
        (
            'a || (b || SUBSTR("abc", 0, 2))',
            'concat(2, str(a), concat(2, str(b), str(SUBSTR("abc", 0, 2))))',
        ),
        (
            'a || b || SUBSTR("abc", 0, 2)',
            'concat(3, str(a), str(b), str(SUBSTR("abc", 0, 2)))',
        ),
    ],
)
def test_expression(input, expected) -> None:
    result = Parser(Lexer(input).tokens(), input).expression()
    if not isinstance(expected, str):
        expected = expected.str()
    assert result.str() == expected, f"\n{expected}\n!=\n{result}\n"


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

        lexer = Lexer(source)

        tokens = lexer.tokens()
        if "-t" in sys.argv:
            tokens_file = input_file.with_suffix(".tokens")

            def format_token(token: Token) -> str:
                return f"{input_file}:{token.line}:{token.col}\t {token.value}"

            tokens_file.write_text("\n".join(format_token(t) for t in tokens) + "\n")

        ast = Parser(tokens, source).program()
        if "-a" in sys.argv:
            ast_file = input_file.with_suffix(".ast")
            ast_file.write_text(ast.str() + "\n")

        compiled = ast.c().strip()

        if "-s" in sys.argv:
            c = input_file.with_suffix(".s")
            with open(c, "w") as f:
                if types:
                    for name, definition in types.items():
                        if isinstance(definition, Array):
                            f.write(f"typedef {definition.c(name)};\n")
                        else:
                            f.write(f"typedef {definition.c()} {name};\n")
                if functions:
                    f.write(functions.strip() + "\n")
                f.write(compiled.strip() + "\n")

        output = arg(sys.argv, "-o") or input_file.with_suffix(".c")
        with open(output, "w") as f:
            f.write('#include "preamble.c"\n')
            if types:
                for name, definition in types.items():
                    if isinstance(definition, Array):
                        f.write(f"typedef {definition.c(name)};\n")
                    else:
                        f.write(f"typedef {definition.c()} {name};\n")
            if functions:
                f.write(functions.strip() + "\n")
            f.write(compiled + "\n")
    else:
        pytest.main(["-vvv", __file__])
