from dataclasses import dataclass
from typing import Any, Optional, Tuple, Union

from lexer import Token


@dataclass
class Node:
    token: Token
    scope: str

    def c(self) -> str:
        print(self)
        raise NotImplementedError()

    def __str__(self) -> str:
        import yamler

        return yamler.yamlizer(self)


@dataclass
class Statement(Node):
    pass


@dataclass
class Type:
    def c(self) -> str:
        print(self)
        raise NotImplementedError

    def zero(self) -> str:
        print(self)
        raise NotImplementedError

    def typedef(self, alias: str = "") -> str:
        print(self)
        raise NotImplementedError

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
        return self.lo.c() + " + " + self.hi.c() + " + 1"

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
                    common.append(c)
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
        function = functions_list.get(self.name)
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
                assert False, f"unsupported variable '{variable}' type in INPUT at {variable.token}"
        return emit(inputs)


@dataclass
class OUTPUT(Statement):
    arguments: list[Expression]

    def c(self) -> str:
        output = []
        format: list[str] = []
        arguments = ", ".join(expression_stringer(argument, format, "OUTPUT") for argument in self.arguments)
        output.append(f'output("{"".join(format)}", {arguments});')
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
        output.append(f'concat("{"".join(format)}", {arguments})')
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
        return "[" + self.value.c() + "]"


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
        assert self.is_const()
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
            raise Exception(f"unsupported {callee} variable argument {v} of type {type} at {v.token}")

        convert = type.format()
        format.append(convert)
        return reference

    if isinstance(v, ConcatenationOperation):
        format.append("A")
        return c

    if isinstance(v, FunctionCall):
        function = functions_list[v.name]
        type = function.type

        if not isinstance(type, BuiltinType):
            raise Exception(f"unsupported {callee} function argument ${v} of type {type} at {v.token}")
        convert = type.format()
        format.append(convert)
        return c

    assert False, f"unsupported {callee} argument {v} at {v.token}"


common: list[str] = []

types_list: dict[str, Type] = {
    "INTEGER": IntegerType(),
    "REAL": RealType(),
    "BOOLEAN": BooleanType(),
    "STRING": StringType(),
}


@dataclass
class BuiltinFunction:
    name: str
    type: Type


functions_list: dict[str, BuiltinFunction | FUNCTION] = {
    "LENGTH": BuiltinFunction("LENGTH", IntegerType()),
    "CHARACTER": BuiltinFunction("CHARACTER", StringType()),
    "SUBSTR": BuiltinFunction("SUBSTR", StringType()),
    "FIX": BuiltinFunction("FIX", IntegerType()),
    "FLOAT": BuiltinFunction("FLOAT", RealType()),
}

procedures_list: dict[str, PROCEDURE] = {}

variables_list: dict[str, Variable] = {}


# ###
class ParseError(Exception):
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
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.i = 0
        self.scopes = []

    def scope(self) -> str:
        return ".".join(self.scopes) if self.scopes else "@"

    def enter_scope(self, name: str) -> None:
        self.scopes.append(name)

    def leave_scope(self) -> None:
        self.scopes.pop()

    def error(self, message: str, token: Token) -> None:
        raise ParseError(message, token)

    def current(self) -> Token:
        return self.tokens[self.i]

    def eat(self, kind: str | tuple[str, ...]) -> Token:
        kinds = (kind,) if isinstance(kind, str) else kind
        token = self.current()
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        expected = "/".join(kinds)
        self.error(f"expected '{expected}', found '{token.value}'", token)

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
            self.enlist_type(name, definition)
        return out

    def enlist_type(self, name: str, type: Type) -> None:
        assert name not in types_list, f"type '{name=}' already defined as {types_list[name]=}"
        types_list[name] = type

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
                variables_list[self.scope() + "|" + name] = variable
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
                    variables_list[self.scope() + "|" + name] = variable
                continue
            self.error("expected a variable or '(' variable, ... ')'", token)
        return declarations

    def parse_type(self) -> Type:
        token = self.current()
        if token.value in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
            self.eat(token.value)
            return types_list[token.value]
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

        if (alias_type := types_list.get(alias_name)) is None:
            raise ParseError(f"unknown type alias '{alias_name}'", token)

        return AliasType(alias_name, alias_type)

    def procedures_and_functions_section(self) -> list[PROCEDURE | FUNCTION]:
        subroutines: list[PROCEDURE | FUNCTION] = []
        while token := self.accept(("FUNCTION", "PROCEDURE")):
            name = self.eat("IDENT").value
            self.enter_scope(token.value + ":" + name)

            parameters: list[tuple[str, Type]] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                    for parameter in parameters:
                        variables_list[self.scope() + "|" + parameter.name] = parameter
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
                procedures_list[name] = subroutine
            else:
                subroutine = FUNCTION(token, self.scope(), name, type, parameters, segment)
                functions_list[name] = subroutine

            self.leave_scope()
            subroutines.append(subroutine)

        return subroutines

    def enlist_variable(self, variable: Variable, scope: str | None = None) -> None:
        if scope is None:
            scope = self.scope()
        fqn = scope + "|" + variable.name
        variables_list[fqn] = variable

    def parameters(self) -> list[Variable]:
        parameters: list[Variable] = []
        while True:
            token = self.eat("IDENT")
            name = token.value
            type = self.parse_type()
            variable = Variable(token, name, type)
            parameters.append(variable)
            self.enlist_variable(variable)
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
            type = functions_list.get(name)
            if type is None:
                self.error(f"undefined function [{name}]", token)
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

            existing_const = next((v for v in variables_list.values() if v.is_const() and v.zero == token.value), None)
            if existing_const:
                variable_reference = VariableReference(token, scope, existing_const.name, [])
                return variable_reference

            const_i = sum(1 for v in variables_list.values() if v.is_const())
            name = f"${const_i}"

            variable = Variable(token, name, StringType(), zero=token.value)

            self.enlist_variable(variable, scope)
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
            "expected an identifier or INTEGER/REAL/STRING literal or '+', '-', '(', 'TRUE/FALSE'",
            token,
        )


def yamlizer(obj: Any) -> str:
    import yamler

    return yamler.yamlizer(obj)


def expand_variable_reference(variable: Variable, variable_reference: VariableReference) -> tuple[Type, str]:
    type = variable.type
    reference = variable.name
    for part in variable_reference.parts:
        if isinstance(part, VariableSubscript):
            if isinstance(type, AliasType):
                type = type.reference_type
            assert isinstance(type, ArrayType), f"reference type of subscript must be ArrayType, not {type}"
            type = type.type
            reference += ".data" + part.c()
        elif isinstance(part, VariableField):
            if isinstance(type, AliasType):
                type = type.reference_type
            assert isinstance(
                type, StructType
            ), f"reference type of field must be StructType, not {type}, at {part.token}"
            field = next((f for f in type.fields if f.name == part.name), None)
            assert field is not None, f"field '{part.name}' not found in {type}"
            type = field.type
            reference += part.c()
        else:
            assert False, f"unexpected part '{part}' at {variable.token}"
    return type, reference


def discover_variable(v: VariableReference) -> Variable:
    scope = v.scope.split(".")
    while scope and not (variable := variables_list.get(".".join(scope) + "|" + v.name)):
        scope.pop()

    if variable is None:
        print(yamlizer(v))
        print(yamlizer(variables_list))
        raise Exception(f"undefined variable '{v.name}' in scope '{v.scope}' at {v.token}")
    return variable
