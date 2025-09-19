import io
import re
import sys
from dataclasses import dataclass, fields, is_dataclass
from pathlib import Path
from typing import Any, Mapping, Optional, Tuple, Union, cast

from ruamel.yaml import YAML

from peg.parser import PEGParser

#


def yamlizer(root: Any) -> str:

    def walker(obj: Node, *, seen: set[int] | None = None) -> Node:
        if seen is None:
            seen = set()

        if obj is None or isinstance(obj, (bool, int, float, str)):
            return obj

        oid = id(obj)
        if oid in seen:
            raise Exception("cycle detected in AST: serialization would recurse forever")
        seen.add(oid)

        if is_dataclass(obj):
            if isinstance(obj, Token):
                token = cast(Token, obj)
                data = f"<{token.value}|{token.type} {token.context.text.filename}:{obj.line}:{obj.character}>"
            else:
                data = {}
                data["node"] = obj.__class__.__name__
                for f in fields(obj):
                    if f.name.startswith("_"):
                        continue
                    value = getattr(obj, f.name)
                    data[f.name] = walker(value, seen=seen)
            seen.remove(oid)
            return data

        if isinstance(obj, Mapping):
            out = {}
            for k, v in obj.items():
                key = k if isinstance(k, str) else repr(k)
                out[key] = walker(v, seen=seen)
            seen.remove(oid)
            return out

        if isinstance(obj, (list, tuple, set)):
            seq = [walker(x, seen=seen) for x in obj]
            seen.remove(oid)
            return seq

        try:
            return str(obj)
        finally:
            seen.remove(oid)

    data = walker(cast(None, root))

    yaml = YAML()
    yaml.default_flow_style = False
    yaml.indent(mapping=2, sequence=4, offset=2)
    stream = io.StringIO()
    yaml.dump(data, stream)
    return stream.getvalue()


#


def table(rows: list[list[str]]) -> str:
    if not rows:
        return ""
    n_cols = max(len(row) for row in rows)
    col_widths = [0] * n_cols
    for row in rows:
        for i, cell in enumerate(row):
            col_widths[i] = max(col_widths[i], len(cell))

    lines = []
    for row in rows:
        line = "  ".join(cell.ljust(col_widths[i]) for i, cell in enumerate(row))
        lines.append(line)

    return "\n".join(lines) + "\n"


#


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


#

KEYWORDS = {
    "PROGRAM",
    #
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
    character: int

    context: "Context"

    def __str__(self) -> str:
        value = self.value if len(self.value) < 20 else self.value[:17] + "..."
        value = f"'{value}'" if self.type in ["STRING", "SYMBOL"] else value
        v = "<" + value
        if self.type != self.value:
            v += f"|{self.type}"
        input = self.context.text
        v += f"|{input.filename}:{self.line}:{self.character}"
        return v

    def __repr__(self) -> str:
        return str(self)


@dataclass
class Context:
    flags: dict[str, str]

    text: InputText
    tokens: list[Token]

    common: list[str]
    types: dict[str, "Type"]
    functions: dict[str, "BuiltinFunction | FUNCTION"]
    procedures: dict[str, "PROCEDURE"]
    variables: dict[str, "Variable"]


class CompilerError(Exception):
    pass


class LexerError(CompilerError):
    pass


class Lexer:
    def __init__(self, context: Context):
        self.context = context

        input = context.text

        self.input = input
        self.text = input.text

        self.position = 0
        self.line = 1
        self.character = 1
        self.n = len(self.text)

    def peek(self, k=1) -> str:
        j = self.position + k
        return self.text[j] if j < self.n else ""

    def current(self) -> str:
        return self.text[self.position] if self.position < self.n else ""

    def advance(self, k=1):
        for _ in range(k):
            if self.position < self.n:
                ch = self.text[self.position]
                self.position += 1
                if ch == "\n":
                    self.line += 1
                    self.character = 1
                else:
                    self.character += 1

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
                    if self.position >= self.n:
                        location = f"{self.input.filename}:{self.line}:{self.character}"
                        raise LexerError(f"unterminated /* */ comment at {location}")
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
        line, character = self.line, self.character
        v = ""
        while self.current().isdigit():
            v += self.current()
            self.advance()

        if self.current() == "." or self.current() == "e":
            v += self.current()
            self.advance()
            while self.current().isdigit() or self.current() in "+-eE":
                v += self.current()
                self.advance()
            return Token("REAL", v, line, character, self.context)
        return Token("INTEGER", v, line, character, self.context)

    def ident_or_keyword(self) -> Token:
        line, character = self.line, self.character
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
            return Token("KEYWORD", value, line, character, self.context)
        return Token("IDENT", v, line, character, self.context)

    def string(self) -> Token:
        line, character = self.line, self.character
        quote = self.current()
        self.advance()
        v = ""
        while True:
            c = self.current()
            if not c:
                location = f"{self.input.filename}:{line}:{character}"
                raise LexerError(f"unterminated string at {location}")
            if c == quote:
                self.advance()
                # doubles quotes for escape: "it""s"
                if self.current() == quote:
                    v += quote
                    self.advance()
                    continue
                break
            v += c
            self.advance()
        return Token("STRING", v, line, character, self.context)

    def symbol(self) -> Token:
        start, character = self.line, self.character
        # try 2-char symbols first
        two = self.current() + self.peek()
        if two in SYMBOLS:
            self.advance(2)
            return Token("SYMBOL", two, start, character, self.context)
        one = self.current()
        if one in SYMBOLS:
            self.advance()
            return Token("SYMBOL", one, start, character, self.context)
        location = f"{self.input.filename}:{start}:{character}"
        raise LexerError(f"unknown symbol '{one}' at {location}")

    def tokenize(self) -> list[Token]:
        v: list[Token] = []
        while True:
            self.skip_whitespace_and_comments()
            if self.position >= self.n:
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
        v.append(Token("EOF", "", self.line, self.character, self.context))
        return v


@dataclass
class Node:
    token: Token
    scope: str

    def context(self) -> Context:
        return self.token.context

    def c(self) -> str:
        print(self)
        raise GenerateError(f"c() not implemented for {self.__class__.__name__} at {self.token}")

    def __str__(self) -> str:
        return yamlizer(self)


@dataclass
class Statement(Node):
    pass


@dataclass
class Type:
    def c(self) -> str:
        print(self)
        raise GenerateError(f"c() not implemented for {self.__class__.__name__}")

    def zero(self) -> str:
        print(self)
        raise GenerateError(f"zero() not implemented for {self.__class__.__name__}")

    def typedef(self, alias: str = "") -> str:
        print(self)
        raise GenerateError(f"typedef() not implemented for {self.__class__.__name__}")

    def format(self) -> str:
        return ""


@dataclass
class BuiltinType(Type):
    pass


@dataclass
class IntegerType(BuiltinType):
    def c(self) -> str:
        return "int"

    def zero(self) -> str:
        return "0"

    def typedef(self, alias: str) -> str:
        return f"typedef int {alias}"

    def format(self) -> str:
        return "i"


@dataclass
class RealType(BuiltinType):
    def c(self) -> str:
        return "double"

    def zero(self) -> str:
        return "0.0"

    def typedef(self, alias: str) -> str:
        return f"typedef double {alias}"

    def format(self) -> str:
        return "r"


@dataclass
class BooleanType(BuiltinType):
    def c(self) -> str:
        return "int"

    def zero(self) -> str:
        return "0"

    def typedef(self, alias: str) -> str:
        return f"typedef int {alias}"

    def format(self) -> str:
        return "b"


@dataclass
class StringType(BuiltinType):
    initial: Optional[str] = None

    def c(self) -> str:
        return "STR"

    def zero(self) -> str:
        if self.initial:
            return f'{{ .data = "{self.initial}" }}'
        return "{0}"

    def typedef(self, alias: str) -> str:
        return f"typedef STR {alias}"

    def format(self) -> str:
        return "A"


@dataclass
class Expression(Node):
    type: Type


@dataclass
class ArrayType(Type):
    type: Type

    hi: Expression
    lo: Expression

    dynamic: bool = False

    def c(self) -> str:
        if self.dynamic:
            v = ["struct\n{", indent(f"{self.type.c()} *data;", 1), "}"]
        else:
            v = ["struct\n{", indent(f"{self.type.c()} data[{self.sz()}];", 1), "}"]
        return "\n".join(v)

    def sz(self) -> str:
        return self.hi.c() + " - " + self.lo.c() + " + 1"

    def zero(self) -> str:
        if self.dynamic:
            return "{ .data = malloc(sizeof(" + self.type.c() + ") * (" + self.sz() + ")) }"
        return "{0}"

    def typedef(self, to: str) -> str:
        return f"typedef {self.c()} {to}"


@dataclass
class StructField(Node):
    name: str
    type: Type

    def c(self) -> str:
        return f"{self.type.c()} {self.name}"


@dataclass
class StructType(Type):
    fields: list[StructField]

    def c(self) -> str:
        v = ["struct\n{"]
        for field in self.fields:
            v.append(indent(field.c() + ";", 1))
        v.append("}")
        return emit(v)

    def init(self) -> str:
        return "{0}"

    def zero(self) -> str:
        return "{0}"

    def typedef(self, name: str) -> str:
        return "typedef " + self.c() + " " + name


@dataclass
class AliasType(Type):
    reference_name: str
    reference_type: Type

    def c(self) -> str:
        return f"{self.reference_name}"

    def zero(self) -> str:
        return self.reference_type.zero()

    def typedef(self, alias: str) -> str:
        return f"typedef {self.reference_name} {alias}"


@dataclass
class Label(Node):
    name: str

    def c(self) -> str:
        return f"{self.name}:"


@dataclass
class Segment(Node):
    types: list["TYPEIS"]
    variables: list["DECLARE"]
    subroutines: list[Union["FUNCTION", "PROCEDURE"]]
    statements: list[Statement]

    def c(self, main: bool = False) -> str:
        v = []
        if self.variables:
            for variable in self.variables:
                c = variable.c()
                if main:
                    self.context().common.append(c)
                else:
                    v.append(c)
        if self.statements:
            for statement in self.statements:
                v.append(statement.c())
        return emit(v)


@dataclass
class DECLARE(Node):
    names: list[str]
    type: Type

    def c(self) -> str:
        v = []
        for name in self.names:
            v.append(f"{self.type.c()} {name} = {self.type.zero()};")
        return emit(v)


@dataclass
class TYPEIS(Node):
    name: str
    definition: Type


@dataclass
class Entity:
    token: Token


@dataclass
class FIELD(Entity):
    name: str
    type: Type

    def c(self) -> str:
        return f"{self.type.name} {self.name}"


@dataclass
class STRUCTURE(Node):
    fields: list[FIELD]

    def c(self) -> str:
        v = ["struct {", " ".join(f"{field.c()};" for field in self.fields), "}"]
        return " ".join(v)


@dataclass
class Argument(Entity):
    name: str
    type: Type

    def c(self) -> str:
        return f"{self.type.name} {self.name}"


@dataclass
class PROCEDURE(Node):
    name: str
    arguments: list[Argument]
    segment: Segment

    def c(self) -> str:
        arguments = ", ".join(argument.c() for argument in self.arguments)
        v = [
            f"void {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return emit(v)


@dataclass
class FUNCTION(Node):
    name: str
    type: Type
    arguments: list[Argument]
    segment: Segment

    def c(self) -> str:
        arguments = ", ".join(argument.c() for argument in self.arguments)
        function = self.context().functions.get(self.name)
        v = [
            f"{function.type.c()} {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return emit(v)


@dataclass
class SET(Statement):
    target: list["VariableReference"]
    expression: Expression

    def c(self) -> str:
        v = []

        for target in self.target:
            variable = discover_variable(target)
            _, reference = expand_variable_reference(variable, target)
            v.append(f"{reference} = {self.expression.c()};")
        return emit(v)


@dataclass
class IF(Statement):
    cond: Expression
    then_branch: Segment
    else_branch: Optional[Segment]

    def c(self) -> str:
        cond = self.cond.c()
        if cond.startswith("(") and cond.endswith(")"):
            cond = cond[1:-1]
        v = [f"if ({cond})", "{", indent(self.then_branch.c(), 1), "}"]
        if self.else_branch:
            v.append("else")
            v.append("{")
            v.append(indent(self.else_branch.c(), 1))
            v.append("}")
        return emit(v)


@dataclass
class FOR(Statement):
    variable: "VariableReference"
    init: Expression
    do: Segment
    by: Optional[Expression] = None
    to: Optional[Expression] = None
    condition: Optional[Expression] = None

    def c(self) -> str:
        v = [
            f"for ({self.variable.c()} = {self.init.c()}; {self.format_condition()}; {self.step()})",
            "{",
            indent(self.do.c(), 1),
            "}",
        ]
        return emit(v)

    def format_condition(self) -> str:
        conditions = []
        if self.condition:
            conditions.append(self.condition.c())
        if self.to:
            conditions.append(f"{self.variable.c()} <= {self.to.c()}")
        return "".join(conditions)

    def step(self) -> str:
        return f"{self.variable.c()} += " + ("1" if not self.by else self.by.c())


@dataclass
class SELECT(Statement):
    expr: Expression
    cases: list[Tuple[Expression, Segment]]

    def c(self) -> str:
        v = []
        for i, [condition, body] in enumerate(self.cases, 0):
            if condition is not None:
                condition = condition.c()
                v.append(("else " if i > 0 else "") + "if " + condition)
            else:
                v.append("else")
            v.append("{")
            v.append(indent(body.c(), 1))
            v.append("}")
        return emit(v)


@dataclass
class INPUT(Statement):
    variables: list["VariableReference"]

    def c(self) -> str:
        inputs = []
        for variable_reference in self.variables:
            variable = discover_variable(variable_reference)
            type, reference = expand_variable_reference(variable, variable_reference)

            if isinstance(type, StringType):
                inputs.append(f'scanf("%s", {reference}.data);')
            elif isinstance(type, IntegerType):
                inputs.append(f'scanf("%d", &{reference});')
            elif isinstance(type, RealType):
                inputs.append(f'scanf("%lf", &{reference});')
            else:
                raise GenerateError(f"unsupported variable '{variable}' type in INPUT at {variable.token}")
        return emit(inputs)


@dataclass
class OUTPUT(Statement):
    arguments: list[Expression]

    def c(self) -> str:
        output = []
        format: list[str] = []
        arguments = ", ".join(expression_stringer(argument, format, "OUTPUT") for argument in self.arguments)
        output.append(f'$output("{"".join(format)}", {arguments});')
        return emit(output)


@dataclass
class REPEAT(Statement):
    label: str

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class REPENT(Statement):
    label: str

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class BEGIN(Statement):
    body: Segment
    label: Optional[str] = None

    def c(self) -> str:
        v = ["{", indent(self.body.c(), 1), "}"]
        if self.label:
            v.append(self.label + ":")
        return emit(v)


@dataclass
class CALL(Statement):
    name: str
    arguments: list[Expression]

    def c(self) -> str:
        arguments = ", ".join(arg.c() for arg in self.arguments)
        return f"{self.name}({arguments});"


@dataclass
class RETURN(Statement):
    value: Optional[Expression] = None

    def c(self) -> str:
        if self.value is None:
            return "return;"
        value = self.value.c()
        return "return" + f" {value}" + ";"


@dataclass
class EXIT(Statement):
    def c(self) -> str:
        return "exit(0);"


@dataclass
class EMPTY(Statement):
    def c(self) -> str:
        return ";"


@dataclass
class FunctionCall(Expression):
    name: str
    arguments: list[Expression]

    def c(self) -> str:
        return f"{self.name}({', '.join(a.c() for a in self.arguments)})"


OPERATIONS = {"|": "||", "&": "&&", "=": "==", "<>": "!=", "MOD": "%", "XOR": "^"}


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression

    def c(self) -> str:
        operation = OPERATIONS.get(self.operation, self.operation)
        if v := string_compare(self.left, self.right, operation):
            return v
        return f"({self.left.c()} {operation} {self.right.c()})"


def string_compare(left: Expression, right: Expression, operation: str) -> str | None:
    if operation not in ("==", "!="):
        return None

    def is_string_type(e: Expression) -> Tuple[bool, Optional[str]]:
        if not isinstance(e, VariableReference):
            return False, None

        variable = discover_variable(e)
        type, reference = expand_variable_reference(variable, e)
        return isinstance(type, StringType), reference

    is_left, left = is_string_type(left)
    is_right, right = is_string_type(right)
    if is_left or is_right:
        cmp = "!=" if operation == "!=" else "=="
        return f"strcmp({left}.data, {right}.data) {cmp} 0"
    return None


@dataclass
class ConcatenationOperation(Expression):
    parts: list[Expression]

    def c(self) -> str:
        output = []
        format: list[str] = []
        arguments = ", ".join(expression_stringer(argument, format, "||") for argument in self.parts)
        output.append(f'$concat("{"".join(format)}", {arguments})')
        return emit(output)


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression

    def c(self) -> str:
        operation = "!" if self.operation == "NOT" else self.operation
        return f"({operation}{self.expr.c()})"


@dataclass
class VariableSubscript(Entity):
    value: Expression

    def c(self) -> str:
        raise NotImplementedError(f"use VariableSubscript.index() instead of c() at {self.token}")

    def index(self) -> str:
        return self.value.c()


@dataclass
class VariableField(Entity):
    name: str

    def c(self) -> str:
        return "." + self.name


@dataclass
class VariableReference(Entity):
    scope: str
    name: str
    parts: list[VariableSubscript | VariableField]

    def c(self) -> str:
        variable = discover_variable(self)

        _, reference = expand_variable_reference(variable, self)
        return reference


@dataclass
class Variable(Entity):
    name: str
    type: Type

    zero: Optional[str] = None

    def c(self) -> str:
        return self.type.c() + " " + self.name

    def is_const(self) -> bool:
        return self.zero is not None

    def const(self) -> str:
        if not self.is_const():
            raise GenerateError(f"variable '{self.name}' is not a constant at {self.token}")

        zero = self.zero.replace('"', r"\"")
        return self.type.c() + " " + self.name + f' = {{ .data = "{zero}" }}'

    def s(self, scope: str) -> list[str]:
        return (self.name, scope, type(self.type).__name__, str(self.token))


@dataclass
class BuiltinLiteral(Expression):
    pass


@dataclass
class IntegerLiteral(BuiltinLiteral):
    value: int

    def c(self) -> str:
        return str(self.value)

    def format(self) -> str:
        return "i"


@dataclass
class RealLiteral(BuiltinLiteral):
    value: float

    def c(self) -> str:
        return str(self.value)

    def format(self) -> str:
        return "r"


@dataclass
class StringLiteral(BuiltinLiteral):
    value: str

    def c(self) -> str:
        value = self.value.replace('"', r"\"")
        return f'from_cstring("{value}")'

    def literal(self) -> str:
        value = self.value.replace('"', r"\"")
        return f'"{value}"'

    def format(self) -> str:
        return "A"


@dataclass
class BoolLiteral(BuiltinLiteral):
    value: bool

    def c(self) -> str:
        return "TRUE" if self.value else "FALSE"

    def format(self) -> str:
        return "b"


@dataclass
class PROGRAM(Node):
    name: str
    segment: Segment

    def c(self) -> str:
        return emit(["int main()", "{", indent(self.segment.c(main=True), 1), "}"])


# --------------------------


def indent(s: str, n: int) -> str:
    pad = "    " * n
    return "\n".join(pad + line for line in s.splitlines())


def emit(lines: list[str]) -> str:
    return "\n".join(lines)


def is_number(name: str) -> bool:
    return name in ("INTEGER", "REAL")


def expression_stringer(v: Expression, format: list[str], callee: str) -> str:
    c = v.c()
    if isinstance(v, BuiltinLiteral):
        convert = v.format()
        format.append(convert)
        return c

    if isinstance(v, VariableReference):
        variable = discover_variable(v)

        type, reference = expand_variable_reference(variable, v)
        if isinstance(type, AliasType):
            type = type.reference_type

        if not isinstance(type, BuiltinType):
            raise GenerateError(f"unsupported {callee} variable argument '{v}' of type '{type}' at {v.token}")

        convert = type.format()
        format.append(convert)
        return reference

    if isinstance(v, ConcatenationOperation):
        format.append("A")
        return c

    if isinstance(v, FunctionCall):
        function = v.context().functions[v.name]
        type = function.type

        if not isinstance(type, BuiltinType):
            raise GenerateError(f"unsupported {callee} function argument ${v} of type {type} at {v.token}")
        convert = type.format()
        format.append(convert)
        return c

    raise GenerateError(f"unsupported {callee} argument '{v}' at {v.token}")


class GenerateError(Exception):
    pass


@dataclass
class BuiltinFunction:
    name: str
    type: Type


# ###
class ParseError(CompilerError):
    def __init__(self, message: str, token: Token):
        super().__init__(message)
        self.message = message
        self.token = token

    def __str__(self) -> str:
        token = self.token
        error_line = token.input.text.splitlines()[token.line - 1]
        return (
            f"{self.message}\n"
            "at "
            f"{token.input.filename}:{token.line}:{token.character}\n"
            f"{error_line}\n{' ' * (token.character - 1)}^"
        )


class Parser:
    def __init__(self, context: Context):
        self.context = context

        self.tokens = context.tokens
        self.i = 0
        self.scopes = []

    def scope(self) -> str:
        return ".".join(self.scopes) if self.scopes else "@"

    def enter_scope(self, name: str) -> None:
        self.scopes.append(name)

    def leave_scope(self) -> None:
        self.scopes.pop()

    def current(self) -> Token:
        return self.tokens[self.i]

    def eat(self, kind: str | tuple[str, ...]) -> Token:
        kinds = (kind,) if isinstance(kind, str) else kind
        token = self.current()
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        expected = "/".join(kinds)
        raise ParseError(f"expected '{expected}', found '{token.value}'", token)

    def accept(self, kind: str | tuple[str, ...]) -> Token | None:
        token = self.current()
        kinds = (kind,) if isinstance(kind, str) else kind
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        return None

    def peek(self) -> Token:
        current = self.current()
        return (
            self.tokens[self.i + 1]
            if self.i + 1 < len(self.tokens)
            else Token("EOF", "", current.line, current.character)
        )

    def program(self) -> PROGRAM:
        token = self.current()

        self.eat("PROGRAM")
        name = self.eat("IDENT").value
        self.eat(":")

        self.enter_scope(f"PROGRAM:{name}")
        segments = self.segment()
        self.leave_scope()

        self.eat("END")
        self.eat("PROGRAM")
        self.eat(name)
        self.eat(";")
        self.eat("EOF")
        return PROGRAM(token, self.scope(), name, segments)

    def segment(self) -> Segment:
        token = self.current()
        types = self.types_section()
        variables = self.variables_section()
        subroutines = self.procedures_and_functions_section()
        statements = self.statements_section()
        return Segment(token, self.scope(), types, variables, subroutines, statements)

    def types_section(self) -> list[TYPEIS]:
        out: list[TYPEIS] = []
        while token := self.accept("TYPE"):
            name = self.eat("IDENT").value
            self.eat("IS")
            definition = self.parse_type()
            self.eat(";")

            out.append(TYPEIS(token, self.scope(), name, definition))
            enlist_type(name, definition, self.context)
        return out

    def variables_section(self) -> list[DECLARE]:
        declarations: list[DECLARE] = []
        while declare_token := self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.parse_type()
                self.eat(";")
                declarations.append(DECLARE(declare_token, self.scope(), [name], type))
                variable = Variable(token, name, type)
                enlist_variable(variable, self.scope())
                continue
            if token.value == "(":
                self.eat("(")
                names = []
                while self.current().value != ")":
                    self.accept(",")
                    names.append(self.eat("IDENT").value)
                self.eat(")")
                type = self.parse_type()
                self.eat(";")
                declarations.append(DECLARE(declare_token, self.scope(), names, type))
                for name in names:
                    variable = Variable(token, name, type)
                    enlist_variable(variable, self.scope())
                continue
            raise ParseError("expected a variable or '(' variable, ... ')'", token)
        return declarations

    def parse_type(self) -> Type:
        token = self.current()
        if token.value in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
            self.eat(token.value)
            return self.context.types[token.value]
        if token.value == "ARRAY":
            self.eat(token.value)
            self.eat("[")
            end = self.expression()
            start = IntegerLiteral(token, self.scope(), IntegerType(), 0)
            if self.accept(":"):
                start = end
                end = self.expression()
            self.eat("]")
            self.eat("OF")
            element_type = self.parse_type()

            dynamic = not (isinstance(end, IntegerLiteral) and isinstance(start, IntegerLiteral))
            return ArrayType(element_type, end, start, dynamic)
        if token.value == "STRUCTURE":
            self.eat(token.value)
            fields = []
            field_token = self.eat("FIELD")
            name = self.eat("IDENT").value
            self.eat("IS")
            field_type = self.parse_type()
            fields.append(StructField(field_token, self.scope(), name, field_type))

            while self.accept(","):
                field_token = self.eat("FIELD")
                name = self.eat("IDENT").value
                self.eat("IS")
                field_type = self.parse_type()
                fields.append(StructField(field_token, self.scope(), name, field_type))

            self.eat("END")
            self.eat("STRUCTURE")
            return StructType(fields)

        token = self.eat("IDENT")
        alias_name = token.value

        if (alias_type := self.context.types.get(alias_name)) is None:
            raise ParseError(f"unknown type alias '{alias_name}'", token)

        return AliasType(alias_name, alias_type)

    def procedures_and_functions_section(self) -> list[PROCEDURE | FUNCTION]:
        subroutines: list[PROCEDURE | FUNCTION] = []
        while token := self.accept(("FUNCTION", "PROCEDURE")):
            name = self.eat("IDENT").value
            self.enter_scope(token.value + ":" + name)

            parameters: list[Variable] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                    for parameter in parameters:
                        enlist_variable(parameter, self.scope())
                self.eat(")")

            type = None
            if token.value == "FUNCTION":
                type = self.parse_type()

            self.eat(":")
            segment = self.segment()

            self.eat("END")
            self.eat(token.value)
            self.eat(name)
            self.eat(";")

            if token.value == "PROCEDURE":
                subroutine = PROCEDURE(token, self.scope(), name, parameters, segment)
                self.context.procedures[name] = subroutine
            else:
                subroutine = FUNCTION(token, self.scope(), name, type, parameters, segment)
                self.context.functions[name] = subroutine

            self.leave_scope()
            subroutines.append(subroutine)

        return subroutines

    def parameters(self) -> list[Variable]:
        parameters: list[Variable] = []
        while True:
            token = self.eat("IDENT")
            name = token.value
            type = self.parse_type()
            variable = Variable(token, name, type)
            parameters.append(variable)
            enlist_variable(variable, self.scope())
            if not self.accept(","):
                break
        return parameters

    def statements_section(self) -> list[Statement]:
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
                label_token = self.eat("IDENT")
                label = label_token.value
                self.eat(":")
                statements.append(Label(label_token, self.scope(), label))
            else:
                statements.append(self.statement())
        return statements

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
        return EMPTY(token, self.scope())

    def arguments(self) -> list[Expression]:
        arguments = [self.expression()]
        while self.accept(","):
            arguments.append(self.expression())
        return arguments

    def if_statement(self) -> IF:
        token = self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.segment()
        else_branch = self.accept("ELSE") and self.segment()
        self.eat("FI")
        self.eat(";")
        return IF(token, self.scope(), cond, then_branch, else_branch)

    def for_statement(self) -> FOR:
        token = self.eat("FOR")
        variable = self.variable_reference()
        self.eat(":=")
        init = self.expression()
        by = self.accept("BY") and self.expression()
        to = self.accept("TO") and self.expression()
        condition = self.accept("WHILE") and self.expression()
        self.eat("DO")
        do = self.segment()
        self.eat("END")
        self.eat("FOR")
        self.eat(";")
        return FOR(token, self.scope(), variable, init, do, by, to, condition)

    def select_statement(self) -> SELECT:
        token = self.eat("SELECT")
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
        return SELECT(token, self.scope(), expr, cases)

    def return_statement(self) -> RETURN:
        token = self.eat("RETURN")
        if self.accept(";"):
            return RETURN(token, self.scope())
        value = self.expression()
        self.eat(";")
        return RETURN(token, self.scope(), value)

    def exit_statement(self) -> EXIT:
        token = self.eat("EXIT")
        self.eat(";")
        return EXIT(token, self.scope())

    def input_statement(self) -> INPUT:
        token = self.eat("INPUT")

        variables = [self.variable_reference()]

        while self.accept(","):
            variables.append(self.variable_reference())

        self.eat(";")
        return INPUT(token, self.scope(), variables)

    def output_statement(self) -> OUTPUT:
        token = self.eat("OUTPUT")
        expressions = [self.expression()]
        while self.accept(","):
            expressions.append(self.expression())
        self.eat(";")
        return OUTPUT(token, self.scope(), expressions)

    def repeat_statement(self) -> REPEAT:
        token = self.eat("REPEAT")
        label = self.eat("IDENT").value
        self.eat(";")
        return REPEAT(token, self.scope(), label)

    def repent_statement(self) -> REPENT:
        token = self.eat("REPENT")
        label = self.eat("IDENT").value
        self.eat(";")
        return REPENT(token, self.scope(), label)

    def begin_statement(self) -> BEGIN:
        token = self.eat("BEGIN")
        body = self.segment()
        self.eat("END")
        label = self.accept("IDENT")
        self.eat(";")
        return BEGIN(token, self.scope(), body, label and label.value)

    def assignment_statement(self) -> SET:
        token = self.eat("SET")
        variable = self.variable_reference()
        self.eat(":=")
        targets = [variable]

        while True:
            current_i = self.i
            if self.current().type != "IDENT":
                break
            variable = self.variable_reference()
            if self.current().value != ":=":
                self.i = current_i
                break
            self.eat(":=")
            targets.append(variable)

        expression = self.expression()
        self.eat(";")
        return SET(token, self.scope(), targets, expression)

    def variable_reference(self) -> VariableReference:
        token = self.eat("IDENT")
        name = token.value
        parts: list[VariableField | VariableSubscript] = []
        while True:
            if self.accept("."):
                field_token = self.eat("IDENT")
                parts.append(VariableField(field_token, field_token.value))
                continue
            if self.accept("["):
                subscript_token = self.current()
                subscript = self.expression()
                parts.append(VariableSubscript(subscript_token, subscript))
                self.eat("]")
                continue
            break
        return VariableReference(token, self.scope(), name, parts)

    def call_statement(self) -> CALL:
        token = self.eat("CALL")
        name = self.eat("IDENT").value
        arguments = []
        if self.accept("("):
            arguments = self.arguments()
            self.eat(")")
        self.eat(";")
        return CALL(token, self.scope(), name, arguments)

    def expression(self) -> Expression:
        return self.expression_OR_XOR()

    def expression_OR_XOR(self) -> Expression:
        left = self.expression_AND()
        while operation := self.accept(("|", "XOR")):
            right = self.expression_AND()
            left = BinaryOperation(operation, self.scope(), "BOOLEAN", operation.value, left, right)
        return left

    def expression_AND(self) -> Expression:
        token = self.current()
        left = self.expression_NOT()
        while operation := self.accept("&"):
            right = self.expression_NOT()
            left = BinaryOperation(token, self.scope(), "BOOLEAN", operation.value, left, right)
        return left

    def expression_NOT(self) -> Expression:
        if token := self.accept("NOT"):
            return UnaryOperation(token, self.scope(), token.type, token.value, self.expression_NOT())
        return self.expression_RELATION()

    def expression_RELATION(self) -> Expression:
        token = self.current()
        left = self.expression_CONCATENATION()
        while operation := self.accept(("<", ">", "=", "<=", ">=", "<>")):
            right = self.expression_CONCATENATION()
            left = BinaryOperation(token, self.scope(), "BOOLEAN", operation.value, left, right)
        return left

    def expression_CONCATENATION(self) -> Expression:
        parts = [self.expression_ADDING()]
        while self.accept("||"):
            token = self.current()
            part = self.expression_ADDING()
            parts.append(part)
        if len(parts) == 1:
            return parts[0]
        return ConcatenationOperation(token, self.scope(), "STRING", parts)

    def expression_ADDING(self) -> Expression:
        left = self.expression_MULTIPLYING()
        operations = ("+", "-")
        while operation := self.accept(operations):
            right = self.expression_MULTIPLYING()
            left = BinaryOperation(operation, self.scope(), type, operation.value, left, right)
        return left

    def expression_MULTIPLYING(self) -> Expression:
        token = self.current()
        left = self.expression_FUNCTION_CALL()
        while operation := self.accept(("*", "/", "MOD")):
            right = self.expression_FUNCTION_CALL()
            left = BinaryOperation(token, self.scope(), type, operation.value, left, right)
        return left

    def expression_FUNCTION_CALL(self) -> Expression | FunctionCall:
        token = self.current()
        if token.type == "IDENT" and self.peek().value == "(":
            name = self.eat("IDENT").value
            self.eat("(")
            if self.current().value != ")":
                arguments = self.arguments()
            else:
                arguments = []
            self.eat(")")
            type = self.context.functions.get(name)
            if type is None:
                raise ParseError(f"undefined function '{name}'", token)
            return FunctionCall(token, self.scope(), type, name, arguments)
        return self.factor()

    def factor(
        self,
    ) -> IntegerLiteral | RealLiteral | StringLiteral | BoolLiteral | VariableReference | UnaryOperation | Expression:
        token = self.current()
        if token.type == "INTEGER":
            token = self.eat(token.type)
            return IntegerLiteral(token, self.scope(), token.type, int(token.value))
        if token.type == "REAL":
            token = self.eat(token.type)
            return RealLiteral(token, self.scope(), token.type, float(token.value))
        if token.type == "STRING":
            token = self.eat(token.type)

            scope = ""
            context = self.context

            existing_const = next(
                (v for v in context.variables.values() if v.is_const() and v.zero == token.value), None
            )
            if existing_const:
                variable_reference = VariableReference(token, scope, existing_const.name, [])
                return variable_reference

            const_i = sum(1 for v in context.variables.values() if v.is_const())
            name = f"${const_i}"

            variable = Variable(token, name, StringType(), zero=token.value)

            enlist_variable(variable, scope)
            variable_reference = VariableReference(token, scope, name, [])
            return variable_reference
        if token.value in ("+", "-"):
            token = self.eat(token.value)
            factor = self.factor()
            return UnaryOperation(token, self.scope(), token.type, token.value, factor)
        if token.value == "(":
            self.eat("(")
            expression = self.expression()
            self.eat(")")
            return expression
        if token.value in ("TRUE", "FALSE"):
            token = self.eat(token.value)
            return BoolLiteral(token, self.scope(), "BOOLEAN", token.value == "TRUE")
        if token.type == "IDENT":
            variable = self.variable_reference()
            return variable
        raise ParseError(
            "expected an identifier or INTEGER/REAL/BOOLEAN/STRING literal or '+', '-', '(', 'TRUE/FALSE'",
            token,
        )


def expand_variable_reference(variable: Variable, variable_reference: VariableReference) -> tuple[Type, str]:
    context = variable.token.context
    if context.flags.get("index_check") == "0":
        return expand_variable_reference_direct(variable, variable_reference)
    return expand_variable_reference_bound_checked(variable, variable_reference)


def expand_variable_reference_bound_checked(
    variable: Variable,
    variable_reference: VariableReference,
) -> tuple[Type, str]:
    variable_type: Type = variable.type

    # expression for current object (or a *pointer* to it after a subscript)
    reference_expression = variable.name

    # for clean typeof(...) chains like typeof(m.data[0].data[0])
    probe_expression = variable.name

    # True iff reference_expression currently denotes a pointer to an aggregate we're indexing into.
    is_pointer = False

    parts = list(variable_reference.parts)
    if not parts:
        return variable_type, reference_expression

    result_reference: str | None = None

    for i, part in enumerate(parts):
        if isinstance(part, VariableSubscript):
            # resolve AliasType if needed
            subscript_type = variable_type.reference_type if isinstance(variable_type, AliasType) else variable_type

            if not isinstance(subscript_type, ArrayType):
                raise GenerateError(
                    f"expect ArrayType in reference type of subscript, not {subscript_type} at {part.token}"
                )
            array_type: ArrayType = subscript_type

            lo = array_type.lo.c()
            hi = array_type.hi.c()
            index = part.index()

            # clean element typeof/sizeof
            element_typeof = f"typeof({probe_expression}.data[0])"
            element_sizeof = f"sizeof({element_typeof})"

            # "data" part for $ref at the current level
            data_expression = f"({reference_expression})->data" if is_pointer else f"{reference_expression}.data"

            location = '"' + str(part.token).replace('"', r"\"") + '"'
            current_reference = (
                f"({element_typeof} *)$ref({data_expression}, {index}, {lo}, {hi}, {element_sizeof}, {location})"
            )

            is_last = i == len(parts) - 1
            if is_last:
                # final dimension: return an lvalue via *cast($ref(...))
                result_reference = f"*{current_reference}"
            else:
                # keep a *pointer* to the selected element for chaining
                reference_expression = current_reference
                is_pointer = True
                probe_expression = f"{probe_expression}.data[0]"

            variable_type = array_type.type  # step into element type

        elif isinstance(part, VariableField):
            # resolve AliasType if needed
            field_type = variable_type.reference_type if isinstance(variable_type, AliasType) else variable_type

            if not isinstance(field_type, StructType):
                raise GenerateError(f"expect StructType in reference type of field, not {field_type}, at {part.token}")
            struct_type: StructType = field_type

            field = next((f for f in struct_type.fields if f.name == part.name), None)
            if field is None:
                raise GenerateError(f"field '{part.name}' not found in {struct_type} at {part.token}")

            # Choose '.' vs '->' based on whether reference_expression is currently a pointer.
            accessor = f"->{part.name}" if is_pointer else f".{part.name}"
            # Parenthesize before '->' to bind correctly.
            reference_expression = (
                f"({reference_expression}){accessor}" if is_pointer else f"{reference_expression}{accessor}"
            )
            # probe_expression is a value-style chain (always uses '.')
            probe_expression = f"{probe_expression}.{part.name}"

            variable_type = field.type

            # After selecting a field, the expression is an lvalue (not a pointer) unless the type system
            # models pointer fields explicitly. If yes, we can set is_pointer = isinstance(t, PointerType).
            # In C, thought, it is always an lvalue.
            is_pointer = False

        else:
            raise GenerateError(f"unexpected variable '{part=}' at {variable.token}")

    # If no subscript or field ever occurred, just return the plain field chain lvalue.
    if result_reference is None:
        result_reference = reference_expression

    return variable_type, result_reference


def expand_variable_reference_direct(variable: Variable, variable_reference: VariableReference) -> tuple[Type, str]:
    type = variable.type
    reference = variable.name
    for part in variable_reference.parts:
        if isinstance(part, VariableSubscript):
            if isinstance(type, AliasType):
                type = type.reference_type

            if not isinstance(type, ArrayType):
                raise GenerateError(f"expect ArrayType in reference type of subscript, not {type} at {part.token}")

            lo = type.lo.c()
            index = f"({part.index()}) - ({lo})"
            type = type.type
            reference += ".data[" + index + "]"
        elif isinstance(part, VariableField):
            if isinstance(type, AliasType):
                type = type.reference_type

            if not isinstance(type, StructType):
                raise GenerateError(f"expect StructType in reference type of field, not {type} at {part.token}")

            field = next((f for f in type.fields if f.name == part.name), None)
            if field is None:
                raise GenerateError(f"field '{part.name}' not found in {type} at {part.token}")

            type = field.type
            reference += part.c()
        else:
            raise GenerateError(f"unexpected part '{part}' at {variable.token}")
    return type, reference


def discover_variable(v: VariableReference) -> Variable:
    scope = v.scope.split(".")
    context = v.token.context
    while scope and not (variable := context.variables.get(".".join(scope) + "|" + v.name)):
        scope.pop()

    if variable is None:
        raise GenerateError(f"undefined variable '{v.name}' in scope '{v.scope}' at {v.token}")
    return variable


def enlist_type(name: str, type: Type, context: Context) -> None:
    if name in context.types:
        raise GenerateError(f"type '{name}' already defined: {context.types[name]=} at {type.token}")
    context.types[name] = type


def enlist_variable(variable: Variable, scope: str) -> None:
    context = variable.token.context
    fqn = scope + "|" + variable.name
    context.variables[fqn] = variable


class Compiler:
    lexer: Lexer
    parser: Parser

    def __init__(self, text: InputText):
        self.context = Context(
            flags={},
            text=text,
            tokens=[],
            common=[],
            types={
                "INTEGER": IntegerType(),
                "REAL": RealType(),
                "BOOLEAN": BooleanType(),
                "STRING": StringType(),
            },
            functions={
                "LENGTH": BuiltinFunction("LENGTH", IntegerType()),
                "CHARACTER": BuiltinFunction("CHARACTER", StringType()),
                "SUBSTR": BuiltinFunction("SUBSTR", StringType()),
                "FIX": BuiltinFunction("FIX", IntegerType()),
                "FLOAT": BuiltinFunction("FLOAT", RealType()),
            },
            procedures={},
            variables={},
        )

    def compile(self) -> PROGRAM:
        self.lexer = Lexer(self.context)
        self.context.tokens = self.lexer.tokenize()

        self.parser = Parser(self.context)
        program = self.parser.program()
        return program

    def generate(self, program: PROGRAM) -> str:
        lines = [
            "#include <stdio.h>",
            "#include <stdlib.h>",
            "#include <string.h>",
            "#include <stdbool.h>",
            '#include "runtime.h"',
            "",
        ]

        if self.context.common:
            lines.append("// Common declarations")
            for declaration in self.context.common:
                lines.append(declaration.c())
            lines.append("")

        if self.context.types:
            lines.append("// Type definitions")
            for type in self.context.types.values():
                if isinstance(type, AliasType):
                    continue
                lines.append(type.c())
            lines.append("")

        if self.context.variables:
            lines.append("// Global variables")
            for variable in self.context.variables.values():
                if variable.is_const():
                    lines.append(variable.const() + ";")
                else:
                    lines.append(variable.c() + ";")
            lines.append("")

        if self.context.procedures:
            lines.append("// Procedures")
            for procedure in self.context.procedures.values():
                lines.append(procedure.c())
                lines.append("")

        if self.context.functions:
            lines.append("// Functions")
            for function in self.context.functions.values():
                lines.append(function.c())
                lines.append("")

        lines.append("// Main program")
        lines.append(program.c())

        return emit(lines)


#


def run(args: list[str]) -> None:
    if len(args) < 2:
        exe = Path(args[0]).name
        print(f"usage: {exe} <input.easy> [-c <output.c>] [-t] [-a] [-e]")
        print("  -c <output.c>  - specify output C file (default: input.c)")
        print("  -t             - generate tokens file (default: input.tokens)")
        print("  -a             - generate YAML AST file (default: input.yaml)")
        print("  -e             - generate PEG YAML AST file (default: input.peg.yaml)")
        print("  -s <output.s>  - generate symbols file (default: input.s)")
        sys.exit(1)

    input_file = Path(args[1])
    compiler = Compiler(InputText(filename=input_file))
    context = compiler.context

    source = input_file.read_text()
    flags_comment = source.splitlines()[0].strip()
    if flags_comment.startswith("//easy:flags "):
        flags_pairs = flags_comment.split()[1:]
        flags = {k: v for k, v in (pair.split("=") for pair in flags_pairs)}
        context.flags.update(flags)

    program = compiler.compile()

    tokens = context.tokens
    if "-t" in args:
        tokens_file = input_file.with_suffix(".tokens")

        def format_token(token: Token) -> str:
            input = token.context.text
            return f"{input.filename}:{token.line}:{token.character}\t {token.value} / {token.type}"

        tokens_file.write_text("\n".join(format_token(t) for t in tokens) + "\n")

    if "-a" in args:
        ast_file = input_file.with_suffix(".yaml")
        ast_file.write_text(yamlizer(program))

    if "-e" in args:
        grammar = Path("peg/easy.peg").read_text()
        peg_ast = PEGParser(grammar, start="compilation").parse(source)

        peg_ast_file = input_file.with_suffix(".peg.yaml")
        peg_ast_file.write_text(yamlizer(peg_ast))

    output_s = Path(arg(args, "-s") or input_file.with_suffix(".s"))
    with open(output_s, "w") as f:
        v = []
        for name, variable in context.variables.items():
            v.append(variable.s(name))
        print(table(v), file=f)

        v = []
        for name, type in context.types.items():
            v.append((name, re.sub(r"\s+", " ", type.c())))
        print(table(v), file=f)

        v = []
        for name, function in context.functions.items():
            is_builtin = isinstance(function, BuiltinFunction)
            if is_builtin:
                v.append((function.name, "->", function.type.c(), "built-in"))
                continue
            arguments = ", ".join([f"{v.name} {v.type.c()}" for v in function.arguments])
            v.append(
                (
                    function.name,
                    "->",
                    function.type.c(),
                    function.__class__.__name__,
                    f"({arguments})",
                    str(function.token),
                )
            )
        print(table(v), file=f)

        v = []
        for name, procedure in context.procedures.items():
            arguments = ", ".join([f"{v.name} {v.type.c()}" for v in procedure.arguments])
            v.append((procedure.name, procedure.__class__.__name__, f"({arguments})", str(procedure.token)))
        print(table(v), file=f)

    output_c = Path(arg(args, "-c") or input_file.with_suffix(".c"))

    code_c = program.c().strip()

    with open(output_c, "w") as f:
        f.write('#include "runtime.c"\n')
        for name, definition in context.types.items():
            v = f"{definition.typedef(name)};\n"
            f.write(v)
        if context.common:
            f.write(emit(context.common) + "\n")
        for name, v in context.variables.items():
            if v.is_const():
                f.write(v.const() + ";\n")
        for v in context.functions.values():
            if isinstance(v, BuiltinFunction):
                continue
            f.write(v.c() + "\n")
        for v in context.procedures.values():
            f.write(v.c() + "\n")
        f.write(code_c + "\n")


if __name__ == "__main__":
    run(sys.argv)
