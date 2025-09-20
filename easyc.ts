#!/usr/bin/env ts-node

// easyc.ts — TypeScript rewrite of the “Easy” single-file compiler
//
// Usage:
//   bun easyc.ts <input.easy> [-c <output.c>] [-t] [-a] [-e] [-s <output.s>]

import * as fs from "fs";
import * as path from "path";
import { YAML } from "bun";
// -------------------------- Utilities --------------------------

function indent(s: string, n: number): string {
    const pad = "    ".repeat(n);
    return s
        .split(/\r?\n/)
        .map((line) => pad + line)
        .join("\n");
}

function emit(lines: string[]): string {
    return lines.join("\n");
}

function table(rows: string[][]): string {
    if (!rows.length) return "";
    const nCols = Math.max(...rows.map((r) => r.length));
    const widths = Array(nCols).fill(0);
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

// -------------------------- Input & Token --------------------------

class InputText {
    filename: string;
    text: string;
    constructor(opts: { text?: string; filename?: string | path.ParsedPath | null }) {
        const { text = "", filename = null } = opts;
        if (text) {
            this.text = text;
            this.filename = "";
        } else if (filename) {
            const fname = typeof filename === "string" ? filename : (filename as any).toString();
            this.text = fs.readFileSync(fname, "utf-8");
            this.filename = fname;
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
        const t = this.token;
        const lines = t.context.text.text.split(/\r?\n/);
        const errorLine = lines[t.line - 1] || "";
        return (
            `${this.message}\n` +
            `at ${t.context.text.filename}:${t.line}:${t.character}\n` +
            `${errorLine}\n` +
            `${" ".repeat(Math.max(0, t.character - 1))}^`
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
    }
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

// -------------------------- YAML-izer --------------------------

// -------------------------- YAML-izer --------------------------

function yamlizer(root: any): string {
    const seen = new Set<any>();

    function isPlainObject(o: any) {
        if (o === null || typeof o !== "object") return false;
        const proto = Object.getPrototypeOf(o);
        return proto === Object.prototype || proto === null;
    }

    function walker(obj: any): any {
        if (obj == null) {
            return "";
        }

        if (typeof obj === "boolean" || typeof obj === "number" || typeof obj === "string") {
            return obj;
        }

        if (typeof obj === "object") {
            if (seen.has(obj)) {
                throw new Error("cycle detected in AST: serialization would recurse forever");
            }
            seen.add(obj);

            // Token special display
            if (obj instanceof Token) {
                const token = obj as Token;
                const data = `<${token.value}|${token.type} ${token.context.text.filename}:${token.line}:${token.character}>`;
                seen.delete(obj);
                return data;
            }

            // ✅ Arrays first
            if (Array.isArray(obj)) {
                const seq = obj.map((x) => walker(x));
                seen.delete(obj);
                return seq;
            }

            // ✅ "Node-like" = non-plain class instances (not arrays, not {})
            if (!isPlainObject(obj)) {
                const data: any = { node: obj.constructor?.name ?? "Object" };
                for (const k of Object.keys(obj)) {
                    if (k.startsWith("_")) continue;
                    data[k] = walker((obj as any)[k]);
                }
                seen.delete(obj);
                return data;
            }

            // Plain object mapping
            const out: any = {};
            for (const [k, v] of Object.entries(obj)) {
                out[k] = walker(v); // keys from Object.entries are strings already
            }
            seen.delete(obj);
            return out;
        }

        return String(obj);
    }

    const data = walker(root);
    // return YAML.stringify(data, {
    //     singleQuote: true,
    //     indent: 2,
    //     nullStr: "",
    //     defaultKeyType: "PLAIN",
    // }).replaceAll(`initial: ''`, `initial:`);
    return JSON.stringify(data, null, 2);
    return YAML.stringify(data, null, 2);
}

// -------------------------- Type System --------------------------

abstract class Type {
    c(): string {
        throw new GenerateError(`c() not implemented for ${this.constructor.name}`);
    }
    zero(): string {
        throw new GenerateError(`zero() not implemented for ${this.constructor.name}`);
    }
    typedef(_alias: string): string {
        throw new GenerateError(`typedef() not implemented for ${this.constructor.name}`);
    }
    format(): string {
        return "";
    }
}

abstract class BuiltinType extends Type {}

class IntegerType extends BuiltinType {
    c() {
        return "int";
    }
    zero() {
        return "0";
    }
    typedef(alias: string) {
        return `typedef int ${alias}`;
    }
    format() {
        return "i";
    }
}
class RealType extends BuiltinType {
    c() {
        return "double";
    }
    zero() {
        return "0.0";
    }
    typedef(alias: string) {
        return `typedef double ${alias}`;
    }
    format() {
        return "r";
    }
}
class BooleanType extends BuiltinType {
    c() {
        return "int";
    }
    zero() {
        return "0";
    }
    typedef(alias: string) {
        return `typedef int ${alias}`;
    }
    format() {
        return "b";
    }
}
class StringType extends BuiltinType {
    initial?: string;
    constructor(initial?: string) {
        super();
        this.initial = initial || "";
    }
    c() {
        return "STR";
    }
    zero() {
        return this.initial ? `{ .data = "${this.initial}" }` : "{0}";
    }
    typedef(alias: string) {
        return `typedef STR ${alias}`;
    }
    format() {
        return "A";
    }
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
        if (this.dynamic) {
            return ["struct", "{", indent(`${this.type.c()} *data;`, 1), "}"].join("\n");
        } else {
            return ["struct", "{", indent(`${this.type.c()} data[${this.sz()}];`, 1), "}"].join("\n");
        }
    }
    sz(): string {
        return `${this.hi.c()} - ${this.lo.c()} + 1`;
    }
    zero(): string {
        if (this.dynamic) {
            return `{ .data = malloc(sizeof(${this.type.c()}) * (${this.sz()})) }`;
        }
        return "{0}";
    }
    typedef(to: string): string {
        return `typedef ${this.c()} ${to}`;
    }
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
    c(): string {
        return `${this.type.c()} ${this.name}`;
    }
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
    init(): string {
        return "{0}";
    }
    zero(): string {
        return "{0}";
    }
    typedef(name: string): string {
        return `typedef ${this.c()} ${name}`;
    }
}

class AliasType extends Type {
    reference_name: string;
    reference_type: Type;
    constructor(name: string, ref: Type) {
        super();
        this.reference_name = name;
        this.reference_type = ref;
    }
    c() {
        return this.reference_name;
    }
    zero() {
        return this.reference_type.zero();
    }
    typedef(alias: string) {
        return `typedef ${this.reference_name} ${alias}`;
    }
}

// -------------------------- AST --------------------------

abstract class Node {
    token: Token;
    scope: string;
    constructor(token: Token, scope: string) {
        this.token = token;
        this.scope = scope;
    }
    context(): Context {
        return this.token.context;
    }
    c(): string {
        throw new GenerateError(`c() not implemented for ${this.constructor.name} at ${this.token}`);
    }
    toString() {
        return yamlizer(this);
    }
}
abstract class Statement extends Node {}
abstract class Expression extends Node {
    type: Type;
    constructor(t: Token, s: string, type: Type) {
        super(t, s);
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
    constructor(t: Token, s: string, n: string, d: Type) {
        super(t, s);
        this.name = n;
        this.definition = d;
    }
}

class DECLARE extends Node {
    names: string[];
    type: Type;
    constructor(t: Token, s: string, names: string[], type: Type) {
        super(t, s);
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
        t: Token,
        s: string,
        types: TYPEIS[],
        variables: DECLARE[],
        subs: (FUNCTION | PROCEDURE)[],
        statements: Statement[]
    ) {
        super(t, s);
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
            for (const s of this.statements) v.push(s.c());
        }
        return emit(v);
    }
}

class FIELD extends Entity {
    name: string;
    type: Type;

    constructor(token: Token, name: string, type: Type) {
        super(token);
        this.name = name;
        this.type = type;
    }
}

class STRUCTURE extends Node {
    fields: FIELD[];
    constructor(t: Token, s: string, fields: FIELD[]) {
        super(t, s);
        this.fields = fields;
    }
    c(): string {
        return `struct { ${this.fields.map((f) => f.c() + ";").join(" ")} }`;
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
    constructor(t: Token, s: string, name: string, args: Argument[], seg: Segment) {
        super(t, s);
        this.name = name;
        this.arguments = args;
        this.segment = seg;
    }
    c(): string {
        const args = this.arguments.map((a) => a.c()).join(", ");
        return emit([`void ${this.name}(${args})`, "{", indent(this.segment.c(), 1), "}"]);
    }
}

class FUNCTION extends Node {
    name: string;
    type: Type;
    arguments: Argument[];
    segment: Segment;
    constructor(t: Token, s: string, name: string, type: Type, args: Argument[], seg: Segment) {
        super(t, s);
        this.name = name;
        this.type = type;
        this.arguments = args;
        this.segment = seg;
    }
    c(): string {
        const fn = this.context().functions[this.name] as BuiltinFunction | FUNCTION;
        const ret = (fn instanceof BuiltinFunction ? fn.type : fn.type).c();
        const args = this.arguments.map((a) => a.c()).join(", ");
        return emit([`${ret} ${this.name}(${args})`, "{", indent(this.segment.c(), 1), "}"]);
    }
}

class Label extends Node {
    name: string;
    constructor(t: Token, s: string, name: string) {
        super(t, s);
        this.name = name;
    }
    c() {
        return `${this.name}:`;
    }
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
        return discover_variable(this).type;
    }
    // override set type(_t: Type) {
    //     /* not used directly */
    // }
    c(): string {
        const variable = discover_variable(this);
        const [, reference] = expand_variable_reference(variable, this);
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
    constructor(t: Token, s: string, v: number) {
        super(t, s, new IntegerType());
        this.value = v;
    }
    c() {
        return String(this.value);
    }
    format() {
        return "i";
    }
}
class RealLiteral extends BuiltinLiteral {
    value: number;
    constructor(t: Token, s: string, v: number) {
        super(t, s, new RealType());
        this.value = v;
    }
    c() {
        const v = String(this.value);
        return v.includes(".") || v.includes("e") ? v : v + ".0";
    }
    format() {
        return "r";
    }
}
class StringLiteral extends BuiltinLiteral {
    value: string;
    constructor(t: Token, s: string, v: string) {
        super(t, s, new StringType());
        this.value = v;
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
    constructor(t: Token, s: string, v: boolean) {
        super(t, s, new BooleanType());
        this.value = v;
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
    constructor(t: Token, s: string, target: VariableReference[], expr: Expression) {
        super(t, s);
        this.target = target;
        this.expression = expr;
    }
    c(): string {
        const v: string[] = [];
        for (const t of this.target) {
            const variable = discover_variable(t);
            const [, ref] = expand_variable_reference(variable, t);
            v.push(`${ref} = ${this.expression.c()};`);
        }
        return emit(v);
    }
}

class IF extends Statement {
    cond: Expression;
    then_branch: Segment;
    else_branch?: Segment | null;
    constructor(t: Token, s: string, cond: Expression, tb: Segment, eb?: Segment | null) {
        super(t, s);
        this.cond = cond;
        this.then_branch = tb;
        this.else_branch = eb ?? null;
    }
    c(): string {
        let cond = this.cond.c();
        if (cond.startsWith("(") && cond.endsWith(")")) cond = cond.slice(1, -1);
        const v = [`if (${cond})`, "{", indent(this.then_branch.c(), 1), "}"];
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
        t: Token,
        s: string,
        variable: VariableReference,
        init: Expression,
        doX: Segment,
        by?: Expression | null,
        to?: Expression | null,
        cond?: Expression | null
    ) {
        super(t, s);
        this.variable = variable;
        this.init = init;
        this.segment = doX;
        this.by = by ?? null;
        this.to = to ?? null;
        this.condition = cond ?? null;
    }
    c(): string {
        const header = `for (${this.variable.c()} = ${this.init.c()}; ${this.format_condition()}; ${this.step()})`;
        return emit([header, "{", indent(this.segment.c(), 1), "}"]);
    }
    format_condition(): string {
        const conditions: string[] = [];
        if (this.condition) conditions.push(this.condition.c());
        if (this.to) conditions.push(`${this.variable.c()} <= ${this.to.c()}`);
        return conditions.join("");
    }
    step(): string {
        return `${this.variable.c()} += ${this.by ? this.by.c() : "1"}`;
    }
}

class SELECT extends Statement {
    expr: Expression;
    cases: Array<[Expression | null, Segment]>;
    constructor(t: Token, s: string, expr: Expression, cases: Array<[Expression | null, Segment]>) {
        super(t, s);
        this.expr = expr;
        this.cases = cases;
    }
    c(): string {
        const v: string[] = [];
        for (let i = 0; i < this.cases.length; i++) {
            const [cond, body] = this.cases[i];
            if (cond) v.push((i > 0 ? "else " : "") + "if " + cond.c());
            else v.push("else");
            v.push("{", indent(body.c(), 1), "}");
        }
        return emit(v);
    }
}

class INPUT extends Statement {
    vars: VariableReference[];
    constructor(t: Token, s: string, variables: VariableReference[]) {
        super(t, s);
        this.vars = variables;
    }
    c(): string {
        const inputs: string[] = [];
        for (const vr of this.vars) {
            const variable = discover_variable(vr);
            const [type, reference] = expand_variable_reference(variable, vr);
            if (type instanceof StringType) inputs.push(`scanf("%s", ${reference}.data);`);
            else if (type instanceof IntegerType) inputs.push(`scanf("%d", &${reference});`);
            else if (type instanceof RealType) inputs.push(`scanf("%lf", &${reference});`);
            else
                throw new GenerateError(
                    `unsupported variable '${variable.name}' type in INPUT at ${String(variable.token)}`
                );
        }
        return emit(inputs);
    }
}

class OUTPUT extends Statement {
    arguments: Expression[];
    constructor(t: Token, s: string, args: Expression[]) {
        super(t, s);
        this.arguments = args;
    }
    c(): string {
        const fmt: string[] = [];
        const params = this.arguments.map((a) => expression_stringer(a, fmt, "OUTPUT")).join(", ");
        return `$output("${fmt.join("")}", ${params});`;
    }
}

class REPEAT extends Statement {
    label: string;
    constructor(t: Token, s: string, label: string) {
        super(t, s);
        this.label = label;
    }
    c() {
        return `goto ${this.label};`;
    }
}
class REPENT extends Statement {
    label: string;
    constructor(t: Token, s: string, label: string) {
        super(t, s);
        this.label = label;
    }
    c() {
        return `goto ${this.label};`;
    }
}

class BEGIN extends Statement {
    body: Segment;
    label?: string | null;
    constructor(t: Token, s: string, body: Segment, label?: string | null) {
        super(t, s);
        this.body = body;
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
    constructor(t: Token, s: string, name: string, args: Expression[]) {
        super(t, s);
        this.name = name;
        this.arguments = args;
    }
    c(): string {
        return `${this.name}(${this.arguments.map((a) => a.c()).join(", ")});`;
    }
}

class RETURN extends Statement {
    value?: Expression | null;
    constructor(t: Token, s: string, v?: Expression | null) {
        super(t, s);
        this.value = v ?? null;
    }
    c(): string {
        return this.value ? `return ${this.value.c()};` : "return;";
    }
}
class EXIT extends Statement {
    c() {
        return "exit(0);";
    }
}
class EMPTY extends Statement {
    c() {
        return ";";
    }
}

const OPERATIONS: Record<string, string> = { "|": "||", "&": "&&", "=": "==", "<>": "!=", MOD: "%", XOR: "^" };

class FunctionCall extends Expression {
    name: string;
    arguments: Expression[];
    constructor(t: Token, s: string, type: Type, name: string, args: Expression[]) {
        super(t, s, type);
        this.name = name;
        this.arguments = args;
    }
    c(): string {
        return `${this.name}(${this.arguments.map((a) => a.c()).join(", ")})`;
    }
}

class BinaryOperation extends Expression {
    operation: string;
    left: Expression;
    right: Expression;
    constructor(t: Token, s: string, type: Type, op: string, left: Expression, right: Expression) {
        super(t, s, type);
        this.operation = op;
        this.left = left;
        this.right = right;
    }
    c(): string {
        const op = OPERATIONS[this.operation] ?? this.operation;
        const st = string_compare(this.left, this.right, op);
        if (st) return st;
        return `(${this.left.c()} ${op} ${this.right.c()})`;
    }
}
class UnaryOperation extends Expression {
    operation: string;
    expr: Expression;
    constructor(t: Token, s: string, op: string, val: string, expr: Expression) {
        super(t, s, expr.type);
        this.operation = op === "NOT" ? "!" : val;
        this.expr = expr;
    }
    c(): string {
        const operation = this.operation === "NOT" ? "!" : this.operation;
        return `(${operation}${this.expr.c()})`;
    }
}

function string_compare(left: Expression, right: Expression, operation: string): string | null {
    if (operation !== "==" && operation !== "!=") return null;
    function is_string_type(e: Expression): [boolean, string | null] {
        if (!(e instanceof VariableReference)) return [false, null];
        const variable = discover_variable(e);
        const [type, reference] = expand_variable_reference(variable, e);
        return [type instanceof StringType, reference];
    }
    const [lIs, lRef] = is_string_type(left);
    const [rIs, rRef] = is_string_type(right);
    if (lIs || rIs) {
        const cmp = operation === "!=" ? "!=" : "==";
        return `strcmp(${lRef}.data, ${rRef}.data) ${cmp} 0`;
    }
    return null;
}

// -------------------------- Lexer --------------------------

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
    current(): string {
        return this.position < this.n ? this.text[this.position] : "";
    }

    advance(k = 1) {
        for (let i = 0; i < k; i++) {
            if (this.position < this.n) {
                const ch = this.text[this.position++];
                if (ch === "\n") {
                    this.line++;
                    this.character = 1;
                } else this.character++;
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
        const line = this.line,
            character = this.character;
        let v = "";
        while (/\d/.test(this.current())) {
            v += this.current();
            this.advance();
        }
        if (this.current() === "." || this.current().toLowerCase() === "e") {
            v += this.current();
            this.advance();
            while (/\d/.test(this.current()) || /[+\-eE]/.test(this.current())) {
                v += this.current();
                this.advance();
            }
            return new Token("REAL", v, line, character, this.context);
        }
        return new Token("INTEGER", v, line, character, this.context);
    }

    ident_or_keyword(): Token {
        const line = this.line,
            character = this.character;
        let v = "";
        let ch = this.current();
        if (/[A-Za-z_]/.test(ch)) {
            v += ch;
            this.advance();
            while (/[A-Za-z0-9_]/.test(this.current())) {
                v += this.current();
                this.advance();
            }
        }
        const value = v;
        if (KEYWORDS.has(value)) return new Token("KEYWORD", value, line, character, this.context); // match Python: token.type="KEYWORD" but usando value como tipo aquí
        return new Token("IDENT", v, line, character, this.context);
    }

    string(): Token {
        const line = this.line,
            character = this.character;
        const quote = this.current();
        this.advance();
        let v = "";
        while (true) {
            const c = this.current();
            if (!c) {
                const loc = `${this.input.filename}:${line}:${character}`;
                throw new LexerError(`unterminated string at ${loc}`);
            }
            if (c === quote) {
                this.advance();
                if (this.current() === quote) {
                    v += quote;
                    this.advance();
                    continue;
                }
                break;
            }
            v += c;
            this.advance();
        }
        return new Token("STRING", v, line, character, this.context);
    }

    symbol(): Token {
        const start = this.line,
            character = this.character;
        const two = this.current() + this.peek();
        if (SYMBOLS.has(two)) {
            this.advance(2);
            return new Token("SYMBOL", two, start, character, this.context);
        }
        const one = this.current();
        if (SYMBOLS.has(one)) {
            this.advance();
            return new Token("SYMBOL", one, start, character, this.context);
        }
        const location = `${this.input.filename}:${start}:${character}`;
        throw new LexerError(`unknown symbol '${one}' at ${location}`);
    }

    tokenize(): Token[] {
        const v: Token[] = [];
        while (true) {
            this.skip_whitespace_and_comments();
            if (this.position >= this.n) break;
            const ch = this.current();
            if (/\d/.test(ch)) v.push(this.number());
            else if (/[A-Za-z_]/.test(ch)) v.push(this.ident_or_keyword());
            else if (ch === '"') v.push(this.string());
            else v.push(this.symbol());
        }
        v.push(new Token("EOF", "", this.line, this.character, this.context));
        return v;
    }
}

// -------------------------- Parser helpers --------------------------

class GenerateError extends Error {}

function is_number_name(name: string): boolean {
    return name === "INTEGER" || name === "REAL";
}

function expression_stringer(v: Expression, fmt: string[], callee: string): string {
    const c = v.c();
    if (v instanceof BuiltinLiteral) {
        fmt.push(v.format());
        return c;
    }
    if (v instanceof VariableReference) {
        const variable = discover_variable(v);
        let [t, reference] = expand_variable_reference(variable, v);
        if (t instanceof AliasType) t = t.reference_type;
        if (!(t instanceof BuiltinType))
            throw new GenerateError(
                `unsupported ${callee} variable argument '${String(v)}' of type '${t.constructor.name}' at ${String(
                    v.token
                )}`
            );
        fmt.push(t.format());
        return reference;
    }
    if (v instanceof FunctionCall) {
        const fn = v.context().functions[v.name];
        const t = fn instanceof BuiltinFunction ? fn.type : fn.type;
        if (!(t instanceof BuiltinType))
            throw new GenerateError(
                `unsupported ${callee} function argument ${String(v)} of type ${t.constructor.name} at ${String(
                    v.token
                )}`
            );
        fmt.push(t.format());
        return c;
    }
    if (v instanceof ConcatenationOperation) {
        fmt.push("A");
        return c;
    }
    throw new GenerateError(`unsupported ${callee} argument '${String(v)}' at ${String(v.token)}`);
}

// -------------------------- Expand variable refs --------------------------

function expand_variable_reference(variable: Variable, variable_reference: VariableReference): [Type, string] {
    const context = variable.token.context;
    if (context.flags["index_check"] === "0") return expand_variable_reference_direct(variable, variable_reference);
    return expand_variable_reference_bound_checked(variable, variable_reference);
}

function expand_variable_reference_bound_checked(
    variable: Variable,
    variable_reference: VariableReference
): [Type, string] {
    let variable_type: Type = variable.type;
    let reference_expression = variable.name; // lvalue or pointer to aggregate
    let probe_expression = variable.name;
    let is_pointer = false;
    const parts = [...variable_reference.parts];
    if (!parts.length) return [variable_type, reference_expression];

    let result_reference: string | null = null;

    for (let i = 0; i < parts.length; i++) {
        const part = parts[i];
        if (part instanceof VariableSubscript) {
            const subscript_type = variable_type instanceof AliasType ? variable_type.reference_type : variable_type;
            if (!(subscript_type instanceof ArrayType))
                throw new GenerateError(
                    `expect ArrayType in reference type of subscript, not ${
                        subscript_type.constructor.name
                    } at ${String(part.token)}`
                );
            const array_type = subscript_type;
            const lo = array_type.lo.c();
            const hi = array_type.hi.c();
            const index = part.index();

            const element_typeof = `typeof(${probe_expression}.data[0])`;
            const element_sizeof = `sizeof(${element_typeof})`;
            const data_expression = is_pointer ? `(${reference_expression})->data` : `${reference_expression}.data`;
            const location = '"' + String(part.token).replace(/"/g, '\\"') + '"';
            const current_reference = `(${element_typeof} *)$ref(${data_expression}, ${index}, ${lo}, ${hi}, ${element_sizeof}, ${location})`;

            const is_last = i === parts.length - 1;
            if (is_last) {
                result_reference = `*${current_reference}`;
            } else {
                reference_expression = current_reference;
                is_pointer = true;
                probe_expression = `${probe_expression}.data[0]`;
            }
            variable_type = array_type.type;
        } else if (part instanceof VariableField) {
            const field_type = variable_type instanceof AliasType ? variable_type.reference_type : variable_type;
            if (!(field_type instanceof StructType))
                throw new GenerateError(
                    `expect StructType in reference type of field, not ${field_type.constructor.name}, at ${String(
                        part.token
                    )}`
                );
            const struct_type = field_type;
            const field = struct_type.fields.find((f) => f.name === part.name);
            if (!field) throw new GenerateError(`field '${part.name}' not found in struct at ${String(part.token)}`);
            const accessor = is_pointer ? `->${part.name}` : `.${part.name}`;
            reference_expression = is_pointer
                ? `(${reference_expression})${accessor}`
                : `${reference_expression}${accessor}`;
            probe_expression = `${probe_expression}.${part.name}`;
            variable_type = field.type;
            is_pointer = false;
        } else {
            throw new GenerateError(`unexpected variable part at ${String(variable.token)}`);
        }
    }
    if (result_reference === null) result_reference = reference_expression;
    return [variable_type, result_reference];
}

function expand_variable_reference_direct(variable: Variable, variable_reference: VariableReference): [Type, string] {
    let type: Type = variable.type;
    let reference = variable.name;
    for (const part of variable_reference.parts) {
        if (part instanceof VariableSubscript) {
            if (type instanceof AliasType) type = type.reference_type;
            if (!(type instanceof ArrayType))
                throw new GenerateError(
                    `expect ArrayType in reference type of subscript, not ${type.constructor.name} at ${String(
                        part.token
                    )}`
                );
            const lo = type.lo.c();
            const index = `(${part.index()}) - (${lo})`;
            type = type.type;
            reference += `.data[${index}]`;
        } else if (part instanceof VariableField) {
            if (type instanceof AliasType) type = type.reference_type;
            if (!(type instanceof StructType))
                throw new GenerateError(
                    `expect StructType in reference type of field, not ${type.constructor.name} at ${String(
                        part.token
                    )}`
                );
            const field = type.fields.find((f) => f.name === part.name);
            if (!field) throw new GenerateError(`field '${part.name}' not found in struct at ${String(part.token)}`);
            type = field.type;
            reference += part.c();
        } else {
            throw new GenerateError(`unexpected part at ${String(variable.token)}`);
        }
    }
    return [type, reference];
}

function discover_variable(v: VariableReference): Variable {
    const context = v.token.context;
    const parts = v.scope.split(".");
    let scopeParts = [...parts];
    let variable: Variable | undefined;
    while (scopeParts.length) {
        const fqn = scopeParts.join(".") + "|" + v.name;
        variable = context.variables[fqn];
        if (variable) break;
        scopeParts.pop();
    }
    if (!variable)
        throw new GenerateError(`undefined variable '${v.name}' in scope '${v.scope}' at ${String(v.token)}`);
    return variable;
}

function enlist_type(name: string, type: Type, context: Context) {
    if (context.types[name]) throw new GenerateError(`type '${name}' already defined at ${name}`);
    context.types[name] = type;
}
function enlist_variable(variable: Variable, scope: string) {
    const fqn = scope + "|" + variable.name;
    variable.token.context.variables[fqn] = variable;
}

// -------------------------- Parser --------------------------

class Parser {
    context: Context;
    tokens: Token[];
    i = 0;
    scopes: string[] = [];
    constructor(context: Context) {
        this.context = context;
        this.tokens = context.tokens;
    }

    scope(): string {
        return this.scopes.length ? this.scopes.join(".") : "@";
    }
    enter_scope(name: string) {
        this.scopes.push(name);
    }
    leave_scope() {
        this.scopes.pop();
    }
    current(): Token {
        return this.tokens[this.i];
    }
    peek(): Token {
        const cur = this.current();
        return this.i + 1 < this.tokens.length
            ? this.tokens[this.i + 1]
            : new Token("EOF", "", cur.line, cur.character, this.context);
    }

    eat(kind: string | string[]): Token {
        const kinds = Array.isArray(kind) ? kind : [kind];
        const token = this.current();
        if (kinds.includes(token.type) || kinds.includes(token.value)) {
            this.i++;
            return token;
        }
        const expected = kinds.join("/");
        throw new ParseError(`expected '${expected}', found '${token.value}'`, token);
    }
    accept(kind: string | string[]): Token | null {
        const kinds = Array.isArray(kind) ? kind : [kind];
        const token = this.current();
        if (kinds.includes(token.type) || kinds.includes(token.value)) {
            this.i++;
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
        const decls: DECLARE[] = [];
        while (true) {
            const declareToken = this.accept("DECLARE");
            if (!declareToken) break;
            const token = this.current();
            if (token.type === "IDENT") {
                const name = this.eat("IDENT").value;
                const type = this.parse_type();
                this.eat(";");
                decls.push(new DECLARE(declareToken, this.scope(), [name], type));
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
                decls.push(new DECLARE(declareToken, this.scope(), names, type));
                for (const name of names) enlist_variable(new Variable(token, name, type), this.scope());
                continue;
            }
            throw new ParseError("expected a variable or '(' variable, ... ')'", token);
        }
        return decls;
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
        const tok = this.eat("IDENT");
        const aliasName = tok.value;
        const aliasType = this.context.types[aliasName];
        if (!aliasType) throw new ParseError(`unknown type alias '${aliasName}'`, tok);
        return new AliasType(aliasName, aliasType);
    }

    procedures_and_functions_section(): (PROCEDURE | FUNCTION)[] {
        const subs: (PROCEDURE | FUNCTION)[] = [];
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
                const args = parameters.map((p) => new Argument(p.token, p.name, p.type));
                const sub = new PROCEDURE(token, this.scope(), name, args, segment);
                this.context.procedures[name] = sub;
                subs.push(sub);
            } else {
                const args = parameters.map((p) => new Argument(p.token, p.name, p.type));
                const sub = new FUNCTION(token, this.scope(), name, type!, args, segment);
                this.context.functions[name] = sub;
                subs.push(sub);
            }
            this.leave_scope();
        }
        return subs;
    }

    parameters(): Variable[] {
        const params: Variable[] = [];
        while (true) {
            const token = this.eat("IDENT");
            const name = token.value;
            const type = this.parse_type();
            const variable = new Variable(token, name, type);
            params.push(variable);
            enlist_variable(variable, this.scope());
            if (!this.accept(",")) break;
        }
        return params;
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
            const cur = this.current();
            return cur.type === "IDENT" && cur.value !== "OTHERWISE" && this.peek().value === ":";
        };
        while (STATEMENTS.has(this.current().value) || is_label()) {
            if (is_label()) {
                const labelToken = this.eat("IDENT");
                const label = labelToken.value;
                this.eat(":");
                statements.push(new Label(labelToken, this.scope(), label));
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
        const cond = this.expression();
        this.eat("THEN");
        const thenb = this.segment();
        const elseb = this.accept("ELSE") ? this.segment() : null;
        this.eat("FI");
        this.eat(";");
        return new IF(token, this.scope(), cond, thenb, elseb);
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
        const doX = this.segment();
        this.eat("END");
        this.eat("FOR");
        this.eat(";");
        return new FOR(token, this.scope(), variable, init, doX, by, to, cond);
    }

    select_statement(): SELECT {
        const token = this.eat("SELECT");
        const expr = this.expression();
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
            const body = this.segment();
            cases.push([null, body]);
        }
        this.eat("END");
        this.eat("SELECT");
        this.eat(";");
        return new SELECT(token, this.scope(), expr, cases);
    }

    return_statement(): RETURN {
        const token = this.eat("RETURN");
        if (this.accept(";")) return new RETURN(token, this.scope(), null);
        const value = this.expression();
        this.eat(";");
        return new RETURN(token, this.scope(), value);
    }
    exit_statement(): EXIT {
        const t = this.eat("EXIT");
        this.eat(";");
        return new EXIT(t, this.scope());
    }

    input_statement(): INPUT {
        const token = this.eat("INPUT");
        const vars = [this.variable_reference()];
        while (this.accept(",")) vars.push(this.variable_reference());
        this.eat(";");
        return new INPUT(token, this.scope(), vars);
    }

    output_statement(): OUTPUT {
        const token = this.eat("OUTPUT");
        const expressions = [this.expression()];
        while (this.accept(",")) expressions.push(this.expression());
        this.eat(";");
        return new OUTPUT(token, this.scope(), expressions);
    }

    repeat_statement(): REPEAT {
        const t = this.eat("REPEAT");
        const label = this.eat("IDENT").value;
        this.eat(";");
        return new REPEAT(t, this.scope(), label);
    }
    repent_statement(): REPENT {
        const t = this.eat("REPENT");
        const label = this.eat("IDENT").value;
        this.eat(";");
        return new REPENT(t, this.scope(), label);
    }

    begin_statement(): BEGIN {
        const t = this.eat("BEGIN");
        const body = this.segment();
        this.eat("END");
        const lbl = this.accept("IDENT");
        this.eat(";");
        return new BEGIN(t, this.scope(), body, lbl ? lbl.value : null);
    }

    assignment_statement(): SET {
        const token = this.eat("SET");
        let variable = this.variable_reference();
        this.eat(":=");
        const targets = [variable];
        while (true) {
            const save = this.i;
            if (this.current().type !== "IDENT") break;
            variable = this.variable_reference();
            if (this.current().value !== ":=") {
                this.i = save;
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
            const op = this.accept("&");
            if (!op) break;
            const right = this.expression_NOT();
            left = new BinaryOperation(token, this.scope(), new BooleanType(), op.value, left, right);
        }
        return left;
    }
    expression_NOT(): Expression {
        const token = this.accept("NOT");
        if (token) return new UnaryOperation(token, this.scope(), token.type, token.value, this.expression_NOT());
        return this.expression_RELATION();
    }
    expression_RELATION(): Expression {
        const token = this.current();
        let left = this.expression_CONCATENATION();
        while (true) {
            const op = this.accept(["<", ">", "=", "<=", ">=", "<>"]);
            if (!op) break;
            const right = this.expression_CONCATENATION();
            left = new BinaryOperation(token, this.scope(), new BooleanType(), op.value, left, right);
        }
        return left;
    }

    expression_CONCATENATION(): Expression {
        const parts: Expression[] = [this.expression_ADDING()];
        let opSeen = false;
        let token: Token = this.current();
        while (this.accept("||")) {
            token = this.current();
            parts.push(this.expression_ADDING());
            opSeen = true;
        }
        if (!opSeen) return parts[0];
        const t = parts[0].token;
        return new ConcatenationOperation(token, this.scope(), new StringType(), parts);
    }

    expression_ADDING(): Expression {
        let left = this.expression_MULTIPLYING();
        while (true) {
            const op = this.accept(["+", "-"]);
            if (!op) break;
            const right = this.expression_MULTIPLYING();
            left = new BinaryOperation(op, this.scope(), left.type, op.value, left, right);
        }
        return left;
    }

    expression_MULTIPLYING(): Expression {
        const token = this.current();
        let left = this.expression_FUNCTION_CALL();
        while (true) {
            const op = this.accept(["*", "/", "MOD"]);
            if (!op) break;
            const right = this.expression_FUNCTION_CALL();
            left = new BinaryOperation(token, this.scope(), left.type, op.value, left, right);
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
            const t = this.eat("INTEGER");
            return new IntegerLiteral(t, this.scope(), parseInt(t.value, 10));
        }
        if (token.type === "REAL") {
            const t = this.eat("REAL");
            return new RealLiteral(t, this.scope(), parseFloat(t.value));
        }
        if (token.type === "STRING") {
            const t = this.eat("STRING");
            const context = this.context;
            const existing = Object.values(context.variables).find((v) => v.isConst() && v.zeroValue === t.value);
            if (existing) return new VariableReference(t, "", existing.name, []);
            const const_i = Object.values(context.variables).filter((v) => v.isConst()).length;
            const name = `$${const_i}`;
            const variable = new Variable(t, name, new StringType(), t.value);
            enlist_variable(variable, "");
            return new VariableReference(t, "", name, []);
        }
        if (token.value === "+" || token.value === "-") {
            const t = this.eat(token.value);
            const f = this.factor();
            return new UnaryOperation(t, this.scope(), t.type, t.value, f);
        }
        if (token.value === "(") {
            this.eat("(");
            const e = this.expression();
            this.eat(")");
            return e;
        }
        if (token.value === "TRUE" || token.value === "FALSE") {
            const t = this.eat(token.value);
            return new BoolLiteral(t, this.scope(), t.value === "TRUE");
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
    constructor(t: Token, s: string, type: Type, parts: Expression[]) {
        super(t, s, type);
        this.parts = parts;
    }
    c(): string {
        const fmt: string[] = [];
        const args = this.parts.map((p) => expression_stringer(p, fmt, "||")).join(", ");
        return `$concat("${fmt.join("")}", ${args})`;
    }
}

class PROGRAM extends Node {
    name: string;
    segment: Segment;
    constructor(t: Token, s: string, name: string, seg: Segment) {
        super(t, s);
        this.name = name;
        this.segment = seg;
    }
    c(): string {
        return emit(["int main()", "{", indent(this.segment.c(true), 1), "}"]);
    }
}

// -------------------------- Compiler --------------------------

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

    generate(program: PROGRAM): string {
        const lines: string[] = [
            "#include <stdio.h>",
            "#include <stdlib.h>",
            "#include <string.h>",
            "#include <stdbool.h>",
            '#include "runtime.h"',
            "",
        ];

        if (this.context.common.length) {
            lines.push("// Common declarations");
            for (const declaration of this.context.common) lines.push(declaration);
            lines.push("");
        }

        if (Object.keys(this.context.types).length) {
            lines.push("// Type definitions");
            for (const ty of Object.values(this.context.types)) {
                if (ty instanceof AliasType) continue;
                lines.push(ty.c());
            }
            lines.push("");
        }

        if (Object.keys(this.context.variables).length) {
            lines.push("// Global variables");
            for (const v of Object.values(this.context.variables)) {
                if (v.isConst()) lines.push(v.const() + ";");
                else lines.push(v.c() + ";");
            }
            lines.push("");
        }

        if (Object.keys(this.context.procedures).length) {
            lines.push("// Procedures");
            for (const p of Object.values(this.context.procedures)) {
                lines.push(p.c(), "");
            }
        }

        if (Object.keys(this.context.functions).length) {
            lines.push("// Functions");
            for (const f of Object.values(this.context.functions)) {
                if (f instanceof BuiltinFunction) continue; // don't emit builtins
                lines.push((f as FUNCTION).c(), "");
            }
        }

        lines.push("// Main program");
        lines.push(program.c());

        return emit(lines);
    }
}

// -------------------------- CLI --------------------------

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
    if (firstLine.startsWith("//easy:flags ")) {
        const pairs = firstLine.split(/\s+/).slice(1);
        const flags = Object.fromEntries(pairs.map((p) => p.split("=")));
        Object.assign(context.flags, flags);
    }

    const program = compiler.compile();

    if (argv.includes("-t")) {
        const tokensFile = inputFile.replace(/\.[^.]+$/, "") + ".tokens";
        const lines = context.tokens.map((t) => {
            const input = t.context.text;
            return `${input.filename}:${t.line}:${t.character}\t ${t.value} / ${t.type}`;
        });
        fs.writeFileSync(tokensFile, lines.join("\n") + "\n", "utf-8");
    }

    if (argv.includes("-a")) {
        const astFile = inputFile.replace(/\.[^.]+$/, "") + ".json";
        fs.writeFileSync(astFile, yamlizer(program), "utf-8");
    }

    const outputS = arg(argv, "-s") ?? inputFile.replace(/\.[^.]+$/, "") + ".s";
    {
        const rowsVars: string[][] = [];
        for (const [name, variable] of Object.entries(context.variables)) rowsVars.push(variable.s(name));
        const rowsTypes: string[][] = [];
        for (const [name, type] of Object.entries(context.types)) rowsTypes.push([name, type.c().replace(/\s+/g, " ")]);
        const rowsFns: string[][] = [];
        for (const [name, fun] of Object.entries(context.functions)) {
            if (fun instanceof BuiltinFunction) {
                rowsFns.push([fun.name, "->", fun.type.c(), "built-in"]);
            } else {
                const args = fun.arguments.map((a) => `${a.name} ${a.type.c()}`).join(", ");
                rowsFns.push([fun.name, "->", fun.type.c(), fun.constructor.name, `(${args})`, String(fun.token)]);
            }
        }
        const rowsProcs: string[][] = [];
        for (const [, proc] of Object.entries(context.procedures)) {
            const args = proc.arguments.map((a) => `${a.name} ${a.type.c()}`).join(", ");
            rowsProcs.push([proc.name, proc.constructor.name, `(${args})`, String(proc.token)]);
        }
        const sOut = table(rowsVars) + "\n" + table(rowsTypes) + "\n" + table(rowsFns) + "\n" + table(rowsProcs);
        fs.writeFileSync(outputS, sOut + "\n", "utf-8");
    }

    const outputC = arg(argv, "-c") ?? inputFile.replace(/\.[^.]+$/, "") + ".c";
    const codeC = program.c().trim();

    const chunks: string[] = [];
    chunks.push('#include "runtime.c"\n');
    for (const [name, def] of Object.entries(context.types)) chunks.push(def.typedef(name) + ";\n");
    if (context.common.length) chunks.push(emit(context.common) + "\n");
    for (const v of Object.values(context.variables)) if (v.isConst()) chunks.push(v.const() + ";\n");
    for (const f of Object.values(context.functions)) {
        if (f instanceof BuiltinFunction) continue;
        chunks.push((f as FUNCTION).c() + "\n");
    }
    for (const p of Object.values(context.procedures)) chunks.push(p.c() + "\n");
    chunks.push(codeC + "\n");

    fs.writeFileSync(outputC, chunks.join(""), "utf-8");
}

if (require.main === module) {
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
