#!/usr/bin/env bun

import * as fs from "node:fs";
import * as path from "node:path";
import { fileURLToPath } from "node:url";

const TRACE: boolean = !["", "0", "false", "False", undefined as any].includes(process.env.PEG_TRACE as any);
const _TRACE_RULES_ENV = (process.env.PEG_TRACE_RULES ?? "").trim();
const TRACE_RULES: Set<string> | null = _TRACE_RULES_ENV
    ? new Set(
          _TRACE_RULES_ENV
              .split(",")
              .map((x) => x.trim())
              .filter(Boolean)
      )
    : null;
const TRACE_SNIPPET = 40;

function _trace_enabled(rule: string): boolean {
    if (!TRACE) return false;
    if (!TRACE_RULES) return true;
    return TRACE_RULES.has(rule);
}

function _fmt_loc(text: string, i: number): [number, number, string] {
    const line = text.slice(0, i).split("\n").length;
    const last_nl = text.lastIndexOf("\n", i - 1);
    const start = last_nl === -1 ? 0 : last_nl + 1;
    let end = text.indexOf("\n", i);
    if (end === -1) end = text.length;
    const character = i - start + 1;
    return [line, character, text.slice(start, end)];
}

function _snippet(text: string, i: number, span: number = TRACE_SNIPPET): string {
    const a = Math.max(0, i - Math.floor(span / 2));
    const b = Math.min(text.length, i + Math.floor(span / 2));
    const caret = " ".repeat(i - a) + "^";
    return text.slice(a, b).replace(/\n/g, "\\n") + "\n" + caret;
}

function _tprint(rule: string, when: string, text: string, i: number, extra = "") {
    if (!_trace_enabled(rule)) return;
    const [line, character] = _fmt_loc(text, i);
    console.log(`[${rule}] ${when} @ ${i} (L${line}:C${character}) ${extra}\n${_snippet(text, i)}`);
}

class PEGSyntaxError extends Error {}
class ParseError extends Error {
    position: number;
    line: number;
    character: number;
    context: string;
    details: string[];
    constructor(
        message: string,
        position: number,
        line: number,
        character: number,
        context: string,
        details?: string[]
    ) {
        super(message);
        this.position = position;
        this.line = line;
        this.character = character;
        this.context = context;
        this.details = details || [];
    }
    override toString(): string {
        const parts = [`${super.toString()} at line ${this.line}, character ${this.character}`, this.context];
        if (this.details.length) {
            parts.push("Expected one of:\n  " + Array.from(new Set(this.details)).sort().slice(0, 20).join("\n  "));
        }
        return parts.join("\n");
    }
}

abstract class Expr {}

class Literal extends Expr {
    value: string;
    ignore_case: boolean;
    constructor(value: string, ignore_case = false) {
        super();
        this.value = value;
        this.ignore_case = ignore_case;
    }
}

class CharClass extends Expr {
    ranges: Array<[string, string]>;
    negated: boolean;
    constructor(ranges: Array<[string, string]>, negated = false) {
        super();
        this.ranges = ranges;
        this.negated = negated;
    }
}

class AnyChar extends Expr {}

class RuleRef extends Expr {
    name: string;
    constructor(name: string) {
        super();
        this.name = name;
    }
}

class Sequence extends Expr {
    parts: Expr[];
    constructor(parts: Expr[]) {
        super();
        this.parts = parts;
    }
}

class Choice extends Expr {
    alts: Expr[];
    constructor(alts: Expr[]) {
        super();
        this.alts = alts;
    }
}

class OptionalE extends Expr {
    inner: Expr;
    constructor(inner: Expr) {
        super();
        this.inner = inner;
    }
}

class ZeroOrMore extends Expr {
    inner: Expr;
    constructor(inner: Expr) {
        super();
        this.inner = inner;
    }
}

class OneOrMore extends Expr {
    inner: Expr;
    constructor(inner: Expr) {
        super();
        this.inner = inner;
    }
}

class And extends Expr {
    inner: Expr;
    constructor(inner: Expr) {
        super();
        this.inner = inner;
    }
}

class Not extends Expr {
    inner: Expr;
    constructor(inner: Expr) {
        super();
        this.inner = inner;
    }
}

class Labeled extends Expr {
    label: string;
    inner: Expr;
    constructor(label: string, inner: Expr) {
        super();
        this.label = label;
        this.inner = inner;
    }
}

const _token_ws = /[ \t\r\n]+/y;
const _identifier = /[A-Za-z_][A-Za-z0-9_]*/y;

function _unescape(s: string): string {
    // Handles \n \r \t \" \\ \xHH \uHHHH
    return s.replace(/\\(u[0-9a-fA-F]{4}|x[0-9a-fA-F]{2}|.|$)/g, (_m, g1: string) => {
        if (!g1) return "";
        switch (g1[0]) {
            case "n":
                return "\n";
            case "r":
                return "\r";
            case "t":
                return "\t";
            case "v":
                return "\v";
            case "f":
                return "\f";
            case "b":
                return "\b";
            case "a":
                return "\x07";
            case "u":
                return String.fromCharCode(parseInt(g1.slice(1), 16));
            case "x":
                return String.fromCharCode(parseInt(g1.slice(1), 16));
            default:
                return g1;
        }
    });
}

class _GrammarScanner {
    text: string;
    n: number;
    i: number;
    constructor(text: string) {
        this.text = text;
        this.n = text.length;
        this.i = 0;
    }
    eof(): boolean {
        return this.i >= this.n;
    }
    _skip_ws_and_comments() {
        while (true) {
            _token_ws.lastIndex = this.i;
            const m = _token_ws.exec(this.text);
            if (m) this.i = _token_ws.lastIndex;
            if (this.i < this.n && this.text[this.i] === "#") {
                const nl = this.text.indexOf("\n", this.i);
                this.i = nl === -1 ? this.n : nl + 1;
                continue;
            }
            break;
        }
    }
    peek(s: string): boolean {
        this._skip_ws_and_comments();
        return this.text.startsWith(s, this.i);
    }
    eat(s: string) {
        this._skip_ws_and_comments();
        if (!this.text.startsWith(s, this.i)) {
            throw new PEGSyntaxError(`Expected '${s}' at position ${this.i}`);
        }
        this.i += s.length;
    }
    try_eat(s: string): boolean {
        this._skip_ws_and_comments();
        if (this.text.startsWith(s, this.i)) {
            this.i += s.length;
            return true;
        }
        return false;
    }
    read_identifier(): string {
        this._skip_ws_and_comments();
        _identifier.lastIndex = this.i;
        const m = _identifier.exec(this.text);
        if (!m) throw new PEGSyntaxError(`Identifier expected at position ${this.i}`);
        this.i = _identifier.lastIndex;
        return m[0];
    }
    read_literal(): Literal {
        this._skip_ws_and_comments();
        if (this.i >= this.n || (this.text[this.i] !== "'" && this.text[this.i] !== '"')) {
            throw new PEGSyntaxError(`String literal expected at position ${this.i}`);
        }
        const quote = this.text[this.i];
        this.i += 1;
        const val_chars: string[] = [];
        let escaped = false;
        while (this.i < this.n) {
            const ch = this.text[this.i];
            if (escaped) {
                val_chars.push(ch);
                escaped = false;
            } else {
                if (ch === "\\") {
                    val_chars.push("\\");
                    escaped = true;
                } else if (ch === quote) {
                    this.i += 1;
                    const s = val_chars.join("");
                    let ignore_case = false;
                    if (this.i < this.n && /[iI]/.test(this.text[this.i])) {
                        ignore_case = true;
                        this.i += 1;
                    }
                    return new Literal(_unescape(s), ignore_case);
                } else {
                    val_chars.push(ch);
                }
            }
            this.i += 1;
        }
        throw new PEGSyntaxError("Unterminated string literal");
    }
    read_class(): CharClass {
        this._skip_ws_and_comments();
        if (!this.try_eat("[")) throw new PEGSyntaxError("Expected '['");
        const neg = this.try_eat("^");
        const items: Array<[string, string]> = [];
        while (!this.peek("]")) {
            if (this.i >= this.n) throw new PEGSyntaxError("Unterminated character class");
            let a: string;
            if (this.text[this.i] === "\\" && this.i + 1 < this.n) {
                a = this.text[this.i + 1];
                this.i += 2;
            } else {
                a = this.text[this.i];
                this.i += 1;
            }
            if (this.try_eat("-") && !this.peek("]")) {
                let b: string;
                if (this.text[this.i] === "\\" && this.i + 1 < this.n) {
                    b = this.text[this.i + 1];
                    this.i += 2;
                } else {
                    b = this.text[this.i];
                    this.i += 1;
                }
                items.push([a, b]);
            } else {
                items.push([a, a]);
            }
        }
        this.eat("]");
        return new CharClass(items, neg);
    }
}

class PEG {
    rules: Record<string, Expr>;
    start?: string;
    constructor(rules: Record<string, Expr>, start?: string | null) {
        this.rules = rules;
        this.start = start || Object.keys(rules)[0];
    }
    static parse(text: string): PEG {
        const s = new _GrammarScanner(text);
        const rules: Record<string, Expr> = {};
        const order: string[] = [];
        while (true) {
            s._skip_ws_and_comments();
            if (s.eof()) break;
            const name = s.read_identifier();
            s.eat("<-");
            const expr = PEG._parse_expr(s);
            rules[name] = expr;
            order.push(name);
        }
        return new PEG(rules, order.length ? order[0] : undefined);
    }
    private static _parse_expr(s: _GrammarScanner): Expr {
        const first = PEG._parse_sequence(s);
        const alts = [first];
        while (s.try_eat("/")) {
            alts.push(PEG._parse_sequence(s));
        }
        return alts.length === 1 ? alts[0] : new Choice(alts);
    }
    private static _next_is_new_rule(s: _GrammarScanner): boolean {
        const pos = s.i;
        try {
            s._skip_ws_and_comments();
            _identifier.lastIndex = s.i;
            const m = _identifier.exec(s.text);
            if (!m) return false;
            const j = _identifier.lastIndex;
            s.i = j;
            s._skip_ws_and_comments();
            const is_rule = s.peek("<-");
            s.i = pos;
            return is_rule;
        } finally {
            s.i = pos;
        }
    }
    private static _parse_sequence(s: _GrammarScanner): Expr {
        const parts: Expr[] = [];
        while (true) {
            s._skip_ws_and_comments();
            if (s.eof() || s.peek("/") || s.peek(")") || PEG._next_is_new_rule(s)) break;
            // labelled?
            const save = s.i;
            try {
                const idt = s.read_identifier();
                if (s.try_eat(":")) {
                    const inner = PEG._parse_prefix(s);
                    parts.push(new Labeled(idt, inner));
                    continue;
                } else {
                    s.i = save;
                }
            } catch (_e) {
                s.i = save;
            }
            // normal
            try {
                const p = PEG._parse_prefix(s);
                parts.push(p);
            } catch (_e) {
                break;
            }
        }
        return parts.length === 1 ? parts[0] : new Sequence(parts);
    }
    private static _parse_prefix(s: _GrammarScanner): Expr {
        if (s.try_eat("&")) return new And(PEG._parse_suffix(s));
        if (s.try_eat("!")) return new Not(PEG._parse_suffix(s));
        return PEG._parse_suffix(s);
    }
    private static _parse_suffix(s: _GrammarScanner): Expr {
        const primary = PEG._parse_primary(s);
        if (s.try_eat("?")) return new OptionalE(primary);
        if (s.try_eat("*")) return new ZeroOrMore(primary);
        if (s.try_eat("+")) return new OneOrMore(primary);
        return primary;
    }
    private static _parse_primary(s: _GrammarScanner): Expr {
        s._skip_ws_and_comments();
        if (s.try_eat("(")) {
            const e = PEG._parse_expr(s);
            s.eat(")");
            return e;
        }
        if (s.try_eat(".")) return new AnyChar();
        if (s.peek("'") || s.peek('"')) return s.read_literal();
        if (s.peek("[")) return s.read_class();
        const name = s.read_identifier();
        return new RuleRef(name);
    }
}

type MemoVal = [boolean, number, any];

class _Runtime {
    g: PEG;
    text: string;
    n: number;
    memo: Map<string, MemoVal>;
    ws_re: RegExp;

    farthest: number;
    expected: string[];

    constructor(grammar: PEG, text: string) {
        this.g = grammar;
        this.text = text;
        this.n = text.length;
        this.memo = new Map();
        this.ws_re = /[ \t\r\n]+/y;

        this.farthest = 0;
        this.expected = [];
    }

    private _note_fail(i: number, expected: string) {
        if (i > this.farthest) {
            this.farthest = i;
            this.expected = [expected];
        } else if (i === this.farthest) {
            this.expected.push(expected);
        }
    }

    _skip_ws_COMMENTLESS(i: number): number {
        this.ws_re.lastIndex = i;
        const m = this.ws_re.exec(this.text);
        return m ? this.ws_re.lastIndex : i;
    }

    private _skip_ws(i: number): number {
        while (i < this.n) {
            this.ws_re.lastIndex = i;
            const m = this.ws_re.exec(this.text);
            if (m) {
                i = this.ws_re.lastIndex;
                continue;
            }
            if (this.text.startsWith("/*", i)) {
                const j = this.text.indexOf("*/", i + 2);
                if (j === -1) return this.n;
                i = j + 2;
                continue;
            }
            if (this.text.startsWith("//", i)) {
                const j = this.text.indexOf("\n", i + 2);
                if (j === -1) return this.n;
                i = j + 1;
                continue;
            }
            break;
        }
        return i;
    }

    private _loc(i: number): [number, number, string] {
        const line = this.text.slice(0, i).split("\n").length;
        const last_nl = this.text.lastIndexOf("\n", i - 1);
        const start = last_nl === -1 ? 0 : last_nl + 1;
        let end = this.text.indexOf("\n", i);
        if (end === -1) end = this.text.length;
        const character = i - start + 1;
        return [line, character, this.text.slice(start, end)];
    }

    parse(start_rule?: string | null): any {
        const start = start_rule || this.g.start!;
        let [ok, j, val] = this._apply_rule(start, 0);
        j = this._skip_ws(j);
        if (!ok || j !== this.n) {
            const i = Math.max(j, this.farthest);
            const [line, character, ctx] = this._loc(i);
            throw new ParseError(`Parse failed (stopped at rule '${start}')`, i, line, character, ctx, this.expected);
        }
        return val;
    }

    private _apply_rule(name: string, i: number): MemoVal {
        const key = `${name}@${i}`;
        const memoed = this.memo.get(key);
        if (memoed) return memoed;
        _tprint(name, "▶ enter", this.text, i);
        const expr = this.g.rules[name];
        let [ok, j, val] = this._eval(expr, this._skip_ws(i)); // skip WS on rule entry
        if (ok) {
            if (val && typeof val === "object" && !Array.isArray(val) && !("type" in val)) {
                val = { type: name, ...val };
            }
            _tprint(name, `✔ ok -> ${j}`, this.text, j);
        } else {
            _tprint(name, "✘ fail", this.text, i);
        }
        const out: MemoVal = [ok, j, val];
        this.memo.set(key, out);
        return out;
    }

    private _should_skip_between(prev: Expr, nxt: Expr | null): boolean {
        if (nxt == null) return false;
        if (prev instanceof And || prev instanceof Not) return false;

        if (prev instanceof CharClass && nxt instanceof ZeroOrMore && nxt.inner instanceof CharClass) return false;
        if (prev instanceof CharClass && nxt instanceof OneOrMore && nxt.inner instanceof CharClass) return false;
        if (prev instanceof CharClass && nxt instanceof CharClass) return false;

        // Important for strings: do not skip between quotes and content.
        if (prev instanceof Literal && (prev.value === '"' || prev.value === "'" || prev.value === "\\")) return false;
        if (nxt instanceof Literal && (nxt.value === '"' || nxt.value === "'")) return false;

        if (prev instanceof Literal && nxt instanceof Literal && prev.value.length === 1 && nxt.value.length === 1) {
            return false;
        }

        return true;
    }

    private _merge_value(fields: Record<string, any>, v: any) {
        // Merge dicts and lists-of-dicts into parent 'fields', but never bubble up child 'type'.
        if (v && typeof v === "object" && !Array.isArray(v)) {
            for (const [k, val] of Object.entries(v)) {
                if (k === "type") continue;
                if (k in fields) {
                    if (!Array.isArray(fields[k])) fields[k] = [fields[k]];
                    (fields[k] as any[]).push(val);
                } else {
                    fields[k] = val;
                }
            }
        } else if (Array.isArray(v)) {
            for (const item of v) this._merge_value(fields, item);
        }
    }

    private _eval(e: Expr, i: number): MemoVal {
        if (e instanceof Literal) {
            const L = e.value.length;
            const seg = this.text.slice(i, i + L);
            if (e.ignore_case) {
                if (seg.toLowerCase() === e.value.toLowerCase()) {
                    const ret = /^[A-Za-z]+$/.test(e.value) ? e.value.toUpperCase() : e.value;
                    return [true, i + L, ret];
                }
                this._note_fail(i, `'${e.value}'`);
                return [false, i, null];
            } else {
                if (seg === e.value) return [true, i + L, e.value];
                this._note_fail(i, `'${e.value}'`);
                return [false, i, null];
            }
        }

        if (e instanceof CharClass) {
            if (i >= this.n) {
                this._note_fail(i, "character class");
                return [false, i, null];
            }
            const ch = this.text[i];
            const in_class = (c: string): boolean => {
                for (const [a, b] of e.ranges) {
                    if (a.charCodeAt(0) <= c.charCodeAt(0) && c.charCodeAt(0) <= b.charCodeAt(0)) return true;
                }
                return false;
            };
            let ok = in_class(ch);
            ok = e.negated ? !ok : ok;
            if (ok) return [true, i + 1, ch];
            this._note_fail(i, "character class");
            return [false, i, null];
        }

        if (e instanceof AnyChar) {
            if (i < this.n) return [true, i + 1, this.text[i]];
            this._note_fail(i, "any character");
            return [false, i, null];
        }

        if (e instanceof RuleRef) {
            return this._apply_rule(e.name, i);
        }

        if (e instanceof Sequence) {
            let j = i;
            const fields: Record<string, any> = {};
            const parts = e.parts;
            for (let idx = 0; idx < parts.length; idx++) {
                const p = parts[idx];
                const [ok, j2, v] = this._eval(p, j);
                if (!ok) return [false, i, null];
                j = j2;
                this._merge_value(fields, v);
                const nxt = idx + 1 < parts.length ? parts[idx + 1] : null;
                if (this._should_skip_between(p, nxt)) j = this._skip_ws(j);
            }
            return [true, j, Object.keys(fields).length ? fields : null];
        }

        if (e instanceof Choice) {
            let far_i = i;
            let far_exp: string[] = [];
            for (const a of e.alts) {
                const [ok, j, v] = this._eval(a, i);
                if (ok) return [true, j, v];
                if (this.farthest > far_i) {
                    far_i = this.farthest;
                    far_exp = [...this.expected];
                }
            }
            if (far_i > this.farthest) {
                this.farthest = far_i;
                this.expected = far_exp;
            }
            return [false, i, null];
        }

        if (e instanceof OptionalE) {
            const [ok, j, v] = this._eval(e.inner, i);
            if (ok) return [true, j, v];
            return [true, i, null];
        }

        if (e instanceof ZeroOrMore) {
            let j = i;
            const items: any[] = [];
            while (true) {
                const [ok, j2, v] = this._eval(e.inner, j);
                if (!ok) break;
                j = j2;
                if (v !== null && v !== undefined) items.push(v);
            }
            if (e.inner instanceof CharClass) {
                const s = items.join("");
                return [true, j, s ? s : null];
            }
            return [true, j, items.length ? items : null];
        }

        if (e instanceof OneOrMore) {
            let j = i;
            const items: any[] = [];
            const [ok1, j1, v1] = this._eval(e.inner, j);
            if (!ok1) return [false, i, null];
            j = j1;
            if (v1 !== null && v1 !== undefined) items.push(v1);
            while (true) {
                const [ok, j2, v] = this._eval(e.inner, j);
                if (!ok) break;
                j = j2;
                if (v !== null && v !== undefined) items.push(v);
            }
            if (e.inner instanceof CharClass) {
                return [true, j, items.join("")];
            }
            return [true, j, items];
        }

        if (e instanceof And) {
            const [ok] = this._eval(e.inner, i);
            return ok ? [true, i, null] : [false, i, null];
        }

        if (e instanceof Not) {
            const [ok] = this._eval(e.inner, i);
            return ok ? [false, i, null] : [true, i, null];
        }

        if (e instanceof Labeled) {
            const [ok, j, v] = this._eval(e.inner, i);
            if (!ok) return [false, i, null];
            let value = v;
            if (
                (value === null || value === undefined) &&
                j > i &&
                !(e.inner instanceof ZeroOrMore) &&
                !(e.inner instanceof OneOrMore) &&
                !(e.inner instanceof OptionalE)
            ) {
                value = this.text.slice(i, j).trim();
            }
            return [true, j, { [e.label]: value }];
        }

        throw new PEGSyntaxError(`Unknown expr ${(e as any)?.constructor?.name ?? e}`);
    }
}

function _simplify_ast(node: any): any {
    if (Array.isArray(node)) {
        const out = node.map((x) => _simplify_ast(x));
        return out.filter((x) => x !== null && x !== undefined);
    }
    if (!(node && typeof node === "object")) return node;

    const simp: Record<string, any> = {};
    for (const [k, v] of Object.entries(node)) simp[k] = _simplify_ast(v);

    for (const k of Object.keys(simp)) {
        const v = simp[k];
        const isEmptyObj = v && typeof v === "object" && !Array.isArray(v) && Object.keys(v).length === 0;
        if (v === null || v === "" || (Array.isArray(v) && v.length === 0) || isEmptyObj) {
            delete simp[k];
        }
    }

    const t = simp["type"];

    // normalize comma-lists to plain lists
    function _concat_head_tail(s: any): any[] {
        const items: any[] = [];
        if ("head" in s) items.push(s["head"]);
        const tail = s["tail"];
        if (Array.isArray(tail)) items.push(...tail);
        else if (tail !== undefined) items.push(tail);
        return items;
    }

    if (t && new Set(["output_list", "input_list", "declared_names", "argument_list"]).has(t)) {
        return _concat_head_tail(simp);
    }

    // collapse trivial expression wrappers
    const collapse_if_single = new Set([
        "xor_expr",
        "or_expr",
        "and_expr",
        "rel_expr",
        "concat_expr",
        "add_expr",
        "mul_expr",
        "postfix_expr",
    ]);
    if (t && collapse_if_single.has(t)) {
        const keys = Object.keys(simp).filter((k) => k !== "type");
        if (keys.length === 1) return simp[keys[0]];
    }

    if (t === "unary_expr") {
        if ("sign" in simp) {
            return { type: "unary", op: simp["sign"], expr: simp["expr"] };
        }
        return simp["expr"];
    }

    if (t === "primary") {
        for (const k of ["function_reference", "variable", "constant", "inner"]) {
            if (k in simp) return simp[k];
        }
    }

    if (t === "variable") {
        // Idempotence: if it's already the final form (base + parts only), keep it.
        if ("parts" in simp && !("fields" in simp) && !("indexes" in simp)) return simp;

        const base = simp["base"];
        const parts_in = simp["parts"];
        const parts_out: Array<Record<string, any>> = [];
        if (Array.isArray(parts_in)) {
            for (const part of parts_in) {
                if (part && typeof part === "object") {
                    if ("field" in part) parts_out.push({ field: part["field"] });
                    else if ("index" in part) parts_out.push({ index: part["index"] });
                }
            }
        }
        return { type: "variable", base, parts: parts_out };
    }

    return simp;
}

class PEGParser {
    grammar: PEG;
    constructor(grammar_text: string, start?: string | null) {
        const peg = PEG.parse(grammar_text);
        this.grammar = peg;
        if (start) this.grammar.start = start;
    }
    parse(text: string, start_rule?: string | null): any {
        const runtime = new _Runtime(this.grammar, text);
        const ast = runtime.parse(start_rule);
        return _simplify_ast(ast);
    }
}

const TRIVIA = `
PROGRAM Test:
  OUTPUT CHARACTER (48);
END PROGRAM Test;
`;

function main(argv: string[]) {
    const thisFile = fileURLToPath(import.meta.url);
    const thisDir = path.dirname(thisFile);
    const grammarPath = path.join(thisDir, "easy.peg");
    const grammar = fs.readFileSync(grammarPath, "utf-8");

    const parser = new PEGParser(grammar, "compilation");

    const self_test = argv.length < 3;
    const code = self_test ? TRIVIA : fs.readFileSync(argv[2], "utf-8");
    const ast = parser.parse(code);

    const ast_text = JSON.stringify(ast, null, 2);

    if (self_test) {
        console.log(ast_text);
    } else {
        const inputFile = argv[2];
        const outFile = inputFile.replace(/\.[^.]+$/, "") + ".peg.json";
        fs.writeFileSync(outFile, ast_text);
    }
}

if (import.meta.main) {
    try {
        main(process.argv);
    } catch (e: any) {
        console.error(e?.stack || String(e));
        process.exit(2);
    }
}
