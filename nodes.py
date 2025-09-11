from dataclasses import dataclass
from typing import Callable, Literal, Optional, Tuple, Union

from lexer import Token

types_registry: dict[str, "Type"] = {}
variables_registry: dict[str, "Type"] = {}
functions_registry: dict[str, "Type"] = {
    "LENGTH": "INTEGER",
    "CHARACTER": "STRING",
    "SUBSTR": "STRING",
    "FIX": "INTEGER",
    "FLOAT": "REAL",
}
python_imports: set[str] = set()

common: list[str] = []


@dataclass
class Node:
    token: Token

    def c(self) -> str:
        print(self)
        raise NotImplementedError()

    def py(self) -> str:
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
    type: "Type"


@dataclass
class Label(Node):
    name: str

    def meta(self) -> str:
        return f"LABEL {self.name}"

    def c(self) -> str:
        return f"{self.name}: "

    def py(self) -> str:
        assert False, "labels are not supported in python"


Subroutine = Union["FunctionStatement", "ProcedureStatement"]


@dataclass
class Statements(Node):
    statements: list[Statement]

    def meta(self) -> str:
        return emit(indent(statement.meta(), 1) for statement in self.statements) if self.statements else "(empty)"

    def c(self) -> str:
        return emit(statement.c().strip() for statement in self.statements) if self.statements else "(empty)"

    def py(self) -> str:
        return emit(statement.py().strip() for statement in self.statements) if self.statements else "(empty)"


@dataclass
class Segment(Node):
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
        return emit(parts)

    def c(self, /, main: bool = False) -> str:
        global common
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
                    for name in v.names:
                        variables.append(f"{(TYPE(v.type))} {name} = {{0}};")
            if main:
                common.extend(variables)
            else:
                parts.extend(variables)
        if self.subroutines:
            subroutines = []
            for subroutine in self.subroutines:
                subroutines.append(subroutine.c())
            common.extend(subroutines)
        parts.append(self.statements.c())
        return emit(parts)

    def py(self) -> str:
        parts = []
        if self.variables:
            for v in self.variables:
                type = v.type.meta() if isinstance(v.type, Node) else v.type
                for name in v.names:
                    if type == "INTEGER":
                        parts.append(f"{name} = 0")
                    elif type == "REAL":
                        parts.append(f"{name} = 0.0")
                    elif type == "BOOLEAN":
                        parts.append(f"{name} = False")
                    elif type == "STRING":
                        parts.append(f"{name} = ''")
                    else:
                        raise ValueError(f"unsupported variable type in python: {type} [{self.token}]")
        if self.subroutines:
            for subroutine in self.subroutines:
                parts.append(subroutine.py())
        parts.append(self.statements.py())
        return emit(parts)


@dataclass
class DeclareStatement(Node):
    names: list[str]
    type: "Type"


@dataclass
class TypeIsStatement(Node):
    name: str
    definition: "Type"


@dataclass
class Array(Node):
    type: "Type"
    start: Optional[Expression]
    end: Expression

    def c(self, variable: str) -> str:
        assert variable, f"array.c() requires a name parameter at {self.token}"
        start = self.start.c() if self.start else "0"
        end = self.end.c()
        t = f"[{start} + {end} + /* @ */ 1]"
        v = self.type
        while isinstance(v, Array):
            t += f"[{v.start.c() if v.start else '0'} + {v.end.c()}]"
            v = v.type
        return f"{TYPE(v)} {variable}{t}"

    def py(self) -> str:
        start = self.start.py() + ":" if self.start else ""
        end = self.end.py()
        return f"[] # {start}{end} OF {self.type}"

    def meta(self) -> str:
        start = self.start.meta() + ":" if self.start else ""
        end = self.end.meta()
        return f"ARRAY[{start}{end}] OF {self.type}"


@dataclass
class FieldStatement(Node):
    name: str
    type: "Type"


@dataclass
class StructureStatement(Node):
    fields: list[FieldStatement]

    def meta(self) -> str:
        v = ["STRUCTURE", " ".join(f"FIELD {field.name} IS {field.type}" for field in self.fields), "END STRUCTURE"]
        return " ".join(v)

    def c(self) -> str:
        v = ["struct {", " ".join(f"{TYPE(field.type)} {field.name};" for field in self.fields), "}"]
        return " ".join(v)

    def py(self) -> str:
        v = ["class STRUCTURE:"]
        for field in self.fields:
            type = field.type.meta() if isinstance(field.type, Node) else field.type
            if type == "INTEGER":
                v.append(f"    {field.name}: int = 0")
            elif type == "REAL":
                v.append(f"    {field.name}: float = 0.0")
            elif type == "BOOLEAN":
                v.append(f"    {field.name}: bool = False")
            elif type == "STRING":
                v.append(f"    {field.name}: str = ''")
            else:
                raise ValueError(f"unsupported field type in python: {type} [{self.token}]")
        return "\n".join(v)


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
        return emit(v)

    def c(self) -> str:
        arguments = ", ".join(f"{TYPE(type)} {name}" for name, type in self.arguments)
        v = [
            f"void {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return emit(v)

    def py(self) -> str:
        arguments = ", ".join(name for name, _ in self.arguments)
        v = [
            f"def {self.name}({arguments}):",
            indent(self.segment.py(), 1),
        ]
        return emit(v)


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
        return emit(v)

    def c(self) -> str:
        arguments = ", ".join(f"{TYPE(type)} {name}" for name, type in self.arguments)
        v = [
            f"{TYPE(self.type)} {self.name}({arguments})",
            "{",
            indent(self.segment.c(), 1),
            "}",
        ]
        return emit(v)

    def py(self) -> str:
        arguments = ", ".join(name for name, _ in self.arguments)
        v = [
            f"def {self.name}({arguments}) -> {self.type}:",
            indent(self.segment.py(), 1),
        ]
        return emit(v)


@dataclass
class SetStatement(Statement):
    variables: list["Variable"]
    expression: Expression

    def meta(self) -> str:
        v = "SET "
        for variable in self.variables:
            v += f"{variable.meta()} := "
        v += self.expression.meta()
        return v

    def c(self) -> str:
        v = []
        for variable in self.variables:
            v.append(f"{variable.c()} = {self.expression.c()};")
        return emit(v)

    def py(self) -> str:
        v = []
        for variable in self.variables:
            v.append(f"{variable.py()} = {self.expression.py()}")
        return emit(v)


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
        return emit(v)

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

    def py(self) -> str:
        v = [f"if {self.cond.py()}:", indent(self.then_branch.py(), 1)]
        if self.else_branch:
            v.append("else:")
            v.append(indent(self.else_branch.py(), 1))
        return emit(v)


@dataclass
class ForStatement(Statement):
    variable: "Variable"
    init: Expression
    do: Segment
    by: Optional[Expression] = None
    to: Optional[Expression] = None
    condition: Optional[Expression] = None

    def meta(self) -> str:
        v = [f"FOR {self.variable.meta()} := {self.init.meta()}"]

        if self.by:
            v.append(f"BY {self.by.meta()}")
        if self.to:
            v.append(f"TO {self.to.meta()}")
        if self.condition:
            v.append(f"WHILE {self.condition.meta()}")

        v.append("DO")

        vv = " ".join(v)
        return emit([vv, indent(self.do.meta(), 1), "END FOR"])

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

    def py(self) -> str:
        variable = self.variable.name
        v = [
            f"{variable} = {self.init.py() if self.init else 0}",
            "while True:",
        ]
        if self.to or self.condition:
            v.append(indent(f"if {variable} > {self.to.py()}:", 1))
            v.append(indent("break", 2))
        if self.condition:
            v.append(indent(f"if not {self.condition.py()}:", 1))
            v.append(indent("break", 2))
        v.append(indent(f"{self.do.py()}", 1))
        v.append(indent(f"{variable} += {self.by.py() if self.by else 1}", 1))
        return emit(v)


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
        return emit(v)

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

    def py(self) -> str:
        v = []
        for i, [condition, body] in enumerate(self.cases, 0):
            if condition is not None:
                condition = condition.py()
                v.append(("elif " if i > 0 else "if ") + condition + ":")
            else:
                v.append("else:")
            v.append(indent(body.py(), 1))
        return emit(v)


@dataclass
class InputStatement(Statement):
    variables: list["Variable"]

    def meta(self) -> str:
        return f"INPUT({', '.join(e.meta() for e in self.variables)})"

    def c(self) -> str:
        inputs = []
        for variable in self.variables:
            type = variable.type
            if type == "STRING":
                inputs.append(f'scanf("%s", {variable.name}.data);')
            elif type == "INTEGER":
                inputs.append(f'scanf("%d", &{variable.name});')
            elif type == "REAL":
                inputs.append(f'scanf("%lf", &{variable.name});')
            else:
                assert False, f"unsupported variable type in INPUT [{variable}]"
        return emit(inputs)

    def py(self) -> str:
        inputs = []
        for variable in self.variables:
            assert variable in variables_registry, f"undeclared variable '{variable}'"
            type = variables_registry[variable]
            if type == "STRING":
                inputs.append(f"{variable} = input()")
            else:
                assert type == "INTEGER", f"unexpected variable type in INPUT '{variable}': {type}"
                inputs.append(f"{variable} = int(input())")
        return emit(inputs)


@dataclass
class OutputStatement(Statement):
    arguments: list[Expression]

    def meta(self) -> str:
        format: list[str] = []
        [expression_stringer(argument, format) for argument in self.arguments]
        return f"OUTPUT('{''.join(format)}', {', '.join(e.meta() for e in self.arguments)})"

    def c(self) -> str:
        output = []

        format: list[str] = []
        arguments = ", ".join(expression_stringer(argument, format) for argument in self.arguments)
        output.append(f'output("{"".join(format)}", {arguments});')
        return emit(output)

    def py(self) -> str:
        def format(argument: Expression) -> str:
            if isinstance(argument, Variable):
                name = argument.name.split(".", 1)[0]
                type = variables_registry.get(name)
                if type != "STRING":
                    return f"str({argument.py()})"
                return argument.py()
            return argument.py().replace("'", '"')

        arguments = ", ".join(format(argument) for argument in self.arguments)
        python_imports.add("runtime_print")
        return f"runtime_print({arguments})"


@dataclass
class RepeatStatement(Statement):
    label: str

    def meta(self) -> str:
        return f"REPEAT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"

    def py(self) -> str:
        assert False, "REPEAT is not supported in python"


@dataclass
class RepentStatement(Statement):
    label: str

    def meta(self) -> str:
        return f"REPENT {self.label}"

    def c(self) -> str:
        return f"goto {self.label};"

    def py(self) -> str:
        assert False, "REPENT is not supported in python"


@dataclass
class BeginStatement(Statement):
    body: Segment
    label: Optional[str] = None

    def meta(self) -> str:
        v = ["BEGIN", indent(self.body.meta(), 1), "END"]
        if self.label:
            v.append("LABEL " + self.label)
        v.append(";")
        return emit(v)

    def c(self) -> str:
        v = ["{", indent(self.body.c(), 1), "}"]
        if self.label:
            v.append(self.label + ":")
        return emit(v)

    def py(self) -> str:
        v = []
        v.append(indent(self.body.py(), 1))
        if self.label:
            assert False, "labels are not supported in python"
        return emit(v)


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

    def py(self) -> str:
        arguments = ", ".join(arg.py() for arg in self.arguments)
        return f"{self.name}({arguments}) # CALL {self.name}"


@dataclass
class ReturnStatement(Statement):
    value: Optional[Expression] = None

    def meta(self) -> str:
        value = self.value.meta() if isinstance(self.value, Expression) else self.value
        return "RETURN" + f" {value}" if self.value is not None else ""

    def c(self) -> str:
        value = self.value.c() if isinstance(self.value, Expression) else self.value
        return "return" + (f" {value}" if self.value is not None else "") + ";"

    def py(self) -> str:
        value = self.value.py() if isinstance(self.value, Expression) else self.value
        return "return" + (f" {value}" if self.value is not None else "") + ""


@dataclass
class ExitStatement(Statement):
    def meta(self) -> str:
        return "EXIT"

    def c(self) -> str:
        return "exit(0);"

    def py(self) -> str:
        return "exit(0)"


@dataclass
class EmptyStatement(Statement):
    def meta(self) -> str:
        return "EMPTY"

    def c(self) -> str:
        return ";"

    def py(self) -> str:
        return "pass"


@dataclass
class FunctionCall(Expression):
    name: str
    arguments: list[Expression]

    def meta(self) -> str:
        return f"{self.name}({', '.join(a.meta() for a in self.arguments)})"

    def c(self) -> str:
        return f"{self.name}({', '.join(a.c() for a in self.arguments)})"

    def py(self) -> str:
        return f"{self.name}({', '.join(a.py() for a in self.arguments)}"


@dataclass
class BinaryOperation(Expression):
    operation: str
    left: Expression
    right: Expression

    def meta(self) -> str:
        return f"({self.left.meta()} {self.operation} {self.right.meta()})"

    def c(self) -> str:

        left_type = expand_type(self.left.type, self.token)
        right_type = expand_type(self.right.type, self.token)
        allowed = left_type == right_type or (is_number(left_type) and is_number(right_type))
        assert allowed, (
            f"\n{self=}"
            f"\ntype mismatch in binary operation '{self.operation}':\n"
            f"{self.left=}\n{self.right=}\n" + f"at {self.token}\n"
        )

        def type_mismatch(v: BinaryOperation) -> str:
            return f"unsupported type '{v.left.type}' in binary operation '{v.operation}' at {v.token}"

        if self.operation in ("+", "-", "*", "/", "MOD"):
            operations = {"MOD": "%"}
            self.operation = operations.get(self.operation, self.operation)
            assert left_type in ("INTEGER", "REAL"), type_mismatch(self)

        elif self.operation in ("<", "<=", ">", ">=", "=", "<>", "|", "XOR"):
            operations = {"=": "==", "<>": "!=", "|": "||", "XOR": "^"}
            self.operation = operations.get(self.operation, self.operation)
            assert left_type in ("BOOLEAN", "INTEGER", "REAL"), type_mismatch(self)

        elif self.operation == "||":
            assert left_type in ("STRING",), type_mismatch(self)

        else:
            raise ValueError(f"invalid binary operation [{self.operation}] at {self.token}")

        return f"({self.left.c()} {self.operation} {self.right.c()})"

    def py(self) -> str:
        operations = {"AND": "and", "OR": "or", "=": "==", "<>": "!="}
        operation = operations.get(self.operation, self.operation)
        return f"({self.left.py()} {operation} {self.right.py()})"


@dataclass
class ConcatenationOperation(Expression):
    parts: list[Expression]

    def meta(self) -> str:
        format: list[str] = []
        [expression_stringer(argument, format) for argument in self.parts]
        return f"||:('{''.join(format)}', {', '.join(e.meta() for e in self.parts)})"

    def c(self) -> str:
        output = []
        format: list[str] = []
        arguments = ", ".join(expression_stringer(argument, format, "||") for argument in self.parts)
        output.append(f'concat("{"".join(format)}", {arguments})')
        return emit(output)

    def py(self) -> str:
        def format(v: Expression) -> str:
            is_concatenation = isinstance(v, ConcatenationOperation)
            is_string_literal = isinstance(v, StringLiteral)
            is_function = isinstance(v, FunctionCall) and v.name in ("CHARACTER", "SUBSTR")

            skip_stringify = any([is_string_literal, is_concatenation, is_function])

            str = v.py()
            if isinstance(v, Variable):
                global variables_registry
                type = variables_registry.get(v.name)
                if type == "STRING":
                    return f"{v.name}"
                return f"str({str})"
            return str if skip_stringify else f"str({str})"

        parts = [format(v) for v in self.parts]
        return " + ".join(parts)


@dataclass
class UnaryOperation(Expression):
    operation: str
    expr: Expression

    def meta(self) -> str:
        return f"({self.operation}{self.expr.meta()})"

    def c(self) -> str:
        operation = "!" if self.operation == "NOT" else self.operation
        return f"({operation}{self.expr.c()})"

    def py(self) -> str:
        operation = "not " if self.operation == "NOT" else self.operation
        return f"({operation}{self.expr.py()})"


@dataclass
class Variable(Expression):
    name: str
    parts: Optional[list[Expression | str]] = None

    def fqn(self, f: Callable[[Expression], str]) -> str:
        v = [self.name]
        for part in self.parts or []:
            if isinstance(part, str):
                v.append(part)
            else:
                v.append(f"[{f(part)}]")
        return "".join(v)

    def meta(self) -> str:
        return self.name

    def c(self) -> str:
        return self.name

    def py(self) -> str:
        return self.name


@dataclass
class IntegerLiteral(Expression):
    value: int

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return str(self.value)

    def py(self) -> str:
        return str(self.value)


@dataclass
class RealLiteral(Expression):
    value: float

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        return str(self.value)

    def py(self) -> str:
        return str(self.value)


@dataclass
class StringLiteral(Expression):
    value: str

    def meta(self) -> str:
        return repr(self.value)

    def c(self) -> str:
        value = self.value.replace('"', r"\"")
        return f'from_cstring("{value}")'

    def py(self) -> str:
        return repr(self.value)


@dataclass
class BoolLiteral(Expression):
    value: bool

    def meta(self) -> str:
        return "TRUE" if self.value else "FALSE"

    def c(self) -> str:
        return "TRUE" if self.value else "FALSE"

    def py(self) -> str:
        return "True" if self.value else "False"


@dataclass
class ProgramStatement(Node):
    name: str
    segment: Segment

    def meta(self) -> str:
        return emit([f"PROGRAM {self.name}", indent(self.segment.meta(), 1)])

    def c(self) -> str:
        return emit(["int main()", "{", indent(self.segment.c(main=True), 1), "}"])

    def py(self) -> str:
        return emit([self.segment.py()])


# --------------------------


def indent(s: str, n: int) -> str:
    pad = "    " * n
    return "\n".join(pad + line for line in s.splitlines())


def emit(lines: list[str]) -> str:
    return "\n".join(lines)


def TYPE(v: "Type") -> str:
    if isinstance(v, Array):
        return v.c()
    if isinstance(v, StructureStatement):
        return v.c()
    if v in types_registry:
        return v
    type = {"INTEGER": "int", "REAL": "double", "BOOLEAN": "int", "STRING": "STR"}.get(v)
    if not type:
        raise ValueError(f"unknown type '{v}'")
    return type


def expand_type(name: Type, token: Token) -> str:
    if name in ("INTEGER", "REAL", "BOOLEAN", "STRING"):
        return name
    if isinstance(name, Array):
        return expand_type(name.type, token)
    custom = types_registry.get(name)
    if custom:
        return expand_type(custom, token)
    raise ValueError(f"unknown type '{name}' at {token}")


def is_number(name: str) -> bool:
    return name in ("INTEGER", "REAL")


def expression_stringer(v: Expression, format: list[str], callee: str = "OUTPUT") -> str:
    c = v.c()
    if isinstance(v, (IntegerLiteral, RealLiteral, BoolLiteral, StringLiteral)):
        literal_formats = {IntegerLiteral: "i", RealLiteral: "r", BoolLiteral: "b", StringLiteral: "A"}
        convert = literal_formats.get(type(v))
        if not convert:
            raise ValueError(f"unsupported literal type=[{type(v).__name__}] in {callee} at {v.token}")
        format.append(convert)
        return c
    if isinstance(v, Variable):
        name = v.name.split(".", 1)[0]
        variable_type = variables_registry.get(name)
        variable_formats = {"INTEGER": "i", "REAL": "r", "BOOLEAN": "b", "STRING": "A"}
        convert = variable_formats.get(variable_type)
        if not convert:
            raise ValueError(f"unsupported variable=[{name}] type=[{variable_type}] in {callee} at {v.token}")
        format.append(convert)
        return v.name
    if isinstance(v, ConcatenationOperation):
        format.append("A")
        return c
    if isinstance(v, FunctionCall):
        if v.name in functions_registry:
            function_type = functions_registry[v.name]
            function_type_formats = {"INTEGER": "i", "REAL": "r", "BOOLEAN": "b", "STRING": "A"}
            convert = function_type_formats.get(function_type)
            if convert:
                format.append(convert)
                return c
            raise ValueError(f"unsupported function=[{v.name}] return type=[{function_type}] in {callee} at {v.token}")
        raise ValueError(f"unsupported function=[{v.name}] invocation in {callee} at {v.token}")
    assert False, f"unsupported [{callee}] argument type=[{type(v).__name__}] at {v.token}"
