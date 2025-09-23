import * as fs from "node:fs";
import * as path from "node:path";
import process from "node:process";

function indent(str: string, n: number): string {
    return str
        .split(/\r?\n/)
        .map((line) => "    ".repeat(n) + line)
        .join("\n");
}

function emit(lines: string[]): string {
    return lines.filter((line) => line.trim() !== "").join("\n");
}

function table(rows: string[][]): string {
    if (!rows.length) return "";
    const columnsNumber = Math.max(...rows.map((r) => r.length));
    const widths = Array(columnsNumber).fill(0);
    for (const row of rows) {
        row.forEach((cell, i) => (widths[i] = Math.max(widths[i], cell.length)));
    }
    const lines = rows.map((row) => row.map((cell, i) => cell.padEnd(widths[i], " ")).join("  "));
    return lines.join("\n") + "\n";
}

function flag(argv: string[], name: string): number | null {
    const i = argv.indexOf(name);
    return i >= 0 ? i : null;
}

function arg(argv: string[], name: string): string | null {
    const i = flag(argv, name);
    return i !== null && i + 1 < argv.length ? argv[i + 1] : null;
}

class InputText {
    filename: string;
    text: string;
    constructor(options: { text?: string; filename?: string }) {
        const { text = "", filename = null } = options;
        if (text) {
            this.text = text;
            this.filename = "";
        } else if (filename) {
            this.text = fs.readFileSync(filename, "utf-8");
            this.filename = filename;
        } else {
            this.text = "";
            this.filename = "";
        }
    }
}

type Flags = Record<string, string>;

class CompilerError extends Error {}
class LexerError extends CompilerError {}

class ParseError extends CompilerError {
    token: Token;
    constructor(message: string, token: Token) {
        super(message);
        this.token = token;
    }
    override toString(): string {
        const token = this.token;
        const lines = token.context.text.text.split(/\r?\n/);
        const errorLine = lines[token.line - 1] || "";
        return (
            `${this.message}\n` +
            `at ${token.context.text.filename}:${token.line}:${token.character}\n` +
            `${errorLine}\n` +
            `${" ".repeat(Math.max(0, token.character - 1))}^`
        );
    }
}

const KEYWORDS = new Set([
    "PROGRAM",
    //
    "BEGIN",
    "END",
    //
    "TYPE",
    "IS",
    "DECLARE",
    "FUNCTION",
    "PROCEDURE",
    "SET",
    //
    "CALL",
    "RETURN",
    "EXIT",
    //
    "INPUT",
    "OUTPUT",
    //
    "INTEGER",
    "REAL",
    "BOOLEAN",
    "STRING",
    //
    "IF",
    "THEN",
    "ELSE",
    "FI",
    //
    "FOR",
    "WHILE",
    "BY",
    "DO",
    //
    "SELECT",
    "CASE",
    "OTHERWISE",
    //
    "TRUE",
    "FALSE",
    //
    // "ARRAY",
    // "OF",
    // "STRUCTURE",
    // "FIELD",
    // "NOT",
    // "MOD",
    // "XOR",
]);

const SYMBOLS = new Set("+ - * / | & ( ) [ ] ; , . : := = <> < <= > >= ||".split(" ").filter(Boolean));

class Context {
    flags: Flags;
    text: InputText;
    tokens: Token[];
    common: string[];
    types: Record<string, Type>;
    functions: Record<string, BuiltinFunction | FUNCTION>;
    procedures: Record<string, PROCEDURE>;
    variables: Record<string, Variable>;
    r: number;

    constructor(init: {
        flags?: Flags;
        text: InputText;
        tokens?: Token[];
        common?: string[];
        types: Record<string, Type>;
        functions?: Record<string, BuiltinFunction | FUNCTION>;
        procedures?: Record<string, PROCEDURE>;
        variables?: Record<string, Variable>;
    }) {
        this.flags = init.flags ?? {};
        this.text = init.text;
        this.tokens = init.tokens ?? [];
        this.common = init.common ?? [];
        this.types = init.types;
        this.functions = init.functions ?? {};
        this.procedures = init.procedures ?? {};
        this.variables = init.variables ?? {};
        this.r = 1;
    }
}

// This function looks for a comment like /* $rN */, which represents a name
// of a temporary variable, which, in turn, represents a value that has been.
// If found, the comment is removed from the string, the of the string without
// the comment is pushed to `pre` (to be emitted before the current line),
// and the name of the temporary variable is returned.
function extract_value(c: string, pre: string[]): string {
    // looks for: /* $rN */ at the end (or anywhere) of the string
    const m = c.match(/\/\*\s*(\$r\d+)\s*\*\//);
    if (!m) return c;
    const ref = m[1];
    // remove the comment and any trailing whitespace/newlines from the string
    const str = c.replace(/\s*\/\*\s*\$r\d+\s*\*\/\s*$/, "");
    pre.push(str);
    return ref;
}

class Token {
    type: string;
    value: string;
    line: number;
    character: number;
    context: Context;
    constructor(type: string, value: string, line: number, character: number, context: Context) {
        this.type = type;
        this.value = value;
        this.line = line;
        this.character = character;
        this.context = context;
    }
    toString(): string {
        const v =
            this.type === "STRING" || this.type === "SYMBOL"
                ? `'${this.value.length < 20 ? this.value : this.value.slice(0, 17) + "..."}'`
                : this.value;
        const input = this.context.text;
        const typePart = this.type !== this.value ? `|${this.type}` : "";
        return `<${v}${typePart}|${input.filename}:${this.line}:${this.character}>`;
    }
}

function printer(root: any): string {
    const seen = new Set<any>();

    // function isPlainObject(obj: any) {
    //     if (obj === null || typeof obj !== "object") return false;
    //     const proto = Object.getPrototypeOf(obj);
    //     return proto === Object.prototype || proto === null;
    // }

    function walker(obj: any): any {
        if (obj == null) return "";

        if (typeof obj === "boolean" || typeof obj === "number" || typeof obj === "string") return obj;

        if (typeof obj === "object") {
            if (seen.has(obj)) throw new Error("cycle detected in AST: serialization would recurse forever");
            seen.add(obj);

            if (obj instanceof Token) {
                const token = obj as Token;
                const data = `<${token.value}|${token.type} ${token.context.text.filename}:${token.line}:${token.character}>`;
                seen.delete(obj);
                return data;
            }

            if (Array.isArray(obj)) {
                const sequence = obj.map((x) => walker(x));
                seen.delete(obj);
                return sequence;
            }

            const data: Record<string, unknown> = { node: obj.constructor?.name ?? "[node]" };
            for (const [name, value] of Object.entries(obj)) {
                if (name.startsWith("_")) continue;
                if (typeof value === "function") continue;
                data[name] = walker(value);
            }
            seen.delete(obj);
            return data;
        }

        return String(obj);
    }

    const data = walker(root);
    return JSON.stringify(data, null, 2);
}

abstract class Type {
    c(): string {
        throw new GenerateError(`c() not implemented for ${this.constructor.name}`);
    }
    zero(): string {
        throw new GenerateError(`zero() not implemented for ${this.constructor.name}`);
    }
    typedef(alias: string): string {
        throw new GenerateError(`typedef() not implemented for ${this.constructor.name}`);
    }
    format = () => "";
}

abstract class BuiltinType extends Type {}

class IntegerType extends BuiltinType {
    c = () => "int";
    zero = () => "0";
    typedef = (alias: string) => `typedef int ${alias}`;
    format = () => "i";
}

class RealType extends BuiltinType {
    c = () => "double";
    zero = () => "0.0";
    typedef = (alias: string) => `typedef double ${alias}`;
    format = () => "r";
}

class BooleanType extends BuiltinType {
    c = () => "int";
    zero = () => "0";
    typedef = (alias: string) => `typedef int ${alias}`;
    format = () => "b";
}

class StringType extends BuiltinType {
    initial?: string;
    constructor(initial?: string) {
        super();
        this.initial = initial;
    }
    zero = () => (this.initial ? `{ .data = "${this.initial}" }` : "{0}");
    c = () => "STR";
    typedef = (alias: string) => `typedef STR ${alias}`;
    format = () => "A";
}

class ArrayType extends Type {
    type: Type;
    hi: Expression;
    lo: Expression;
    dynamic: boolean;
    constructor(type: Type, hi: Expression, lo: Expression, dynamic = false) {
        super();
        this.type = type;
        this.hi = hi;
        this.lo = lo;
        this.dynamic = dynamic;
    }
    c(): string {
        const data = this.dynamic ? `*data` : `data[${this.sz()}]`;
        return emit(["struct", "{", indent(`${this.type.c()} ${data};`, 1), "}"]);
    }
    sz = () => `${this.hi.c()} - ${this.lo.c()} + 1`;
    zero(): string {
        if (!this.dynamic) return "{0}";
        return `{ .data = malloc(sizeof(${this.type.c()}) * (${this.sz()})) }`;
    }
    typedef = (alias: string) => `typedef ${this.c()} ${alias}`;
}

class StructField {
    token: Token;
    scope: string;
    name: string;
    type: Type;
    constructor(token: Token, scope: string, name: string, type: Type) {
        this.token = token;
        this.scope = scope;
        this.name = name;
        this.type = type;
    }
    c = () => `${this.type.c()} ${this.name}`;
}

class StructType extends Type {
    fields: StructField[];
    constructor(fields: StructField[]) {
        super();
        this.fields = fields;
    }
    c(): string {
        const v = ["struct", "{", ...this.fields.map((f) => indent(f.c() + ";", 1)), "}"];
        return emit(v);
    }
    init = () => "{0}";
    zero = () => "{0}";
    typedef = (alias: string): string => `typedef ${this.c()} ${alias}`;
}

class AliasType extends Type {
    reference_name: string;
    reference_type: Type;
    constructor(name: string, ref: Type) {
        super();
        this.reference_name = name;
        this.reference_type = ref;
    }
    c = () => this.reference_name;
    zero = () => this.reference_type.zero();
    typedef = (alias: string) => `typedef ${this.c()} ${alias}`;
}

abstract class Node {
    token: Token;
    scope: string;
    constructor(token: Token, scope: string) {
        this.token = token;
        this.scope = scope;
    }
    context = () => this.token.context;
    c(): string {
        throw new GenerateError(`c() not implemented for ${this.constructor.name} at ${this.token}`);
    }
    toString = () => printer(this);
}

abstract class Statement extends Node {}
abstract class Expression extends Node {
    type: Type;
    constructor(token: Token, scope: string, type: Type) {
        super(token, scope);
        this.type = type;
    }
}
abstract class Entity {
    token: Token;
    constructor(token: Token) {
        this.token = token;
    }
}
class TYPEIS extends Node {
    name: string;
    definition: Type;
    constructor(token: Token, scope: string, name: string, definition: Type) {
        super(token, scope);
        this.name = name;
        this.definition = definition;
    }
}

class DECLARE extends Node {
    names: string[];
    type: Type;
    constructor(token: Token, scope: string, names: string[], type: Type) {
        super(token, scope);
        this.names = names;
        this.type = type;
    }
    c(): string {
        return this.names.map((n) => `${this.type.c()} ${n} = ${this.type.zero()};`).join("\n");
    }
}

class Segment extends Node {
    types: TYPEIS[];
    variables: DECLARE[];
    subroutines: (FUNCTION | PROCEDURE)[];
    statements: Statement[];
    constructor(
        token: Token,
        scope: string,
        types: TYPEIS[],
        variables: DECLARE[],
        subs: (FUNCTION | PROCEDURE)[],
        statements: Statement[]
    ) {
        super(token, scope);
        this.types = types;
        this.variables = variables;
        this.subroutines = subs;
        this.statements = statements;
    }
    c(main = false): string {
        const v: string[] = [];
        if (this.variables) {
            for (const variable of this.variables) {
                const c = variable.c();
                if (main) this.context().common.push(c);
                else v.push(c);
            }
        }
        if (this.statements) {
            for (const statement of this.statements) v.push(statement.c());
        }
        return emit(v);
    }
}

class Argument {
    token: Token;
    name: string;
    type: Type;
    constructor(token: Token, name: string, type: Type) {
        this.token = token;
        this.name = name;
        this.type = type;
    }
    c(): string {
        return `${this.type.c()} ${this.name}`;
    }
}

class PROCEDURE extends Node {
    name: string;
    arguments: Argument[];
    segment: Segment;
    constructor(token: Token, scope: string, name: string, args: Argument[], seg: Segment) {
        super(token, scope);
        this.name = name;
        this.arguments = args;
        this.segment = seg;
    }
    c(): string {
        const args = this.arguments.map((v) => v.c()).join(", ");
        return emit([`void ${this.name}(${args})`, "{", indent(this.segment.c(), 1), "}"]);
    }
}

class FUNCTION extends Node {
    name: string;
    type: Type;
    arguments: Argument[];
    segment: Segment;
    constructor(token: Token, scope: string, name: string, type: Type, args: Argument[], seg: Segment) {
        super(token, scope);
        this.name = name;
        this.type = type;
        this.arguments = args;
        this.segment = seg;
    }
    c(): string {
        const func = this.context().functions[this.name] as BuiltinFunction | FUNCTION;
        const type = (func instanceof BuiltinFunction ? func.type : func.type).c();
        const args = this.arguments.map((a) => a.c()).join(", ");
        return emit([`${type} ${this.name}(${args})`, "{", indent(this.segment.c(), 1), "}"]);
    }
}

class Label extends Node {
    name: string;
    constructor(token: Token, scope: string, name: string) {
        super(token, scope);
        this.name = name;
    }
    c = () => `${this.name}:`;
}

class Variable {
    token: Token;
    name: string;
    type: Type;
    zeroValue?: string;
    constructor(token: Token, name: string, type: Type, zero?: string) {
        this.token = token;
        this.name = name;
        this.type = type;
        this.zeroValue = zero;
    }
    c(): string {
        return `${this.type.c()} ${this.name}`;
    }
    isConst(): boolean {
        return this.zeroValue !== undefined;
    }
    const(): string {
        if (!this.isConst()) throw new GenerateError(`variable '${this.name}' is not a constant at ${this.token}`);
        const z = (this.zeroValue ?? "").replace(/"/g, '\\"');
        return `${this.type.c()} ${this.name} = { .data = "${z}" }`;
    }
    s(scope: string): string[] {
        return [this.name, scope, this.type.constructor.name, String(this.token)];
    }
}

class BuiltinFunction {
    name: string;
    type: Type;
    constructor(name: string, type: Type) {
        this.name = name;
        this.type = type;
    }
}

class VariableField {
    token: Token;
    name: string;
    constructor(token: Token, name: string) {
        this.token = token;
        this.name = name;
    }
    c(): string {
        return `.${this.name}`;
    }
}
class VariableSubscript {
    token: Token;
    value: Expression;
    constructor(token: Token, value: Expression) {
        this.token = token;
        this.value = value;
    }
    index(): string {
        return this.value.c();
    }
}
class VariableReference extends Entity {
    scope: string;
    name: string;
    parts: (VariableSubscript | VariableField)[];
    constructor(token: Token, scope: string, name: string, parts: (VariableSubscript | VariableField)[], type?: Type) {
        super(token);
        this.scope = scope;
        this.name = name;
        this.parts = parts;
    }
    get type(): Type {
        const variable = discover_variable(this);
        const { type } = expand_variable_reference(variable, this, []);
        return type;
    }
    c(): string {
        const variable = discover_variable(this);
        const pre: string[] = [];
        const { reference } = expand_variable_reference(variable, this, pre);
        return reference;
    }
}

class BuiltinLiteral extends Expression {
    format(): string {
        throw new GenerateError(`format() not implemented for ${this.constructor.name} at ${this.token}`);
    }
}

class IntegerLiteral extends BuiltinLiteral {
    value: number;
    constructor(token: Token, scope: string, value: number) {
        super(token, scope, new IntegerType());
        this.value = value;
    }
    c = () => String(this.value);
    format = () => "i";
}

class RealLiteral extends BuiltinLiteral {
    value: number;
    constructor(token: Token, scope: string, value: number) {
        super(token, scope, new RealType());
        this.value = value;
    }
    c() {
        const v = String(this.value);
        return v.includes(".") || v.includes("e") ? v : v + ".0";
    }
    format = () => "r";
}

class StringLiteral extends BuiltinLiteral {
    value: string;
    constructor(token: Token, scope: string, value: string) {
        super(token, scope, new StringType());
        this.value = value;
    }
    c() {
        return `from_cstring("${this.value.replace(/"/g, '\\"')}")`;
    }
    literal() {
        return `"${this.value.replace(/"/g, '\\"')}"`;
    }
    format() {
        return "A";
    }
}

class BoolLiteral extends BuiltinLiteral {
    value: boolean;
    constructor(token: Token, scope: string, value: boolean) {
        super(token, scope, new BooleanType());
        this.value = value;
    }
    c() {
        return this.value ? "TRUE" : "FALSE";
    }
    format() {
        return "b";
    }
}

class SET extends Statement {
    target: VariableReference[];
    expression: Expression;
    constructor(token: Token, scope: string, target: VariableReference[], expr: Expression) {
        super(token, scope);
        this.target = target;
        this.expression = expr;
    }
    c(): string {
        const out: string[] = [];
        for (const target of this.target) {
            const variable = discover_variable(target);
            const pre: string[] = [];
            const { reference } = expand_variable_reference(variable, target, pre);
            const rhs = extract_value(this.expression.c(), pre);
            out.push(...pre, `${reference} = ${rhs};`);
        }
        return emit(out);
    }
}

class IF extends Statement {
    cond: Expression;
    then_branch: Segment;
    else_branch?: Segment | null;
    constructor(token: Token, scope: string, cond: Expression, then_: Segment, else_?: Segment | null) {
        super(token, scope);
        this.cond = cond;
        this.then_branch = then_;
        this.else_branch = else_ ?? null;
    }
    c(): string {
        const pre: string[] = [];
        let cond = extract_value(this.cond.c(), pre);
        if (cond.startsWith("(") && cond.endsWith(")")) cond = cond.slice(1, -1);

        const v = [...pre, `if (${cond})`, "{", indent(this.then_branch.c(), 1), "}"];
        if (this.else_branch) v.push("else", "{", indent(this.else_branch.c(), 1), "}");
        return emit(v);
    }
}

class FOR extends Statement {
    variable: VariableReference;
    init: Expression;
    segment: Segment;
    by?: Expression | null;
    to?: Expression | null;
    condition?: Expression | null;
    constructor(
        token: Token,
        scope: string,
        variable: VariableReference,
        init: Expression,
        doX: Segment,
        by?: Expression | null,
        to?: Expression | null,
        cond?: Expression | null
    ) {
        super(token, scope);
        this.variable = variable;
        this.init = init;
        this.segment = doX;
        this.by = by ?? null;
        this.to = to ?? null;
        this.condition = cond ?? null;
    }
    c(): string {
        const v: string[] = [];
        const preInit: string[] = [];
        const init = extract_value(this.init.c(), preInit);
        v.push(...preInit, `${this.variable.c()} = ${init};`);

        const inner: string[] = [];
        const condition = extract_value(this.format_condition(inner), inner);
        const step = extract_value(this.step(inner), inner);

        v.push(
            "while (1)",
            "{",
            indent(emit(inner), 1),
            indent(`if (!(${condition})) break;`, 1),
            indent(this.segment.c(), 1),
            indent(step, 1),
            "}"
        );
        return emit(v);
    }
    format_condition(pre: string[]): string {
        const parts: string[] = [];
        if (this.condition) parts.push(extract_value(this.condition.c(), pre));
        if (this.to) parts.push(`${this.variable.c()} <= ${extract_value(this.to.c(), pre)}`);
        return parts.join("");
    }

    step(pre: string[]): string {
        const by = this.by ? extract_value(this.by.c(), pre) : "1";
        return `${this.variable.c()} += ${by};`;
    }
}

class SELECT extends Statement {
    expr: Expression;
    cases: Array<[Expression | null, Segment]>;
    constructor(token: Token, scope: string, expression: Expression, cases: Array<[Expression | null, Segment]>) {
        super(token, scope);
        this.expr = expression;
        this.cases = cases;
    }
    c(): string {
        const v: string[] = [];
        const pre: string[] = [];
        for (let i = 0; i < this.cases.length; i++) {
            const [condExpr, body] = this.cases[i];
            if (condExpr) {
                const cond = extract_value(condExpr.c(), pre);
                v.push((i > 0 ? "else " : "") + `if (${cond})`);
            } else {
                v.push("else");
            }
            v.push("{", indent(body.c(), 1), "}");
        }
        return emit([...pre, ...v]);
    }
}

class INPUT extends Statement {
    variables: VariableReference[];
    constructor(token: Token, scope: string, variables: VariableReference[]) {
        super(token, scope);
        this.variables = variables;
    }
    c(): string {
        const pre: string[] = [];
        for (const vr of this.variables) {
            const variable = discover_variable(vr);
            const { type, reference } = expand_variable_reference(variable, vr, pre);
            if (type instanceof StringType) pre.push(`scanf("%s", ${reference}.data);`);
            else if (type instanceof IntegerType) pre.push(`scanf("%d", &${reference});`);
            else if (type instanceof RealType) pre.push(`scanf("%lf", &${reference});`);
            else throw new GenerateError(`unsupported variable '${variable}' type in INPUT at ${variable.token}`);
        }
        return emit(pre);
    }
}

class OUTPUT extends Statement {
    arguments: Expression[];
    constructor(token: Token, scope: string, args: Expression[]) {
        super(token, scope);
        this.arguments = args;
    }
    c(): string {
        const pre: string[] = [];
        const fmt: string[] = [];
        const params = this.arguments.map((a) => expression_stringer(a, fmt, "OUTPUT", pre)).join(", ");
        pre.push(`$output("${fmt.join("")}", ${params});`);
        return emit(pre);
    }
}

class REPEAT extends Statement {
    label: string;
    constructor(token: Token, scope: string, label: string) {
        super(token, scope);
        this.label = label;
    }
    c() {
        return `goto ${this.label};`;
    }
}
class REPENT extends Statement {
    label: string;
    constructor(token: Token, scope: string, label: string) {
        super(token, scope);
        this.label = label;
    }
    c() {
        return `goto ${this.label};`;
    }
}

class BEGIN extends Statement {
    body: Segment;
    label?: string | null;
    constructor(token: Token, scope: string, segment: Segment, label?: string | null) {
        super(token, scope);
        this.body = segment;
        this.label = label ?? null;
    }
    c(): string {
        const v = ["{", indent(this.body.c(), 1), "}"];
        if (this.label) v.push(this.label + ":");
        return emit(v);
    }
}

class CALL extends Statement {
    name: string;
    arguments: Expression[];
    constructor(token: Token, scope: string, name: string, args: Expression[]) {
        super(token, scope);
        this.name = name;
        this.arguments = args;
    }
    c(): string {
        const pre: string[] = [];
        const args = this.arguments.map((a) => extract_value(a.c(), pre)).join(", ");
        pre.push(`${this.name}(${args});`);
        return emit(pre);
    }
}

class RETURN extends Statement {
    value?: Expression | null;
    constructor(token: Token, scope: string, value?: Expression | null) {
        super(token, scope);
        this.value = value ?? null;
    }
    c(): string {
        if (!this.value) return "return;";
        const pre: string[] = [];
        const v = extract_value(this.value.c(), pre);
        pre.push(`return ${v};`);
        return emit(pre);
    }
}

class EXIT extends Statement {
    c() {
        return "exit(0);";
    }
}

class EMPTY extends Statement {
    c() {
        return "while (0);";
    }
}

const OPERATIONS: Record<string, string> = { "|": "||", "&": "&&", "=": "==", "<>": "!=", MOD: "%", XOR: "^" };

class FunctionCall extends Expression {
    name: string;
    arguments: Expression[];
    constructor(token: Token, scope: string, type: Type, name: string, args: Expression[]) {
        super(token, scope, type);
        this.name = name;
        this.arguments = args;
    }
    c(): string {
        const ctx = this.context();
        const r = `$r${ctx.r++}`;
        const pre: string[] = [];
        const args = this.arguments.map((a) => extract_value(a.c(), pre)).join(", ");
        pre.push(`const auto ${r} = ${this.name}(${args}); /* ${r} */`);
        return emit(pre);
    }
}

class BinaryOperation extends Expression {
    operation: string;
    left: Expression;
    right: Expression;
    constructor(token: Token, scope: string, type: Type, operation: string, left: Expression, right: Expression) {
        super(token, scope, type);
        this.operation = operation;
        this.left = left;
        this.right = right;
    }
    c(): string {
        const operation = OPERATIONS[this.operation] ?? this.operation;
        const is_string = string_compare(this.left, this.right, operation);
        if (is_string) return is_string;

        const ctx = this.context();
        const r = `$r${ctx.r++}`;

        const pre: string[] = [];
        const L = extract_value(this.left.c(), pre);
        const R = extract_value(this.right.c(), pre);
        pre.push(`const auto ${r} = (${L} ${operation} ${R}); /* ${r} */`);
        return emit(pre);
    }
}
class UnaryOperation extends Expression {
    operation: string;
    expr: Expression;
    constructor(token: Token, scope: string, operation: string, expr: Expression) {
        super(token, scope, expr.type);
        this.operation = operation;
        this.expr = expr;
    }
    c(): string {
        const ctx = this.context();
        const r = `$r${ctx.r++}`;
        const op = this.operation === "NOT" ? "!" : this.operation;
        const pre: string[] = [];
        const v = extract_value(this.expr.c(), pre);
        pre.push(`const auto ${r} = (${op}${v}); /* ${r} */`);
        return emit(pre);
    }
}

function string_compare(left: Expression, right: Expression, operation: string): string | null {
    if (operation !== "==" && operation !== "!=") return null;

    function is_string_type(e: Expression): [boolean, string | null] {
        if (!(e instanceof VariableReference)) return [false, null];
        const variable = discover_variable(e);
        const { type, reference } = expand_variable_reference(variable, e, []);
        return [type instanceof StringType, reference];
    }

    const [left_string, left_reference] = is_string_type(left);
    const [right_string, right_reference] = is_string_type(right);

    if (left_string || right_string) {
        const cmp = operation === "!=" ? "!=" : "==";
        return `strcmp(${left_reference}.data, ${right_reference}.data) ${cmp} 0`;
    }
    return null;
}

class Lexer {
    context: Context;
    input: InputText;
    text: string;
    position = 0;
    line = 1;
    character = 1;
    n: number;

    constructor(context: Context) {
        this.context = context;
        this.input = context.text;
        this.text = this.input.text;
        this.n = this.text.length;
    }

    peek(k = 1): string {
        const j = this.position + k;
        return j < this.n ? this.text[j] : "";
    }
    current = () => (this.position < this.n ? this.text[this.position] : "");

    advance(k = 1) {
        for (let i = 0; i < k; i++) {
            if (this.position < this.n) {
                const c = this.text[this.position++];
                if (c === "\n") {
                    this.line += 1;
                    this.character = 1;
                } else this.character += 1;
            }
        }
    }

    skip_whitespace_and_comments() {
        while (true) {
            while (this.current() && /\s/.test(this.current())) this.advance();

            const comments = (): boolean => {
                if (this.current() === "/" && this.peek() === "*") {
                    this.advance(2);
                    while (!(this.current() === "*" && this.peek() === "/")) {
                        if (this.position >= this.n) {
                            const loc = `${this.input.filename}:${this.line}:${this.character}`;
                            throw new LexerError(`unterminated /* */ comment at ${loc}`);
                        }
                        if (comments()) continue;
                        this.advance();
                    }
                    this.advance(2);
                    return true;
                }
                return false;
            };

            if (comments()) continue;

            if (this.current() === "/" && this.peek() === "/") {
                this.advance(2);
                while (this.current() && this.current() !== "\n") this.advance();
                if (this.current() === "\n") this.advance();
                continue;
            }
            break;
        }
    }

    number(): Token {
        const { line, character } = this;
        let value = "";
        while (/\d/.test(this.current())) {
            value += this.current();
            this.advance();
        }
        if (this.current() === "." || this.current().toLowerCase() === "e") {
            value += this.current();
            this.advance();
            while (/\d/.test(this.current()) || /[+\-eE]/.test(this.current())) {
                value += this.current();
                this.advance();
            }
            return new Token("REAL", value, line, character, this.context);
        }
        return new Token("INTEGER", value, line, character, this.context);
    }

    ident_or_keyword(): Token {
        const { line, character } = this;
        const c = this.current();
        let value = "";
        if (/[A-Za-z_]/.test(c)) {
            value += c;
            this.advance();
            while (/[A-Za-z0-9_]/.test(this.current())) {
                value += this.current();
                this.advance();
            }
        }
        if (KEYWORDS.has(value)) return new Token("KEYWORD", value, line, character, this.context);
        return new Token("IDENT", value, line, character, this.context);
    }

    string(): Token {
        const { line, character } = this;
        const quote = this.current();
        this.advance();
        let value = "";
        while (true) {
            const c = this.current();
            if (!c) {
                const location = `${this.input.filename}:${line}:${character}`;
                throw new LexerError(`unterminated string at ${location}`);
            }
            if (c === quote) {
                this.advance();
                if (this.current() === quote) {
                    value += quote;
                    this.advance();
                    continue;
                }
                break;
            }
            value += c;
            this.advance();
        }
        return new Token("STRING", value, line, character, this.context);
    }

    symbol(): Token {
        const { line, character } = this;
        const two = this.current() + this.peek();
        if (SYMBOLS.has(two)) {
            this.advance(2);
            return new Token("SYMBOL", two, line, character, this.context);
        }
        const one = this.current();
        if (SYMBOLS.has(one)) {
            this.advance();
            return new Token("SYMBOL", one, line, character, this.context);
        }
        const location = `${this.input.filename}:${line}:${character}`;
        throw new LexerError(`unknown symbol '${one}' at ${location}`);
    }

    tokenize(): Token[] {
        const tokens: Token[] = [];
        while (true) {
            this.skip_whitespace_and_comments();
            if (this.position >= this.n) break;
            const c = this.current();
            if (/\d/.test(c)) tokens.push(this.number());
            else if (/[A-Za-z_]/.test(c)) tokens.push(this.ident_or_keyword());
            else if (c === '"') tokens.push(this.string());
            else tokens.push(this.symbol());
        }
        tokens.push(new Token("EOF", "", this.line, this.character, this.context));
        return tokens;
    }
}

class GenerateError extends Error {}

function is_number_name(name: string): boolean {
    return name === "INTEGER" || name === "REAL";
}

function expression_stringer(e: Expression, fmt: string[], callee: string, pre: string[]): string {
    const c = extract_value(e.c(), pre);

    if (e instanceof BuiltinLiteral) {
        fmt.push(e.format());
        return c;
    }

    if (e instanceof VariableReference) {
        const variable = discover_variable(e);
        let { type, reference } = expand_variable_reference(variable, e, pre);
        if (type instanceof AliasType) type = type.reference_type;
        if (!(type instanceof BuiltinType))
            throw new GenerateError(
                `unsupported ${callee} variable argument '${e}' of type '${type.constructor.name}' at ${e.token}`
            );

        fmt.push(type.format());
        return reference;
    }

    if (e instanceof ConcatenationOperation) {
        fmt.push("A");
        return c;
    }

    if (e instanceof FunctionCall) {
        const func = e.context().functions[e.name];
        const type = func instanceof BuiltinFunction ? func.type : func.type;
        if (!(type instanceof BuiltinType))
            throw new GenerateError(
                `unsupported ${callee} function argument ${e} of type ${type.constructor.name} at ${e.token}`
            );

        fmt.push(type.format());
        return c;
    }

    throw new GenerateError(`unsupported ${callee} argument '${e}' at ${e.token}`);
}

function expand_variable_reference(variable: Variable, variable_reference: VariableReference, pre: string[]) {
    let type: Type = variable.type;
    let reference = variable.name;

    for (const part of variable_reference.parts) {
        if (part instanceof VariableSubscript) {
            if (type instanceof AliasType) type = type.reference_type;

            if (!(type instanceof ArrayType))
                throw new GenerateError(
                    `expect ArrayType in reference type of subscript, not ${type.constructor.name} at ${part.token}`
                );

            const { filename } = part.token.context.text;
            const { line, character } = part.token;

            enlist_variable(new Variable(part.token, "$F", new StringType(), filename), "");

            const lo = extract_value(type.lo.c(), pre);
            const hi = extract_value(type.hi.c(), pre);
            const index = extract_value(part.index(), pre);

            pre.push(`$index(${index}, ${lo}, ${hi}, &$F, ${line}, ${character});`);

            const indexExpr = `(${index}) - (${lo})`;
            type = type.type;
            reference += `.data[${indexExpr}]`;
        } else if (part instanceof VariableField) {
            if (type instanceof AliasType) type = type.reference_type;

            if (!(type instanceof StructType))
                throw new GenerateError(
                    `expect StructType in reference type of field, not ${type.constructor.name} at ${part.token}`
                );

            const field = type.fields.find((f) => f.name === part.name);
            if (!field) throw new GenerateError(`field '${part.name}' not found in ${type} at ${part.token}`);

            type = field.type;
            reference += part.c();
        } else {
            throw new GenerateError(`unexpected variable reference part '${part}' at ${variable.token}`);
        }
    }
    return { type, reference };
}

function discover_variable(v: VariableReference): Variable {
    const { context } = v.token;

    const parts = v.scope.split(".");

    let variable: Variable | undefined;
    while (parts.length) {
        const fqn = parts.join(".") + "|" + v.name;
        variable = context.variables[fqn];
        if (variable) break;
        parts.pop();
    }
    if (!variable) throw new GenerateError(`undefined variable '${v.name}' in scope '${v.scope}' at ${v.token}`);
    return variable;
}

function enlist_type(name: string, type: Type, context: Context) {
    if (context.types[name]) throw new GenerateError(`type '${name}' already defined at ${name}`);
    context.types[name] = type;
}

function enlist_variable(variable: Variable, scope: string) {
    const scopedName = scope + "|" + variable.name;
    variable.token.context.variables[scopedName] = variable;
}

class Parser {
    context: Context;
    tokens: Token[];
    i = 0;
    scopes: string[] = [];

    constructor(context: Context) {
        this.context = context;
        this.tokens = context.tokens;
    }

    scope = () => (this.scopes.length ? this.scopes.join(".") : "@");

    enter_scope = (name: string) => this.scopes.push(name);
    leave_scope = () => this.scopes.pop();

    current = () => this.tokens[this.i];

    peek(): Token {
        const current = this.current();
        return this.i + 1 < this.tokens.length
            ? this.tokens[this.i + 1]
            : new Token("EOF", "", current.line, current.character, this.context);
    }

    eat(kind: string | string[]): Token {
        const kinds = Array.isArray(kind) ? kind : [kind];
        const token = this.current();
        if (kinds.includes(token.type) || kinds.includes(token.value)) {
            this.i += 1;
            return token;
        }
        const expected = kinds.join("/");
        throw new ParseError(`expected '${expected}', found '${token.value}'`, token);
    }

    accept(kind: string | string[]): Token | null {
        const kinds = Array.isArray(kind) ? kind : [kind];
        const token = this.current();
        if (kinds.includes(token.type) || kinds.includes(token.value)) {
            this.i += 1;
            return token;
        }
        return null;
    }

    program(): PROGRAM {
        const token = this.current();
        this.eat("PROGRAM");
        const name = this.eat("IDENT").value;
        this.eat(":");
        this.enter_scope(`PROGRAM:${name}`);
        const segments = this.segment();
        this.leave_scope();
        this.eat("END");
        this.eat("PROGRAM");
        this.eat(name);
        this.eat(";");
        this.eat("EOF");
        return new PROGRAM(token, this.scope(), name, segments);
    }

    segment(): Segment {
        const token = this.current();
        const types = this.types_section();
        const variables = this.variables_section();
        const subroutines = this.procedures_and_functions_section();
        const statements = this.statements_section();
        return new Segment(token, this.scope(), types, variables, subroutines, statements);
    }

    types_section(): TYPEIS[] {
        const out: TYPEIS[] = [];
        let token: Token | null;
        while ((token = this.accept("TYPE"))) {
            const name = this.eat("IDENT").value;
            this.eat("IS");
            const definition = this.parse_type();
            this.eat(";");
            out.push(new TYPEIS(token, this.scope(), name, definition));
            enlist_type(name, definition, this.context);
        }
        return out;
    }

    variables_section(): DECLARE[] {
        const declarations: DECLARE[] = [];
        while (true) {
            const declareToken = this.accept("DECLARE");
            if (!declareToken) break;

            const token = this.current();
            if (token.type === "IDENT") {
                const name = this.eat("IDENT").value;
                const type = this.parse_type();
                this.eat(";");
                declarations.push(new DECLARE(declareToken, this.scope(), [name], type));
                enlist_variable(new Variable(token, name, type), this.scope());
                continue;
            }

            if (token.value === "(") {
                this.eat("(");
                const names: string[] = [];
                while (this.current().value !== ")") {
                    this.accept(",");
                    names.push(this.eat("IDENT").value);
                }
                this.eat(")");
                const type = this.parse_type();
                this.eat(";");
                declarations.push(new DECLARE(declareToken, this.scope(), names, type));
                for (const name of names) enlist_variable(new Variable(token, name, type), this.scope());
                continue;
            }
            throw new ParseError("expected a variable or '(' variable')'", token);
        }
        return declarations;
    }

    parse_type(): Type {
        const token = this.current();
        if (["INTEGER", "BOOLEAN", "REAL", "STRING"].includes(token.value)) {
            this.eat(token.value);
            return this.context.types[token.value];
        }
        if (token.value === "ARRAY") {
            this.eat("ARRAY");
            this.eat("[");
            let end = this.expression();
            let start: Expression = new IntegerLiteral(token, this.scope(), 0);
            if (this.accept(":")) {
                start = end;
                end = this.expression();
            }
            this.eat("]");
            this.eat("OF");
            const elementType = this.parse_type();
            const dynamic = !(end instanceof IntegerLiteral && start instanceof IntegerLiteral);
            return new ArrayType(elementType, end, start, dynamic);
        }
        if (token.value === "STRUCTURE") {
            this.eat("STRUCTURE");
            const fields: StructField[] = [];
            const fieldToken = this.eat("FIELD");
            let name = this.eat("IDENT").value;
            this.eat("IS");
            let fieldType = this.parse_type();
            fields.push(new StructField(fieldToken, this.scope(), name, fieldType));
            while (this.accept(",")) {
                const ft = this.eat("FIELD");
                name = this.eat("IDENT").value;
                this.eat("IS");
                fieldType = this.parse_type();
                fields.push(new StructField(ft, this.scope(), name, fieldType));
            }
            this.eat("END");
            this.eat("STRUCTURE");
            return new StructType(fields);
        }
        const typeAliasToken = this.eat("IDENT");
        const aliasName = typeAliasToken.value;
        const aliasType = this.context.types[aliasName];
        if (!aliasType) throw new ParseError(`unknown type alias '${aliasName}'`, typeAliasToken);
        return new AliasType(aliasName, aliasType);
    }

    procedures_and_functions_section(): (PROCEDURE | FUNCTION)[] {
        const subroutines: (PROCEDURE | FUNCTION)[] = [];
        while (true) {
            const token = this.accept(["FUNCTION", "PROCEDURE"]);
            if (!token) break;
            const name = this.eat("IDENT").value;
            this.enter_scope(token.value + ":" + name);
            let parameters: Variable[] = [];
            if (this.accept("(")) {
                if (this.current().value !== ")") {
                    parameters = this.parameters();
                    for (const p of parameters) enlist_variable(p, this.scope());
                }
                this.eat(")");
            }
            let type: Type | null = null;
            if (token.value === "FUNCTION") type = this.parse_type();
            this.eat(":");
            const segment = this.segment();
            this.eat("END");
            this.eat(token.value);
            this.eat(name);
            this.eat(";");
            if (token.value === "PROCEDURE") {
                const args = parameters.map((v) => new Argument(v.token, v.name, v.type));
                const procedure = new PROCEDURE(token, this.scope(), name, args, segment);
                this.context.procedures[name] = procedure;
                subroutines.push(procedure);
            } else {
                const args = parameters.map((v) => new Argument(v.token, v.name, v.type));
                const func = new FUNCTION(token, this.scope(), name, type!, args, segment);
                this.context.functions[name] = func;
                subroutines.push(func);
            }
            this.leave_scope();
        }
        return subroutines;
    }

    parameters(): Variable[] {
        const parameters: Variable[] = [];
        while (true) {
            const token = this.eat("IDENT");
            const name = token.value;
            const type = this.parse_type();
            const variable = new Variable(token, name, type);
            parameters.push(variable);
            enlist_variable(variable, this.scope());
            if (!this.accept(",")) break;
        }
        return parameters;
    }

    statements_section(): Statement[] {
        const statements: Statement[] = [];
        const STATEMENTS = new Set([
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
        ]);
        const is_label = () => {
            const current = this.current();
            return current.type === "IDENT" && current.value !== "OTHERWISE" && this.peek().value === ":";
        };
        while (STATEMENTS.has(this.current().value) || is_label()) {
            if (is_label()) {
                const token = this.eat("IDENT");
                const label = token.value;
                this.eat(":");
                statements.push(new Label(token, this.scope(), label));
            } else {
                statements.push(this.statement());
            }
        }
        return statements;
    }

    statement(): Statement {
        const token = this.current();
        switch (token.value) {
            case "SET":
                return this.assignment_statement();
            case "CALL":
                return this.call_statement();
            case "IF":
                return this.if_statement();
            case "FOR":
                return this.for_statement();
            case "SELECT":
                return this.select_statement();
            case "RETURN":
                return this.return_statement();
            case "EXIT":
                return this.exit_statement();
            case "INPUT":
                return this.input_statement();
            case "OUTPUT":
                return this.output_statement();
            case "REPEAT":
                return this.repeat_statement();
            case "REPENT":
                return this.repent_statement();
            case "BEGIN":
                return this.begin_statement();
            case ";":
                this.eat(";");
                return new EMPTY(token, this.scope());
            default:
                throw new ParseError("unexpected statement", token);
        }
    }

    arguments(): Expression[] {
        const args = [this.expression()];
        while (this.accept(",")) args.push(this.expression());
        return args;
    }

    if_statement(): IF {
        const token = this.eat("IF");
        const condition = this.expression();
        this.eat("THEN");
        const then_ = this.segment();
        const else_ = this.accept("ELSE") ? this.segment() : null;
        this.eat("FI");
        this.eat(";");
        return new IF(token, this.scope(), condition, then_, else_);
    }

    for_statement(): FOR {
        const token = this.eat("FOR");
        const variable = this.variable_reference();
        this.eat(":=");
        const init = this.expression();
        const by = this.accept("BY") ? this.expression() : null;
        const to = this.accept("TO") ? this.expression() : null;
        const cond = this.accept("WHILE") ? this.expression() : null;
        this.eat("DO");
        const segment = this.segment();
        this.eat("END");
        this.eat("FOR");
        this.eat(";");
        return new FOR(token, this.scope(), variable, init, segment, by, to, cond);
    }

    select_statement(): SELECT {
        const token = this.eat("SELECT");
        const expression = this.expression();
        this.eat("OF");
        const cases: Array<[Expression | null, Segment]> = [];
        while (this.accept("CASE")) {
            this.eat("(");
            const cond = this.expression();
            this.eat(")");
            this.eat(":");
            const body = this.segment();
            cases.push([cond, body]);
        }
        if (this.accept("OTHERWISE")) {
            this.eat(":");
            const segment = this.segment();
            cases.push([null, segment]);
        }
        this.eat("END");
        this.eat("SELECT");
        this.eat(";");
        return new SELECT(token, this.scope(), expression, cases);
    }

    return_statement(): RETURN {
        const token = this.eat("RETURN");
        if (this.accept(";")) return new RETURN(token, this.scope(), null);
        const value = this.expression();
        this.eat(";");
        return new RETURN(token, this.scope(), value);
    }
    exit_statement(): EXIT {
        const token = this.eat("EXIT");
        this.eat(";");
        return new EXIT(token, this.scope());
    }

    input_statement(): INPUT {
        const token = this.eat("INPUT");
        const variable_references = [this.variable_reference()];
        while (this.accept(",")) variable_references.push(this.variable_reference());
        this.eat(";");
        return new INPUT(token, this.scope(), variable_references);
    }

    output_statement(): OUTPUT {
        const token = this.eat("OUTPUT");
        const expressions = [this.expression()];
        while (this.accept(",")) expressions.push(this.expression());
        this.eat(";");
        return new OUTPUT(token, this.scope(), expressions);
    }

    repeat_statement(): REPEAT {
        const token = this.eat("REPEAT");
        const label = this.eat("IDENT").value;
        this.eat(";");
        return new REPEAT(token, this.scope(), label);
    }

    repent_statement(): REPENT {
        const token = this.eat("REPENT");
        const label = this.eat("IDENT").value;
        this.eat(";");
        return new REPENT(token, this.scope(), label);
    }

    begin_statement(): BEGIN {
        const token = this.eat("BEGIN");
        const segment = this.segment();
        this.eat("END");
        const label = this.accept("IDENT");
        this.eat(";");
        return new BEGIN(token, this.scope(), segment, label ? label.value : null);
    }

    assignment_statement(): SET {
        const token = this.eat("SET");
        const variable = this.variable_reference();
        this.eat(":=");
        const targets = [variable];
        while (true) {
            const position = this.i;
            if (this.current().type !== "IDENT") break;
            const variable = this.variable_reference();
            if (this.current().value !== ":=") {
                this.i = position;
                break;
            }
            this.eat(":=");
            targets.push(variable);
        }
        const expression = this.expression();
        this.eat(";");
        return new SET(token, this.scope(), targets, expression);
    }

    variable_reference(): VariableReference {
        const token = this.eat("IDENT");
        const name = token.value;
        const parts: (VariableField | VariableSubscript)[] = [];
        while (true) {
            if (this.accept(".")) {
                const ft = this.eat("IDENT");
                parts.push(new VariableField(ft, ft.value));
                continue;
            }
            if (this.accept("[")) {
                const st = this.current();
                const sub = this.expression();
                parts.push(new VariableSubscript(st, sub));
                this.eat("]");
                continue;
            }
            break;
        }
        return new VariableReference(token, this.scope(), name, parts);
    }

    call_statement(): CALL {
        const token = this.eat("CALL");
        const name = this.eat("IDENT").value;
        let args: Expression[] = [];
        if (this.accept("(")) {
            if (this.current().value !== ")") args = this.arguments();
            this.eat(")");
        }
        this.eat(";");
        return new CALL(token, this.scope(), name, args);
    }

    expression(): Expression {
        return this.expression_OR_XOR();
    }

    expression_OR_XOR(): Expression {
        let left = this.expression_AND();
        while (true) {
            const op = this.accept(["|", "XOR"]);
            if (!op) break;
            const right = this.expression_AND();
            left = new BinaryOperation(op, this.scope(), new BooleanType(), op.value, left, right);
        }
        return left;
    }

    expression_AND(): Expression {
        const token = this.current();
        let left = this.expression_NOT();
        while (true) {
            const operation = this.accept("&");
            if (!operation) break;
            const right = this.expression_NOT();
            left = new BinaryOperation(token, this.scope(), new BooleanType(), operation.value, left, right);
        }
        return left;
    }

    expression_NOT(): Expression {
        const token = this.accept("NOT");
        if (token) return new UnaryOperation(token, this.scope(), token.value, this.expression_NOT());
        return this.expression_RELATION();
    }

    expression_RELATION(): Expression {
        const token = this.current();
        let left = this.expression_CONCATENATION();
        while (true) {
            const operation = this.accept(["<", ">", "=", "<=", ">=", "<>"]);
            if (!operation) break;
            const right = this.expression_CONCATENATION();
            left = new BinaryOperation(token, this.scope(), new BooleanType(), operation.value, left, right);
        }
        return left;
    }

    expression_CONCATENATION(): Expression {
        const parts: Expression[] = [this.expression_ADDING()];
        let token: Token = this.current();
        while (this.accept("||")) {
            token = this.current();
            parts.push(this.expression_ADDING());
        }
        if (parts.length === 1) return parts[0];
        return new ConcatenationOperation(token, this.scope(), new StringType(), parts);
    }

    expression_ADDING(): Expression {
        let left = this.expression_MULTIPLYING();
        while (true) {
            const operation = this.accept(["+", "-"]);
            if (!operation) break;
            const right = this.expression_MULTIPLYING();
            left = new BinaryOperation(operation, this.scope(), left.type, operation.value, left, right);
        }
        return left;
    }

    expression_MULTIPLYING(): Expression {
        const token = this.current();
        let left = this.expression_FUNCTION_CALL();
        while (true) {
            const operation = this.accept(["*", "/", "MOD"]);
            if (!operation) break;
            const right = this.expression_FUNCTION_CALL();
            left = new BinaryOperation(token, this.scope(), left.type, operation.value, left, right);
        }
        return left;
    }

    expression_FUNCTION_CALL(): Expression | FunctionCall {
        const token = this.current();
        if (token.type === "IDENT" && this.peek().value === "(") {
            const name = this.eat("IDENT").value;
            this.eat("(");
            const args = this.current().value !== ")" ? this.arguments() : [];
            this.eat(")");
            const type = this.context.functions[name];
            if (!type) throw new ParseError(`undefined function '${name}'`, token);
            const rettype = type instanceof BuiltinFunction ? type.type : type.type;
            return new FunctionCall(token, this.scope(), rettype, name, args);
        }
        return this.factor();
    }

    factor(): Expression {
        const token = this.current();
        if (token.type === "INTEGER") {
            const token = this.eat("INTEGER");
            return new IntegerLiteral(token, this.scope(), parseInt(token.value, 10));
        }
        if (token.type === "REAL") {
            const token = this.eat("REAL");
            return new RealLiteral(token, this.scope(), parseFloat(token.value));
        }
        if (token.type === "STRING") {
            const token = this.eat("STRING");
            const context = this.context;
            const existing = Object.values(context.variables).find((v) => v.isConst() && v.zeroValue === token.value);
            if (existing) return new VariableReference(token, "", existing.name, []);

            const const_i = Object.values(context.variables).filter((v) => v.isConst()).length;
            const name = `$${const_i}`;
            const variable = new Variable(token, name, new StringType(), token.value);
            enlist_variable(variable, "");
            return new VariableReference(token, "", name, []);
        }
        if (token.value === "+" || token.value === "-") {
            const unaryToken = this.eat(token.value);
            const factor = this.factor();
            return new UnaryOperation(unaryToken, this.scope(), unaryToken.value, factor);
        }
        if (token.value === "(") {
            this.eat("(");
            const e = this.expression();
            this.eat(")");
            return e;
        }
        if (token.value === "TRUE" || token.value === "FALSE") {
            const valueToken = this.eat(token.value);
            return new BoolLiteral(valueToken, this.scope(), valueToken.value === "TRUE");
        }
        if (token.type === "IDENT") {
            return this.variable_reference();
        }
        throw new ParseError(
            "expected an identifier or INTEGER/REAL/BOOLEAN/STRING literal or '+', '-', '(', 'TRUE/FALSE'",
            token
        );
    }
}

class ConcatenationOperation extends Expression {
    parts: Expression[];
    constructor(token: Token, scope: string, type: Type, parts: Expression[]) {
        super(token, scope, type);
        this.parts = parts;
    }
    c(): string {
        const context = this.context();
        const r = `$r${context.r++}`;

        const pre: string[] = [];
        const fmt: string[] = [];
        const args = this.parts.map((p) => expression_stringer(p, fmt, "||", pre)).join(", ");
        pre.push(`const auto ${r} = $concat("${fmt.join("")}", ${args}); /* ${r} */`);
        return emit(pre);
    }
}

class PROGRAM extends Node {
    name: string;
    segment: Segment;
    constructor(token: Token, scope: string, name: string, segment: Segment) {
        super(token, scope);
        this.name = name;
        this.segment = segment;
    }
    c(): string {
        return emit(["int main()", "{", indent(this.segment.c(true), 1), "}"]);
    }
}

class Compiler {
    context: Context;
    lexer!: Lexer;
    parser!: Parser;

    constructor(text: InputText) {
        this.context = new Context({
            text,
            types: {
                INTEGER: new IntegerType(),
                REAL: new RealType(),
                BOOLEAN: new BooleanType(),
                STRING: new StringType(),
            },
            functions: {
                LENGTH: new BuiltinFunction("LENGTH", new IntegerType()),
                CHARACTER: new BuiltinFunction("CHARACTER", new StringType()),
                SUBSTR: new BuiltinFunction("SUBSTR", new StringType()),
                FIX: new BuiltinFunction("FIX", new IntegerType()),
                FLOAT: new BuiltinFunction("FLOAT", new RealType()),
            },
            procedures: {},
            variables: {},
            flags: {},
            tokens: [],
            common: [],
        });
    }

    compile(): PROGRAM {
        this.lexer = new Lexer(this.context);
        this.context.tokens = this.lexer.tokenize();
        this.parser = new Parser(this.context);
        return this.parser.program();
    }
}

function run(argv: string[]) {
    if (argv.length < 3) {
        const exe = path.basename(argv[1] || "easyc.ts");
        console.log(`usage: ${exe} <input.easy> [-c <output.c>] [-t] [-a] [-e]`);
        console.log("  -c <output.c>  - specify output C file (default: input.c)");
        console.log("  -t             - generate tokens file (default: input.tokens)");
        console.log("  -a             - generate JSON AST file (default: input.json)");
        console.log("  -e             - generate PEG JSON AST file (default: input.peg.json)");
        console.log("  -s <output.s>  - generate symbols file (default: input.s)");
        process.exit(1);
    }

    const inputFile = argv[2];
    const compiler = new Compiler(new InputText({ filename: inputFile }));
    const context = compiler.context;

    const source = fs.readFileSync(inputFile, "utf-8");
    const firstLine = source.split(/\r?\n/)[0]?.trim() ?? "";
    if (firstLine.startsWith("//flags ")) {
        const pairs: string[] = firstLine.split(/\s+/).slice(1);
        const flags = Object.fromEntries(pairs.map((v) => v.split("=")));
        Object.assign(context.flags, flags);
    }

    const program = compiler.compile();

    if (argv.includes("-t")) {
        const tokensFile = inputFile.replace(/\.[^.]+$/, "") + ".tokens";
        const lines = context.tokens.map((token) => {
            const input = token.context.text;
            return `${input.filename}:${token.line}:${token.character}\t ${token.value} / ${token.type}`;
        });
        fs.writeFileSync(tokensFile, lines.join("\n") + "\n", "utf-8");
    }

    if (argv.includes("-a")) {
        const astFile = inputFile.replace(/\.[^.]+$/, "") + ".json";
        fs.writeFileSync(astFile, printer(program), "utf-8");
    }

    const symbolsFile = arg(argv, "-s") ?? inputFile.replace(/\.[^.]+$/, "") + ".s";

    const variables: string[][] = [];
    for (const [name, variable] of Object.entries(context.variables)) variables.push(variable.s(name));

    const types: string[][] = [];
    for (const [name, type] of Object.entries(context.types)) types.push([name, type.c().replace(/\s+/g, " ")]);

    const functions: string[][] = [];
    for (const [name, fun] of Object.entries(context.functions)) {
        if (fun instanceof BuiltinFunction) {
            functions.push([fun.name, "->", fun.type.c(), "built-in"]);
        } else {
            const args = fun.arguments.map((a) => `${a.name} ${a.type.c()}`).join(", ");
            functions.push([fun.name, "->", fun.type.c(), fun.constructor.name, `(${args})`, String(fun.token)]);
        }
    }

    const procedures: string[][] = [];
    for (const [, proc] of Object.entries(context.procedures)) {
        const args = proc.arguments.map((a) => `${a.name} ${a.type.c()}`).join(", ");
        procedures.push([proc.name, proc.constructor.name, `(${args})`, String(proc.token)]);
    }

    const sOut = table(variables) + "\n" + table(types) + "\n" + table(functions) + "\n" + table(procedures);
    fs.writeFileSync(symbolsFile, sOut + "\n", "utf-8");

    const outputFile = arg(argv, "-c") ?? inputFile.replace(/\.[^.]+$/, "") + ".c";
    const compiledText = program.c().trim();

    const output: string[] = [];
    output.push('#include "runtime.c"\n');
    for (const [name, def] of Object.entries(context.types)) output.push(def.typedef(name) + ";\n");
    if (context.common.length) output.push(emit(context.common) + "\n");
    for (const v of Object.values(context.variables)) if (v.isConst()) output.push(v.const() + ";\n");
    for (const f of Object.values(context.functions)) {
        if (f instanceof BuiltinFunction) continue;
        output.push((f as FUNCTION).c() + "\n");
    }
    for (const v of Object.values(context.procedures)) output.push(v.c() + "\n");
    output.push(compiledText + "\n");

    fs.writeFileSync(outputFile, output.join(""), "utf-8");
}

if (import.meta.main) {
    try {
        run(process.argv);
    } catch (e: any) {
        if (e instanceof ParseError || e instanceof LexerError || e instanceof GenerateError) {
            console.error(String(e));
            process.exit(2);
        }
        console.error(e?.stack || String(e));
        process.exit(2);
    }
}
