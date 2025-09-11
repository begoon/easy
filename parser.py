from typing import Tuple, Union, cast

from lexer import Token
from nodes import (
    Array,
    BeginStatement,
    BinaryOperation,
    BoolLiteral,
    CallStatement,
    ConcatenationOperation,
    DeclareStatement,
    EmptyStatement,
    ExitStatement,
    Expression,
    FieldStatement,
    ForStatement,
    FunctionCall,
    FunctionStatement,
    IfStatement,
    InputStatement,
    IntegerLiteral,
    Label,
    OutputStatement,
    ProcedureStatement,
    ProgramStatement,
    RealLiteral,
    RepeatStatement,
    RepentStatement,
    ReturnStatement,
    Segment,
    SelectStatement,
    SetStatement,
    Statement,
    Statements,
    StringLiteral,
    StructureStatement,
    Type,
    TypeIsStatement,
    UnaryOperation,
    Variable,
    expand_type,
    functions_registry,
    is_number,
    types_registry,
    variables_registry,
)
from yamler import yamlizer


class ParseError(Exception):
    def __init__(self, message: str, token: Token, source: str):
        super().__init__(message)
        self.message = message
        self.token = token
        self.source = source

    def __str__(self) -> str:
        error_line = self.source.splitlines()[self.token.line - 1]
        return (
            f"{self.message}\n"
            "at "
            f"{self.token.filename}:{self.token.line}:{self.token.col}\n"
            f"{error_line}\n{' ' * (self.token.col - 1)}^"
        )


class Parser:
    def __init__(self, tokens: list[Token], source: str):
        self.tokens = tokens
        self.i = 0
        self.source = source

    def error(self, message: str, token: Token) -> None:
        raise ParseError(message, token, self.source)

    def current(self) -> Token:
        return self.tokens[self.i]

    def eat(self, kind: Union[str, tuple[str, ...]]) -> Token:
        kinds = (kind,) if isinstance(kind, str) else kind
        token = self.current()
        if token.type in kinds or token.value in kinds:
            self.i += 1
            return token
        expected = "/".join(kinds)
        self.error(f"expected '{expected}', found '{token.value}'", token)

    def accept(self, kind: Union[str, tuple[str, ...]]) -> Token | None:
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
        token = self.current()

        self.eat("PROGRAM")
        name = self.eat("IDENT").value
        self.eat(":")
        segments = self.segment()
        self.eat("END")
        self.eat("PROGRAM")
        self.eat(name)
        self.eat(";")
        self.eat("EOF")
        return ProgramStatement(token, name, segments)

    def segment(self) -> Segment:
        token = self.current()
        types = self.types_section()
        variables = self.variables_section()
        subroutines = self.procedures_and_functions_section()
        statements = self.statements_section()
        return Segment(token, types, variables, subroutines, statements)

    def types_section(self) -> list[TypeIsStatement]:
        out: list[TypeIsStatement] = []
        while token := self.accept("TYPE"):
            name = self.eat("IDENT").value
            self.eat("IS")
            definition = self.parse_type()
            self.eat(";")
            out.append(TypeIsStatement(token, name, definition))
            types_registry[name] = definition
        return out

    def variables_section(self) -> list[DeclareStatement]:
        declarations: list[DeclareStatement] = []
        while declare_token := self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.parse_type()
                self.eat(";")
                declarations.append(DeclareStatement(declare_token, [name], type))
                variables_registry[name] = type
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
                declarations.append(DeclareStatement(declare_token, names, type))
                for name in names:
                    variables_registry[name] = type
                continue
            self.error(f"expected a variable and or '(', not '{token.value}' ", token)
        return declarations

    def parse_type(self) -> Type:
        token = self.current()
        if token.value in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
            self.eat(token.value)
            return token.value
        if token.value == "ARRAY":
            self.eat(token.value)

            self.eat("[")
            end = self.expression()
            start = None
            if self.accept(":"):
                start = end
                end = self.expression()
            self.eat("]")
            self.eat("OF")
            type = cast(Array, self.parse_type())
            return Array(token, type, start, end)
        if token.value == "STRUCTURE":
            self.eat(token.value)

            fields = []
            field_token = self.eat("FIELD")
            name = self.eat("IDENT").value
            self.eat("IS")
            type = self.parse_type()
            fields.append(FieldStatement(field_token, name, type))

            while self.accept(","):
                field_token = self.eat("FIELD")
                name = self.eat("IDENT").value
                self.eat("IS")
                type = self.parse_type()
                fields.append(FieldStatement(field_token, name, type))

            self.eat("END")
            self.eat("STRUCTURE")
            return StructureStatement(token, fields)

        return self.eat("IDENT").value

    def procedures_and_functions_section(self) -> list[ProcedureStatement | FunctionStatement]:
        subroutines: list[ProcedureStatement | FunctionStatement] = []
        while token := self.accept(("FUNCTION", "PROCEDURE")):
            name = self.eat("IDENT").value
            parameters: list[Tuple[str, Type]] = []
            if self.accept("("):
                if self.current().type != ")":
                    parameters = self.parameters()
                    for parameter in parameters:
                        variables_registry[parameter[0]] = parameter[1]
                self.eat(")")

            type = None
            if token.value == "FUNCTION":
                type = self.parse_type()
                functions_registry[name] = type

            self.eat(":")
            segment = self.segment()

            if token.value == "FUNCTION":
                has_return = any(isinstance(v, ReturnStatement) for v in segment.statements.statements)
                if not has_return:
                    segment.statements.statements.append(ReturnStatement(token, 0))

            self.eat("END")
            self.eat(token.value)
            self.eat(name)
            self.eat(";")

            if token.value == "PROCEDURE":
                assert type is None, f"PROCEDURE {name} cannot have a return {type=}"
                subroutines.append(ProcedureStatement(token, name, parameters, segment))
            else:
                subroutines.append(FunctionStatement(token, name, type, parameters, segment))
        return subroutines

    def parameters(self) -> list[tuple[str, Type]]:
        parameters: list[tuple[str, Type]] = []
        while True:
            name = self.eat("IDENT").value
            type = self.parse_type()
            parameters.append((name, type))
            if not self.accept(","):
                break
        return parameters

    def statements_section(self) -> Statements:
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

        statements_token = self.current()
        while self.current().value in STATEMENTS or is_label():
            if is_label():
                label_token = self.eat("IDENT")
                label = label_token.value
                self.eat(":")
                statements.append(Label(label_token, label))
            else:
                statements.append(self.statement())
        return Statements(statements_token, statements)

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
        return EmptyStatement(token)

    def arguments(self) -> list[Expression]:
        arguments = [self.expression()]
        while self.accept(","):
            arguments.append(self.expression())
        return arguments

    def if_statement(self) -> IfStatement:
        token = self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.segment()
        else_branch = self.accept("ELSE") and self.segment()
        self.eat("FI")
        self.eat(";")
        return IfStatement(token, cond, then_branch, else_branch)

    def for_statement(self) -> ForStatement:
        token = self.eat("FOR")
        variable = self.known_variable(("INTEGER", "REAL"))
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
        return ForStatement(token, variable, init, do, by, to, condition)

    def select_statement(self) -> SelectStatement:
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
        return SelectStatement(token, expr, cases)

    def return_statement(self) -> ReturnStatement:
        token = self.eat("RETURN")
        if self.accept(";"):
            return ReturnStatement(token)
        value = self.expression()
        self.eat(";")
        return ReturnStatement(token, value)

    def exit_statement(self) -> ExitStatement:
        token = self.eat("EXIT")
        self.eat(";")
        return ExitStatement(token)

    def known_variable(self, allowed_types: tuple[str] | None = None) -> Variable:
        variable = self.variable()

        compound = len(variable.parts) > 1
        if not compound and variables_registry.get(variable.name) is None:
            self.error(f"undefined variable [{variable}]", variable.token)

        name = variable.parts[0]
        type = variables_registry[name]
        variable.type = type

        if allowed_types and (type not in allowed_types):
            self.error(f"illegal [{variable}] {type=}, allowed types: {",".join(allowed_types)}", variable.token)
        return variable

    def input_statement(self) -> InputStatement:
        token = self.eat("INPUT")

        allowed_types = ("INTEGER", "REAL", "STRING")
        variables = [self.known_variable(allowed_types)]

        while self.accept(","):
            variables.append(self.known_variable(allowed_types))

        self.eat(";")
        return InputStatement(token, variables)

    def output_statement(self) -> OutputStatement:
        token = self.eat("OUTPUT")
        expressions = [self.expression()]
        while self.accept(","):
            expressions.append(self.expression())
        self.eat(";")
        return OutputStatement(token, expressions)

    def repeat_statement(self) -> Statements:
        token = self.eat("REPEAT")
        label = self.eat("IDENT").value
        self.eat(";")
        return RepeatStatement(token, label)

    def repent_statement(self) -> Statements:
        token = self.eat("REPENT")
        label = self.eat("IDENT").value
        self.eat(";")
        return RepentStatement(token, label)

    def begin_statement(self) -> Statements:
        token = self.eat("BEGIN")
        body = self.segment()
        self.eat("END")
        label = self.accept("IDENT")
        self.eat(";")
        return BeginStatement(token, body, label and label.value)

    def assignment_statement(self) -> SetStatement:
        token = self.eat("SET")
        variable = self.known_variable()
        self.eat(":=")
        targets = [variable]

        while True:
            current = self.current()
            if current.type != "IDENT" or self.peek().value != ":=":
                break
            variable = self.known_variable()
            self.eat(":=")
            targets.append(variable)

        expression = self.expression()
        for target in targets:
            self.check_variable_type(target, expression.type, token)

        self.eat(";")
        return SetStatement(token, targets, expression)

    def check_variable_type(self, variable: Variable, expected_type: Type, token: Token) -> Variable:
        if len(variable.parts) > 1:  # TODO: (!), check field and array types
            return variable

        expanded_expected_type = expand_type(expected_type, token)
        expanded_variable_type = expand_type(variables_registry.get(variable.name), token)

        if expanded_expected_type != expanded_variable_type:
            self.error(
                f"variable [{variable.name}] type mismatch:\n"
                + f"variable type: {expanded_variable_type}\n"
                + f"expected type: {expanded_expected_type}",
                token,
            )

        variable.type = expanded_variable_type

    def variable(self) -> Variable:
        token = self.eat("IDENT")
        name = token.value
        parts = [name]
        while True:
            if self.accept("."):
                part = self.eat("IDENT").value
                parts.append(part)
                name += "." + part
                continue
            if self.accept("["):
                index = self.expression()
                if index.type != "INTEGER":
                    self.error(f"expected INTEGER array index, not {index.type}", index.token)
                self.eat("]")
                parts.append(index)
                name += "[" + index.c() + "]"
                continue
            break
        return Variable(token, "UNKNOWN", name, parts)

    def call_statement(self) -> CallStatement:
        token = self.eat("CALL")
        name = self.eat("IDENT").value
        arguments = []
        if self.accept("("):
            arguments = self.arguments()
            self.eat(")")
        self.eat(";")
        return CallStatement(token, name, arguments)

    def expression(self) -> Expression:
        return self.expression_OR_XOR()

    def expression_OR_XOR(self) -> Expression:
        left = self.expression_AND()
        while operation := self.accept(("|", "XOR")):
            right = self.expression_AND()
            left = BinaryOperation(operation, "BOOLEAN", operation.value, left, right)
        return left

    def expression_AND(self) -> Expression:
        token = self.current()
        left = self.expression_NOT()
        while operation := self.accept("&"):
            right = self.expression_NOT()
            left = BinaryOperation(token, "BOOLEAN", operation.value, left, right)
        return left

    def expression_NOT(self) -> Expression:
        if token := self.accept("NOT"):
            return UnaryOperation(token, "BOOLEAN", token.value, self.expression_NOT())
        return self.expression_RELATION()

    def expression_RELATION(self) -> Expression:
        token = self.current()
        left = self.expression_CONCATENATION()
        while operation := self.accept(("<", ">", "=", "<=", ">=", "<>")):
            right = self.expression_CONCATENATION()

            left_type = expand_type(left.type, token)
            right_type = expand_type(right.type, token)

            allowed = left_type == right_type or (is_number(left_type) and is_number(right_type))
            assert allowed, f"cannot perform [{operation.value}] {left=} and {right=}"

            assert left_type in ("BOOLEAN", "INTEGER", "REAL"), f"cannot perform [{operation.value}] on {left}"
            left = BinaryOperation(token, "BOOLEAN", operation.value, left, right)
        return left

    def expression_CONCATENATION(self) -> Expression:
        parts = [self.expression_ADDING()]
        while self.accept("||"):
            token = self.current()
            part = self.expression_ADDING()
            parts.append(part)
        if len(parts) == 1:
            return parts[0]
        return ConcatenationOperation(token, "STRING", parts)

    def expression_ADDING(self) -> Expression:
        left = self.expression_MULTIPLYING()
        while operation := self.accept(("+", "-")):
            right = self.expression_MULTIPLYING()
            assert left.type == right.type, f"cannot {operation.value} {left=} and {right=}"
            assert left.type in ("INTEGER", "REAL"), f"cannot [{operation.value}] {left}"
            type = left.type
            left = BinaryOperation(operation, type, operation.value, left, right)
        return left

    def expression_MULTIPLYING(self) -> Expression:
        token = self.current()
        left = self.expression_FUNCTION_CALL()
        while operation := self.accept(("*", "/", "MOD")):
            right = self.expression_FUNCTION_CALL()
            if left.type != right.type:
                self.error(f"{left=} and {right=} must be of the same type to perform [{operation.value}]", token)
            if left.type not in ("INTEGER", "REAL"):
                self.error(f"operation only supported for INTEGER and REAL, not [{left.type}]", token)
            type = left.type
            left = BinaryOperation(token, type, operation.value, left, right)
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
            type = functions_registry.get(name)
            if type is None:
                self.error(f"undefined function [{name}]", token)
            return FunctionCall(token, type, name, arguments)
        return self.factor()

    def factor(self) -> Expression:
        token = self.current()
        if token.type == "INTEGER":
            token = self.eat("INTEGER")
            return IntegerLiteral(token, token.type, int(token.value))
        if token.type == "REAL":
            token = self.eat("REAL")
            return RealLiteral(token, token.type, float(token.value))
        if token.type == "STRING":
            self.i += 1
            return StringLiteral(token, token.type, token.value)
        if token.value in ("+", "-"):
            token = self.eat(("+", "-"))
            factor = self.factor()
            type = factor.type
            return UnaryOperation(token, type, token.value, factor)
        if token.value == "(":
            self.eat("(")
            expression = self.expression()
            self.eat(")")
            return expression
        if token.value in ("TRUE", "FALSE"):
            token = self.eat(("TRUE", "FALSE"))
            return BoolLiteral(token, "BOOLEAN", token.value == "TRUE")
        if token.type == "IDENT":
            variable = self.variable()
            name = variable.parts[0]
            if name not in variables_registry:
                self.error(f"undefined variable [{name}]\n{yamlizer(variables_registry)}", token)
            variable.type = variables_registry[name]
            if variable.type == "UNKNOWN":
                self.error(f"cannot use variable [{variable=}] with unknown type in expression", token)
            return variable
        self.error(
            f"expected an identifier or INTEGER/REAL/STRING literal or '+', '-', '(', 'TRUE/FALSE', "
            f"not {token.type}('{token.value}')",
            token,
        )
