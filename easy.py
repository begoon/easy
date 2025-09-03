import pathlib
import sys
from dataclasses import dataclass
from typing import Literal, Optional, Tuple, Union, cast

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

globals: str = ""


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

    def meta(self):
        print(self)
        raise NotImplementedError()


@dataclass
class Statement(Node):
    pass


@dataclass
class Expression(Node):
    pass


@dataclass
class Label(Node):
    name: str

    def meta(self) -> str:
        return f"LABEL {self.name}"

    def c(self) -> str:
        return f"{self.name}: "


Subroutine = Union["FunctionStatement", "ProcedureStatement"]


@dataclass
class Statements(Node):
    statements: list[Statement]

    def meta(self) -> str:
        return block(indent(statement.meta(), 1) for statement in self.statements) if self.statements else "(empty)"

    def c(self) -> str:
        return block(statement.c().strip() for statement in self.statements) if self.statements else "(empty)"


@dataclass
class Segment:
    types: list["TypeIsStatement"]
    variables: list["DeclareStatement"]
    subroutines: list[Subroutine]
    statements: Statements

    def meta(self) -> str:
        parts = []
        if self.types:
            parts.append("Types:")
            for type in self.types:
                parts.append(indent(f"{type.name}: {type.definition.meta()}", 1))
        if self.variables:
            parts.append("Variables:")
            for variable in self.variables:
                type = variable.type.meta() if isinstance(variable.type, Node) else variable.type
                names = ", ".join(variable.names)
                parts.append(indent(f"{names}: {type}", 1))
        if self.subroutines:
            parts.append("Subroutines:")
            for subroutine in self.subroutines:
                parts.append(indent(subroutine.meta(), 1))
        parts.append("Statements:")
        parts.append(self.statements.meta())
        return block(parts)

    def c(self, /, main: bool = False) -> str:
        global globals
        parts = []
        if self.variables:
            variables = []
            for v in self.variables:
                if isinstance(v.type, Array):
                    for name in v.names:
                        variables.append(f"{(v.type.c(name))};")
                elif isinstance(v.type, StructureStatement):
                    for name in v.names:
                        variables.append(f"{(v.type.c())} {name} = {{0}};")
                else:
                    names = ", ".join(v.names)
                    variables.append(f"{(TYPE(v.type))} {names} = {{0}};")
            if main:
                globals += block(variables) + "\n"
            else:
                parts.extend(variables)
        if self.subroutines:
            subroutines = []
            for subroutine in self.subroutines:
                subroutines.append(subroutine.c())
            globals += block(subroutines) + "\n"
        parts.append(self.statements.c())
        return block(parts)


@dataclass
class DeclareStatement:
    names: list[str]
    type: "Type"


@dataclass
class TypeIsStatement:
    name: str
    definition: "Type"


@dataclass
class Array(Node):
    type: "Type"
    start: Optional[Expression]
    end: Expression

    def c(self, variable: str) -> str:
        assert variable, "Array.c() requires a name parameter"
        start = self.start.c() if self.start else "0"
        end = self.end.c()
        t = f"[{start} + {end} + /* @ */ 1]"
        v = self.type
        while isinstance(v, Array):
            t += f"[{v.start.c() if v.start else '0'} + {v.end.c()}]"
            v = v.type
        return f"{TYPE(v)} {variable}{t}"

    def meta(self) -> str:
        start = self.start.meta() + ":" if self.start else ""
        end = self.end.meta()
        return f"ARRAY[{start}{end}] OF {self.type}"


@dataclass
class FieldStatement:
    name: str
    type: "Type"


@dataclass
class StructureStatement:
    fields: list[FieldStatement]

    def meta(self) -> str:
        v = ["STRUCTURE", " ".join(f"FIELD {field.name} IS {field.type}" for field in self.fields), "END STRUCTURE"]
        return " ".join(v)

    def c(self) -> str:
        v = ["struct {", " ".join(f"{TYPE(field.type)} {field.name};" for field in self.fields), "}"]
        return " ".join(v)


Type = Union[Literal["INTEGER", "REAL", "BOOLEAN", "STRING"], Array, StructureStatement, str]


@dataclass
class ProcedureStatement(Node):
    name: str
    arguments: list[Tuple[str, Type]]
    segment: Segment

    def meta(self) -> str:
        arguments = ", ".join(name for name, _ in self.arguments)
        v = [
            f"PROCEDURE {self.name}({arguments})",
            indent(self.segment.meta(), 1),
        ]
        return block(v)

    def c(self) -> str:
        arguments = ", ".join(f"{TYPE(type)} {name}" for name, type in self.arguments)
        v = [
            f"void {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return block(v)


@dataclass
class FunctionStatement(Node):
    name: str
    type: Type
    arguments: list[Tuple[str, Type]]
    segment: Segment

    def meta(self) -> str:
        arguments = ", ".join(f"{type} {name}" for name, type in self.arguments)
        v = [
            f"FUNCTION {self.name}({arguments}) : {self.type}",
            indent(self.segment.meta(), 1),
        ]
        return block(v)

    def c(self) -> str:
        arguments = ", ".join(f"{TYPE(type)} {name}" for name, type in self.arguments)
        v = [
            f"{TYPE(self.type)} {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return block(v)


@dataclass
class SetStatement(Statement):
    name: str
    expression: Expression
    indexes: list[Optional[Expression]]

    def meta(self) -> str:
        indexes = "".join(f"[{index.meta()}]" for index in self.indexes) if self.indexes else ""
        return f"SET {self.name}{indexes} := {self.expression.meta()}"

    def c(self) -> str:
        global variables_registry
        type = variables_registry.get(self.name)
        if type == "STRING":

            def T(v: str) -> str:
                if v.startswith('""'):
                    v = v[0] + "\\" + v[1:]
                if v.endswith('""'):
                    v = v[:-2] + "\\" + v[-2:]
                return v

            return f"strcpy({self.name}.data, {T(self.expression.c())});"
        indexes = "".join(f"[{index.c()}]" for index in self.indexes) if self.indexes else ""
        return f"{self.name}{indexes} = {self.expression.c()};"


@dataclass
class IfStatement(Statement):
    cond: Expression
    then_branch: Segment
    else_branch: Optional[Segment]

    def meta(self) -> str:
        v = [f"IF {self.cond.meta()} THEN", indent(self.then_branch.meta(), 1)]
        if self.else_branch:
            v.append("ELSE")
            v.append(indent(self.else_branch.meta(), 1))
        v.append("FI")
        return block(v)

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
        return block(v)


@dataclass
class ForStatement(Statement):
    variable: "Variable"
    init: Expression
    do: Segment
    by: Optional[Expression] = None
    to: Optional[Expression] = None
    condition: Optional[Expression] = None

    def meta(self) -> str:
        v = f"FOR {self.variable.meta()} := {self.init.meta()}"

        if self.by:
            v += f" BY {self.by.meta()}"
        if self.to:
            v += f" TO {self.to.meta()}"
        if self.condition:
            v += f" WHILE {self.condition.meta()}"

        v += " DO"

        return block([v, indent(self.do.meta(), 1), "END FOR"])

    def c(self) -> str:
        v = [
            f"for ({self.variable.c()} = {self.init.c()}; {self.format_condition()}; {self.step()})",
            "{",
            indent(self.do.c(), 1),
            "}",
        ]
        return block(v)

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
class SelectStatement(Statement):
    expr: Expression
    cases: list[Tuple[Expression, Segment]]

    def meta(self) -> str:
        v = [f"SELECT {self.expr.meta()}"]
        for condition, body in self.cases:
            if condition is not None:
                condition = condition.meta()
                v.append(indent(f"CASE {condition}:", 1))
            else:
                v.append(indent("OTHERWISE:", 1))
            v.append(f"{indent(body.meta(), 2)}")
        v.append("END SELECT")
        return block(v)

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
        return block(v)


@dataclass
class InputStatement(Statement):
    variables: list[str]

    def meta(self) -> str:
        return f"INPUT({', '.join(str(e) for e in self.variables)})"

    def c(self) -> str:
        inputs = []
        for variable in self.variables:
            assert variable in variables_registry, f"undeclared variable '{variable}'"
            type = variables_registry[variable]
            if type == "STRING":
                inputs.append(f'scanf("%s", {variable}.data);')
            else:
                assert type == "INTEGER", f"unexpected variable type in INPUT '{variable}': {type}"
                inputs.append(f'scanf("%d", &{variable});')
        return block(inputs)


@dataclass
class OutputStatement(Statement):
    arguments: list[Expression]

    def meta(self) -> str:
        return f"OUTPUT({', '.join(e.meta() for e in self.arguments)})"

    def c(self) -> str:
        def format(argument: Expression) -> str:
            v = argument.c()
            if isinstance(argument, Variable):
                assert argument.name in variables_registry, f"undeclared variable in OUTPUT '{argument}'"
                type = variables_registry[argument.name]
                if type != "STRING":
                    return f"str({v})"
            return v

        arguments = ", ".join(format(argument) for argument in self.arguments)
        return f"output({len(self.arguments)}, {arguments});"


@dataclass
class RepeatStatement(Statement):
    label: str

    def meta(self) -> str:
        return f"REPEAT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class RepentStatement(Statement):
    label: str

    def meta(self) -> str:
        return f"REPENT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"


@dataclass
class BeginStatement(Statement):
    body: Segment
    label: Optional[str] = None

    def meta(self) -> str:
        v = ["BEGIN", indent(self.body.meta(), 1), "END"]
        if self.label:
            v.append("LABEL " + self.label)
        v.append(";")
        return block(v)

    def c(self) -> str:
        v = ["{", indent(self.body.c(), 1), "}"]
        if self.label:
            v.append(self.label + ":")
        return block(v)


@dataclass
class CallStatement(Statement):
    name: str
    arguments: list[Expression]

    def meta(self) -> str:
        arguments = ", ".join(arg.meta() for arg in self.arguments)
        return f"CALL {self.name}({arguments})"

    def c(self) -> str:
        arguments = ", ".join(arg.c() for arg in self.arguments)
        return f"{self.name}({arguments});"


@dataclass
class ReturnStatement(Statement):
    value: Optional[Expression] = None

    def meta(self) -> str:
        value = self.value.meta() if isinstance(self.value, Expression) else self.value
        return "RETURN" + f" {value}" if self.value is not None else ""

    def c(self) -> str:
        value = self.value.c() if isinstance(self.value, Expression) else self.value
        return "return" + (f" {value}" if self.value is not None else "") + ";"


@dataclass
class ExitStatement(Statement):
    def meta(self) -> str:
        return "EXIT"

    def c(self) -> str:
        return "exit(0);"


@dataclass
class EmptyStatement(Statement):
    def meta(self) -> str:
        return "EMPTY"

    def c(self) -> str:
        return ";"


@dataclass
class FunctionInvoke(Expression):
    name: str
    arguments: list[Expression]

    def meta(self) -> str:
        return f"{self.name}({', '.join(a.meta() for a in self.arguments)})"

    def c(self) -> str:
        def format(argument: Expression) -> str:
            if isinstance(argument, Variable):
                type = variables_registry.get(argument.name)
                if type == "STRING":
                    return f"{argument.name}.data"
            return argument.c()

        return f"{self.name}({', '.join(format(a) for a in self.arguments)})"


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression

    def meta(self) -> str:
        return f"({self.left.meta()} {self.operation} {self.right.meta()})"

    def c(self) -> str:
        return f"({self.left.c()} {self.operation} {self.right.c()})"


@dataclass
class ConcatenationOperation(Expression):
    parts: list[Expression]

    def meta(self) -> str:
        def format(v: Expression) -> str:
            if isinstance(v, StringLiteral):
                return repr(v.value)

            if isinstance(v, ConcatenationOperation):
                return v.meta()

            if isinstance(v, FunctionInvoke) and v.name in "CHARACTER":
                return v.meta()

            s = v.meta()
            return f"str({s})"

        parts = [format(v) for v in self.parts]
        return f"{len(parts)}:(" + " || ".join(parts) + ")"

    def c(self) -> str:
        def format(v: Expression) -> str:
            is_concatenation = isinstance(v, ConcatenationOperation)
            is_string_literal = isinstance(v, StringLiteral)
            is_function = isinstance(v, FunctionInvoke) and v.name in ("CHARACTER", "SUBSTR")

            skip_stringify = any([is_string_literal, is_concatenation, is_function])

            str = v.c()
            if isinstance(v, Variable):
                global variables_registry
                type = variables_registry.get(v.name)
                if type == "STRING":
                    return f"{v.name}.data"
                return f"str({str})"
            return str if skip_stringify else f"str({str})"

        parts = [format(v) for v in self.parts]
        return f"concat({len(parts)}, {', '.join(parts)})"


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression

    def meta(self) -> str:
        return f"({self.operation}{self.expr.meta()})"

    def c(self) -> str:
        operation = "!" if self.operation == "NOT" else self.operation
        return f"({operation}{self.expr.c()})"


@dataclass
class Variable(Expression):
    name: str
    indexes: list[Optional[Expression]] = None

    def meta(self) -> str:
        indexes = "".join(f"[{index.meta()}]" for index in self.indexes) if self.indexes else ""
        return self.name + indexes

    def c(self) -> str:
        indexes = "".join(f"[{index.c()}]" for index in self.indexes) if self.indexes else ""
        return self.name + indexes


@dataclass
class IntegerLiteral(Expression):
    value: int

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return str(self.value)


@dataclass
class RealLiteral(Expression):
    value: float

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return str(self.value)


@dataclass
class StringLiteral(Expression):
    value: str

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return f'"{self.value}"'


@dataclass
class BoolLiteral(Expression):
    value: bool

    def meta(self) -> str:
        return "TRUE" if self.value else "FALSE"

    def c(self) -> str:
        return "1" if self.value else "0"


def block(lines: list[str]) -> str:
    return "\n".join(lines)


@dataclass
class ProgramStatement(Node):
    name: str
    segment: Segment

    def meta(self) -> str:
        return block([f"PROGRAM {self.name}", indent(self.segment.meta(), 1)])

    def c(self) -> str:
        return block(["int main()", "{", indent(self.segment.c(main=True), 1), "}"])


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

    def program(self) -> ProgramStatement:
        self.eat("PROGRAM")
        name = self.eat("IDENT").value
        self.eat(":")
        segments = self.segment()
        self.eat("END")
        self.eat("PROGRAM")
        self.eat(name)
        self.eat(";")
        self.eat("EOF")
        return ProgramStatement(name, segments)

    def segment(self) -> Segment:
        types = self.types_section()
        variables = self.variables_section()
        subroutines = self.subroutines()
        statements = self.statements()
        return Segment(types, variables, subroutines, statements)

    def types_section(self) -> list[TypeIsStatement]:
        out: list[TypeIsStatement] = []
        while self.accept("TYPE"):
            name = self.eat("IDENT").value
            self.eat("IS")
            definition = self.type()
            self.eat(";")
            out.append(TypeIsStatement(name, definition))
            types_registry[name] = definition
        return out

    def variables_section(self) -> list[DeclareStatement]:
        global variables_registry

        out: list[DeclareStatement] = []
        while self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.type()
                self.eat(";")
                out.append(DeclareStatement([name], type))
                variables_registry[name] = type
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
                out.append(DeclareStatement(names, type))
                for name in names:
                    variables_registry[name] = type
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
            fields.append(FieldStatement(name, type))

            while self.accept(","):
                name = self.eat("IDENT").value
                self.eat("IS")
                type = self.type()
                fields.append(FieldStatement(name, type))

            self.eat("END")
            self.eat("STRUCTURE")
            return StructureStatement(fields)
        return self.eat("IDENT").value

    def subroutines(self) -> list[ProcedureStatement | FunctionStatement]:
        out: list[ProcedureStatement | FunctionStatement] = []
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
                has_return = any(
                    isinstance(v, ReturnStatement) and v.value is not None for v in segment.statements.statements
                )
                if not has_return:
                    segment.statements.statements.append(ReturnStatement(0))

            self.eat("END")
            self.eat(token.value)
            self.eat(name)
            self.eat(";")

            if token.value == "PROCEDURE":
                assert type is None, f"{type=}"
                out.append(ProcedureStatement(name, parameters, segment))
            else:
                out.append(FunctionStatement(name, type, parameters, segment))
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
        return EmptyStatement()

    def arguments(self) -> list[Expression]:
        v = [self.expression()]
        while self.accept(","):
            v.append(self.expression())
        return v

    def if_statement(self) -> IfStatement:
        self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.segment()
        else_branch = None
        if self.accept("ELSE"):
            else_branch = self.segment()
        self.eat("FI")
        self.eat(";")
        return IfStatement(cond, then_branch, else_branch)

    def for_statement(self) -> ForStatement:
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
        return ForStatement(variable, init, do, by, to, while_)

    def select_statement(self) -> SelectStatement:
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
        return SelectStatement(expr, cases)

    def return_statement(self) -> ReturnStatement:
        self.eat("RETURN")
        if self.accept(";"):
            return ReturnStatement()
        value = self.expression()
        self.eat(";")
        return ReturnStatement(value)

    def exit_statement(self) -> ExitStatement:
        self.eat("EXIT")
        self.eat(";")
        return ExitStatement()

    def input_statement(self) -> InputStatement:
        self.eat("INPUT")
        variables = []
        variables.append(self.eat("IDENT").value)
        while self.accept(","):
            variables.append(self.eat("IDENT").value)
        self.eat(";")
        return InputStatement(variables)

    def output_statement(self) -> OutputStatement:
        self.eat("OUTPUT")
        expressions = [self.expression()]
        while self.accept(","):
            expressions.append(self.expression())
        self.eat(";")
        return OutputStatement(expressions)

    def repeat_statement(self) -> Statements:
        self.eat("REPEAT")
        label = self.eat("IDENT").value
        self.eat(";")
        return RepeatStatement(label)

    def repent_statement(self) -> Statements:
        self.eat("REPENT")
        label = self.eat("IDENT").value
        self.eat(";")
        return RepentStatement(label)

    def begin_statement(self) -> Statements:
        self.eat("BEGIN")
        body = self.segment()
        self.eat("END")
        label = self.accept("IDENT")
        self.eat(";")
        return BeginStatement(body, label and label.value)

    def assignment_statement(self) -> SetStatement:
        self.eat("SET")
        variable = self.variable_name()
        self.eat(":=")
        expr = self.expression()
        self.eat(";")
        return SetStatement(variable.name, expr, variable.indexes)

    def variable_name(self) -> Variable:
        name = self.eat("IDENT").value
        while self.accept("."):
            name += "." + self.eat("IDENT").value
        indexes = []
        while self.accept("["):
            index = self.expression()
            self.eat("]")
            indexes.append(index)
        return Variable(name, indexes)

    def call_statement(self) -> CallStatement:
        self.eat("CALL")
        name = self.eat("IDENT").value
        arguments = []
        if self.accept("("):
            arguments = self.arguments()
            self.eat(")")
        self.eat(";")
        return CallStatement(name, arguments)

    def expression(self) -> Expression:
        return self.expression_OR_XOR()

    def expression_OR_XOR(self) -> Expression:
        left = self.expression_AND()
        while operation := self.accept(("|", "XOR")):
            right = self.expression_AND()
            operation.value = {"|": "||", "XOR": "^"}.get(operation.value, operation.value)
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_AND(self) -> Expression:
        left = self.expression_NOT()
        while operation := self.accept(("&",)):
            right = self.expression_NOT()
            operation.value = "&&"
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_NOT(self) -> Expression:
        if self.accept("NOT"):
            return UnaryOperation("NOT", self.expression_NOT())
        return self.expression_relation()

    def expression_relation(self) -> Expression:
        left = self.expression_CONCATENATION()
        while operation := self.accept(("<", ">", "=", "<=", ">=", "<>")):
            right = self.expression_CONCATENATION()
            operation.value = {"=": "==", "<>": "!="}.get(operation.value, operation.value)
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_CONCATENATION(self) -> Expression:
        parts = [self.expression_ADDING()]
        while self.accept(("||",)):
            parts.append(self.expression_ADDING())
        if len(parts) == 1:
            return parts[0]
        return ConcatenationOperation(parts)

    def expression_ADDING(self) -> Expression:
        left = self.expression_MULTIPLYING()
        while operation := self.accept(("+", "-")):
            right = self.expression_MULTIPLYING()
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_MULTIPLYING(self) -> Expression:
        left = self.expression_FUNCTION_CALL()
        while operation := self.accept(("*", "/", "MOD")):
            right = self.expression_FUNCTION_CALL()
            operation.value = {"MOD": "%"}.get(operation.value, operation.value)
            left = BinaryOperation(operation.value, left, right)
        return left

    def expression_FUNCTION_CALL(self) -> Expression | FunctionInvoke:
        name = self.current()
        if name.type == "IDENT" and self.peek().value == "(":
            self.i += 1
            self.eat("(")
            arguments = self.arguments()
            self.eat(")")
            return FunctionInvoke(name.value, arguments)
        return self.factor()

    def factor(self) -> Expression:
        token = self.current()
        if token.type == "IDENT":
            variable = self.variable_name()
            return variable
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
        self.error(f"unexpected token {token.type}('{token.value}')", token)


def indent(s: str, n: int) -> str:
    pad = "    " * n
    return "\n".join(pad + line for line in s.splitlines())


types_registry: dict[str, str | Array | StructureStatement] = {}
variables_registry: dict[str, str | Array | StructureStatement] = {}


def TYPE(v: str) -> str:
    if isinstance(v, Array):
        return v
    if isinstance(v, StructureStatement):
        return v.meta()
    if v in types_registry:
        return v
    type = {"INTEGER": "int", "REAL": "double", "BOOLEAN": "int", "STRING": "STR"}.get(v)
    if not type:
        raise ValueError(f"unknown type '{v}'")
    return type


def parse(code: str) -> ProgramStatement:
    lexer = Lexer(code)
    tokens = lexer.tokens()
    return Parser(tokens, code).program()


# ---


def compile(source: str) -> ProgramStatement:
    return parse(source)


def flag(argv: list[str], name: str) -> Optional[int]:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> Optional[str]:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: easy.py <input.easy> [-o <output.c>] [-t] [-a]")
        print("  -o <output.c> - specify output C file (default: input.c)")
        print("  -t            - generate tokens file (default: off)")
        print("  -a            - generate AST file (default: off)")
        sys.exit(1)

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
        ast_file.write_text(ast.meta() + "\n")

    compiled = ast.c().strip()

    output = arg(sys.argv, "-o") or input_file.with_suffix(".c")

    with open(output, "w") as f:
        f.write('#include "preamble.c"\n')
        if types_registry:
            for name, definition in types_registry.items():
                v = "typedef "
                if isinstance(definition, Array):
                    v += definition.c(variable=name)
                else:
                    v += definition.c() + " " + name
                v += ";\n"
                f.write(v)
        if globals:
            f.write(globals.strip() + "\n")
        f.write(compiled.strip() + "\n")
