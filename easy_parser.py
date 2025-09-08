from typing import Tuple, Union, cast

from easy_lexer import Token
from easy_nodes import (
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
    FunctionInvoke,
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
    types_registry,
    variables_registry,
)


class ParseError(Exception):
    def __init__(self, message: str, token: Token, source: str, filename: str):
        super().__init__(message)
        self.message = message
        self.token = token
        self.source = source
        self.filename = filename

    def __str__(self) -> str:
        error_line = self.source.splitlines()[self.token.line - 1]
        return (
            f"{self.message} at "
            f"{self.filename}:{self.token.line}:{self.token.col}\n"
            f"{error_line}\n{' ' * (self.token.col - 1)}^"
        )


class Parser:
    def __init__(self, tokens: list[Token], source: str, filename: str | None = None):
        self.tokens = tokens
        self.i = 0
        self.source = source
        self.filename = filename

    def error(self, message: str, token: Token) -> None:
        raise ParseError(message, token, self.source, self.filename)

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
        subroutines = self.procedures_and_functions_section()
        statements = self.statements_section()
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

        declarations: list[DeclareStatement] = []
        while self.accept("DECLARE"):
            token = self.current()
            if token.type == "IDENT":
                name = self.eat("IDENT").value
                type = self.type()
                self.eat(";")
                declarations.append(DeclareStatement([name], type))
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
                declarations.append(DeclareStatement(names, type))
                for name in names:
                    variables_registry[name] = type
                continue
            self.error(f"expected a variable and or '(', not '{token.value}' ", token)
        return declarations

    def type(self) -> Type:
        token = self.current()
        if token.value in ("INTEGER", "BOOLEAN", "REAL", "STRING"):
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
                self.eat("FIELD")
                name = self.eat("IDENT").value
                self.eat("IS")
                type = self.type()
                fields.append(FieldStatement(name, type))

            self.eat("END")
            self.eat("STRUCTURE")
            return StructureStatement(fields)
        return self.eat("IDENT").value

    def procedures_and_functions_section(self) -> list[ProcedureStatement | FunctionStatement]:
        subroutines: list[ProcedureStatement | FunctionStatement] = []
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
                has_return = any(isinstance(v, ReturnStatement) for v in segment.statements.statements)
                if not has_return:
                    segment.statements.statements.append(ReturnStatement(0))

            self.eat("END")
            self.eat(token.value)
            self.eat(name)
            self.eat(";")

            if token.value == "PROCEDURE":
                assert type is None, f"PROCEDURE {name} cannot have a return {type=}"
                subroutines.append(ProcedureStatement(name, parameters, segment))
            else:
                subroutines.append(FunctionStatement(name, type, parameters, segment))
        return subroutines

    def parameters(self) -> list[tuple[str, Type]]:
        parameters: list[tuple[str, Type]] = []
        while True:
            name = self.eat("IDENT").value
            type = self.type()
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
        arguments = [self.expression()]
        while self.accept(","):
            arguments.append(self.expression())
        return arguments

    def if_statement(self) -> IfStatement:
        self.eat("IF")
        cond = self.expression()
        self.eat("THEN")
        then_branch = self.segment()
        else_branch = self.accept("ELSE") and self.segment()
        self.eat("FI")
        self.eat(";")
        return IfStatement(cond, then_branch, else_branch)

    def for_statement(self) -> ForStatement:
        self.eat("FOR")
        variable = Variable(self.eat("IDENT").value)
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
        return ForStatement(variable, init, do, by, to, condition)

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
        indexes = {variable.name: variable.indexes}
        self.eat(":=")
        targets = [variable.name]

        while True:
            current = self.current()
            if current.type != "IDENT" or self.peek().value != ":=":
                break
            variable = self.variable_name()
            indexes[variable.name] = variable.indexes
            self.eat(":=")
            targets.append(variable.name)
        expr = self.expression()
        self.eat(";")
        return SetStatement(targets, expr, indexes)

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
        while operation := self.accept("&"):
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
        while self.accept("||"):
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
        if token.value == "+":
            self.i += 1
            return UnaryOperation("+", self.factor())
        if token.value == "-":
            self.i += 1
            return UnaryOperation("-", self.factor())
        if token.value == "(":
            self.i += 1
            expression = self.expression()
            self.eat(")")
            return expression
        if token.value in ("TRUE", "FALSE"):
            self.i += 1
            return BoolLiteral(token.value == "TRUE")
        self.error(
            f"expected an identifier or INTEGER/REAL/STRING literal or '+', '-', '(', 'TRUE/FALSE', "
            f"not {token.type}('{token.value}')",
            token,
        )
