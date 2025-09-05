# Generic PEG interpreter (packrat) with left recursion detection and labeled AST.
# Includes: TRACE, whitespace handling fixes, list-merge, and AST simplifier that
# preserves variable suffixes as fields/indexes and flattens comma lists.

import os
import re
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Set, Tuple

# ---------- TRACE CONFIG ----------
TRACE = os.getenv("PEG_TRACE", "") not in ("", "0", "false", "False")
# e.g. PEG_TRACE_RULES=output_statement,output_list,expression,primary,variable
_TRACE_RULES_ENV = os.getenv("PEG_TRACE_RULES", "").strip()
TRACE_RULES: Optional[Set[str]] = (
    set(x.strip() for x in _TRACE_RULES_ENV.split(",") if x.strip()) if _TRACE_RULES_ENV else None
)
TRACE_SNIPPET = 40


def _trace_enabled(rule: str) -> bool:
    if not TRACE:
        return False
    if not TRACE_RULES:
        return True
    return rule in TRACE_RULES


def _fmt_loc(text: str, i: int) -> Tuple[int, int, str]:
    line = text.count("\n", 0, i) + 1
    last_nl = text.rfind("\n", 0, i)
    start = 0 if last_nl == -1 else last_nl + 1
    end = text.find("\n", i)
    if end == -1:
        end = len(text)
    col = i - start + 1
    return line, col, text[start:end]


def _snippet(text: str, i: int, span: int = TRACE_SNIPPET) -> str:
    a = max(0, i - span // 2)
    b = min(len(text), i + span // 2)
    caret = " " * (i - a) + "^"
    return text[a:b].replace("\n", "\\n") + "\n" + caret


def _tprint(rule: str, when: str, text: str, i: int, extra: str = ""):
    if not _trace_enabled(rule):
        return
    line, col, _ = _fmt_loc(text, i)
    print(f"[{rule}] {when} @ {i} (L{line}:C{col}) {extra}\n{_snippet(text, i)}")


# -------------------------------
# Errors
# -------------------------------


class PEGSyntaxError(Exception):
    pass


class ParseError(Exception):
    def __init__(self, message, position, line, col, context, details=None):
        super().__init__(message)
        self.position = position
        self.line = line
        self.col = col
        self.context = context
        self.details = details or []

    def __str__(self):
        parts = [f"{super().__str__()} at line {self.line}, col {self.col}", self.context]
        if self.details:
            parts.append("Expected one of:\n  " + "\n  ".join(sorted(set(self.details))[:20]))
        return "\n".join(parts)


class LeftRecursionError(Exception):
    pass


# -------------------------------
# Grammar AST nodes
# -------------------------------


@dataclass
class Expr:  #
    ...


@dataclass
class Literal(Expr):
    value: str
    ignore_case: bool = False


@dataclass
class CharClass(Expr):
    ranges: List[Tuple[str, str]]
    negated: bool = False


@dataclass
class AnyChar(Expr):  #
    ...


@dataclass
class RuleRef(Expr):
    name: str


@dataclass
class Sequence(Expr):
    parts: List[Expr]


@dataclass
class Choice(Expr):
    alts: List[Expr]


@dataclass
class OptionalE(Expr):
    inner: Expr


@dataclass
class ZeroOrMore(Expr):
    inner: Expr


@dataclass
class OneOrMore(Expr):
    inner: Expr


@dataclass
class And(Expr):
    inner: Expr


@dataclass
class Not(Expr):
    inner: Expr


@dataclass
class Labeled(Expr):
    label: str
    inner: Expr


# -------------------------------
# PEG grammar parser (for the PEG text)
# -------------------------------

_token_ws = re.compile(r"[ \t\r\n]+")
_identifier = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")


def _unescape(s: str) -> str:
    return bytes(s, "utf-8").decode("unicode_escape")


class _GrammarScanner:
    def __init__(self, text: str):
        self.text = text
        self.n = len(text)
        self.i = 0

    def eof(self):
        return self.i >= self.n

    def _skip_ws_and_comments(self):
        while True:
            m = _token_ws.match(self.text, self.i)
            if m:
                self.i = m.end()
            if self.i < self.n and self.text[self.i] == "#":
                nl = self.text.find("\n", self.i)
                self.i = self.n if nl == -1 else nl + 1
                continue
            break

    def peek(self, s: str) -> bool:
        self._skip_ws_and_comments()
        return self.text.startswith(s, self.i)

    def eat(self, s: str):
        self._skip_ws_and_comments()
        if not self.text.startswith(s, self.i):
            raise PEGSyntaxError(f"Expected '{s}' at position {self.i}")
        self.i += len(s)

    def try_eat(self, s: str) -> bool:
        self._skip_ws_and_comments()
        if self.text.startswith(s, self.i):
            self.i += len(s)
            return True
        return False

    def read_identifier(self) -> str:
        self._skip_ws_and_comments()
        m = _identifier.match(self.text, self.i)
        if not m:
            raise PEGSyntaxError(f"Identifier expected at position {self.i}")
        self.i = m.end()
        return m.group(0)

    def read_literal(self) -> Literal:
        self._skip_ws_and_comments()
        if self.i >= self.n or self.text[self.i] not in ("'", '"'):
            raise PEGSyntaxError(f"String literal expected at position {self.i}")
        quote = self.text[self.i]
        self.i += 1
        val_chars = []
        escaped = False
        while self.i < self.n:
            ch = self.text[self.i]
            if escaped:
                val_chars.append(ch)
                escaped = False
            else:
                if ch == "\\":
                    val_chars.append("\\")
                    escaped = True
                elif ch == quote:
                    self.i += 1
                    s = "".join(val_chars)
                    ignore_case = False
                    if self.i < self.n and self.text[self.i] in "iI":
                        ignore_case = True
                        self.i += 1
                    return Literal(_unescape(s), ignore_case)
                else:
                    val_chars.append(ch)
            self.i += 1
        raise PEGSyntaxError("Unterminated string literal")

    def read_class(self) -> CharClass:
        self._skip_ws_and_comments()
        if not self.try_eat("["):
            raise PEGSyntaxError("Expected '['")
        neg = self.try_eat("^")
        items = []
        while not self.peek("]"):
            if self.i >= self.n:
                raise PEGSyntaxError("Unterminated character class")
            if self.text[self.i] == "\\" and self.i + 1 < self.n:
                a = self.text[self.i + 1]
                self.i += 2
            else:
                a = self.text[self.i]
                self.i += 1
            if self.try_eat("-") and not self.peek("]"):
                if self.text[self.i] == "\\" and self.i + 1 < self.n:
                    b = self.text[self.i + 1]
                    self.i += 2
                else:
                    b = self.text[self.i]
                    self.i += 1
                items.append((a, b))
            else:
                items.append((a, a))
        self.eat("]")
        return CharClass(items, neg)


class PEG:
    def __init__(self, rules: Dict[str, Expr], start: Optional[str] = None):
        self.rules = rules
        self.start = start or next(iter(rules.keys()))

    @staticmethod
    def parse(text: str) -> "PEG":
        s = _GrammarScanner(text)
        rules: Dict[str, Expr] = {}
        order: List[str] = []
        while True:
            s._skip_ws_and_comments()
            if s.eof():
                break
            name = s.read_identifier()
            s.eat("<-")
            expr = PEG._parse_expr(s)
            rules[name] = expr
            order.append(name)
        return PEG(rules, start=order[0] if order else None)

    @staticmethod
    def _parse_expr(s: _GrammarScanner) -> Expr:
        first = PEG._parse_sequence(s)
        alts = [first]
        while s.try_eat("/"):
            alts.append(PEG._parse_sequence(s))
        return alts[0] if len(alts) == 1 else Choice(alts)

    @staticmethod
    def _next_is_new_rule(s: _GrammarScanner) -> bool:
        pos = s.i
        try:
            s._skip_ws_and_comments()
            m = _identifier.match(s.text, s.i)
            if not m:
                return False
            j = m.end()
            s.i = j
            s._skip_ws_and_comments()
            is_rule = s.peek("<-")
            s.i = pos
            return is_rule
        finally:
            s.i = pos

    @staticmethod
    def _parse_sequence(s: _GrammarScanner) -> Expr:
        parts: List[Expr] = []
        while True:
            s._skip_ws_and_comments()
            if s.eof() or s.peek("/") or s.peek(")") or PEG._next_is_new_rule(s):
                break
            # labelled?
            save = s.i
            try:
                idt = s.read_identifier()
                if s.try_eat(":"):
                    inner = PEG._parse_prefix(s)
                    parts.append(Labeled(idt, inner))
                    continue
                else:
                    s.i = save
            except PEGSyntaxError:
                s.i = save
            # normal
            try:
                p = PEG._parse_prefix(s)
            except PEGSyntaxError:
                break
            parts.append(p)
        return parts[0] if len(parts) == 1 else Sequence(parts)

    @staticmethod
    def _parse_prefix(s: _GrammarScanner) -> Expr:
        if s.try_eat("&"):
            return And(PEG._parse_suffix(s))
        if s.try_eat("!"):
            return Not(PEG._parse_suffix(s))
        return PEG._parse_suffix(s)

    @staticmethod
    def _parse_suffix(s: _GrammarScanner) -> Expr:
        primary = PEG._parse_primary(s)
        if s.try_eat("?"):
            return OptionalE(primary)
        if s.try_eat("*"):
            return ZeroOrMore(primary)
        if s.try_eat("+"):
            return OneOrMore(primary)
        return primary

    @staticmethod
    def _parse_primary(s: _GrammarScanner) -> Expr:
        s._skip_ws_and_comments()
        if s.try_eat("("):
            e = PEG._parse_expr(s)
            s.eat(")")
            return e
        if s.try_eat("."):
            return AnyChar()
        if s.peek("'") or s.peek('"'):
            return s.read_literal()
        if s.peek("["):
            return s.read_class()
        name = s.read_identifier()
        return RuleRef(name)


# -------------------------------
# Left recursion analysis
# -------------------------------


class _Analyzer:
    def __init__(self, rules: Dict[str, Expr]):
        self.rules = rules
        self.nullable: Dict[str, bool] = {name: False for name in rules}
        self.first_rules: Dict[str, Set[str]] = {name: set() for name in rules}
        self._compute()

    def _expr_nullable(self, e: Expr) -> bool:
        if isinstance(e, Literal):
            return len(e.value) == 0
        if isinstance(e, CharClass):
            return False
        if isinstance(e, AnyChar):
            return False
        if isinstance(e, RuleRef):
            return self.nullable[e.name]
        if isinstance(e, Sequence):
            return all(self._expr_nullable(p) for p in e.parts)
        if isinstance(e, Choice):
            return any(self._expr_nullable(a) for a in e.alts)
        if isinstance(e, OptionalE):
            return True
        if isinstance(e, ZeroOrMore):
            return True
        if isinstance(e, OneOrMore):
            return self._expr_nullable(e.inner)
        if isinstance(e, And):
            return True
        if isinstance(e, Not):
            return True
        if isinstance(e, Labeled):
            return self._expr_nullable(e.inner)
        return False

    def _first_rules_expr(self, e: Expr) -> Set[str]:
        S: Set[str] = set()
        if isinstance(e, RuleRef):
            S.add(e.name)
        elif isinstance(e, Sequence):
            for p in e.parts:
                S |= self._first_rules_expr(p)
                if not self._expr_nullable(p):
                    break
        elif isinstance(e, Choice):
            for a in e.alts:
                S |= self._first_rules_expr(a)
        elif isinstance(e, (OptionalE, ZeroOrMore, OneOrMore, And, Not, Labeled)):
            S |= self._first_rules_expr(e.inner)
        return S

    def _compute(self):
        changed = True
        while changed:
            changed = False
            for name, expr in self.rules.items():
                val = self._expr_nullable(expr)
                if val != self.nullable[name]:
                    self.nullable[name] = val
                    changed = True
        changed = True
        while changed:
            changed = False
            for name, expr in self.rules.items():
                fr = set(self._first_rules_expr(expr))
                if not fr.issubset(self.first_rules[name]):
                    self.first_rules[name] |= fr
                    changed = True

    def detect_left_recursion(self) -> List[List[str]]:
        edges = {a: set(bs) for a, bs in self.first_rules.items()}
        cycles: List[List[str]] = []
        visiting, visited = set(), set()
        stack: List[str] = []

        def dfs(u: str):
            visiting.add(u)
            stack.append(u)
            for v in edges.get(u, ()):
                if v in visiting:
                    if v in stack:
                        idx = stack.index(v)
                        cyc = stack[idx:] + [v]
                        if cyc not in cycles:
                            cycles.append(cyc)
                elif v not in visited:
                    dfs(v)
            stack.pop()
            visiting.remove(u)
            visited.add(u)

        for r in self.rules:
            if r not in visited:
                dfs(r)
        pruned = []
        for cyc in cycles:
            if len(cyc) >= 2 and cyc[0] == cyc[-1]:
                pruned.append(cyc)
        return pruned


# -------------------------------
# Runtime parser (packrat)
# -------------------------------


class _Runtime:
    def __init__(self, grammar: PEG, text: str):
        self.g = grammar
        self.text = text
        self.n = len(text)
        self.memo: Dict[Tuple[str, int], Tuple[bool, int, Any]] = {}
        self.ws_re = re.compile(r"[ \t\r\n]+")

        self.farthest = 0
        self.expected: List[str] = []

    def _note_fail(self, i: int, expected: str):
        if i > self.farthest:
            self.farthest = i
            self.expected = [expected]
        elif i == self.farthest:
            self.expected.append(expected)

    def _skip_ws_COMMENTLESS(self, i: int) -> int:
        m = self.ws_re.match(self.text, i)
        return m.end() if m else i

    def _skip_ws(self, i: int) -> int:
        while i < self.n:
            # skip spaces/tabs/newlines
            m = self.ws_re.match(self.text, i)
            if m:
                i = m.end()
                continue
            # C-style block comment
            if self.text.startswith("/*", i):
                j = self.text.find("*/", i + 2)
                if j == -1:
                    # unterminated comment: skip to EOF
                    return self.n
                i = j + 2
                continue
            # C++-style line comment
            if self.text.startswith("//", i):
                j = self.text.find("\n", i + 2)
                if j == -1:
                    return self.n
                i = j + 1
                continue
            break
        return i

    def _loc(self, i: int) -> Tuple[int, int, str]:
        line = self.text.count("\n", 0, i) + 1
        last_nl = self.text.rfind("\n", 0, i)
        start = 0 if last_nl == -1 else last_nl + 1
        end = self.text.find("\n", i)
        end = len(self.text) if end == -1 else end
        col = i - start + 1
        return line, col, self.text[start:end]

    def parse(self, start_rule: Optional[str] = None) -> Any:
        start = start_rule or self.g.start
        ok, j, val = self._apply_rule(start, 0)
        j = self._skip_ws(j)
        if not ok or j != self.n:
            i = max(j, self.farthest)
            line, col, ctx = self._loc(i)
            raise ParseError(f"Parse failed (stopped at rule '{start}')", i, line, col, ctx, self.expected)
        return val

    def _apply_rule(self, name: str, i: int) -> Tuple[bool, int, Any]:
        key = (name, i)
        if key in self.memo:
            return self.memo[key]
        _tprint(name, "▶ enter", self.text, i)
        expr = self.g.rules[name]
        ok, j, val = self._eval(expr, self._skip_ws(i))  # skip WS on rule entry
        if ok:
            if isinstance(val, dict) and "type" not in val:
                val = {"type": name, **val}
            _tprint(name, f"✔ ok -> {j}", self.text, j)
        else:
            _tprint(name, "✘ fail", self.text, i)
        self.memo[key] = (ok, j, val)
        return ok, j, val

    def _should_skip_between(self, prev, nxt):
        # (existing) do not skip after last part
        if nxt is None:
            return False

        # (already added earlier) do not skip after lookaheads (e.g., !'"' .)
        if isinstance(prev, (And, Not)):
            return False

        # (existing) charclass-to-charclass guards ...
        if (
            isinstance(prev, CharClass)
            and isinstance(nxt, (ZeroOrMore, OneOrMore))
            and isinstance(nxt.inner, CharClass)
        ):
            return False
        if isinstance(prev, CharClass) and isinstance(nxt, CharClass):
            return False

        # *** Important for strings ***
        # Do not skip after a quote or backslash literal (", ', \)
        if isinstance(prev, Literal) and prev.value in ('"', "'", "\\"):
            return False
        # Do not skip just before a closing quote literal
        if isinstance(nxt, Literal) and nxt.value in ('"', "'"):
            return False

        # Extra-safe: prevent skipping between two 1-char literals (covers '""' and "''")
        if isinstance(prev, Literal) and isinstance(nxt, Literal) and len(prev.value) == 1 and len(nxt.value) == 1:
            return False

        return True

    def _merge_value(self, fields: Dict[str, Any], v: Any):
        # Merge dicts and lists-of-dicts into parent 'fields', but never bubble up child 'type'.
        if isinstance(v, dict):
            for k, val in v.items():
                if k == "type":
                    continue  # avoid 'type' list pollution at parent
                if k in fields:
                    if not isinstance(fields[k], list):
                        fields[k] = [fields[k]]
                    fields[k].append(val)
                else:
                    fields[k] = val
        elif isinstance(v, list):
            for item in v:
                self._merge_value(fields, item)

    def _eval(self, e: Expr, i: int) -> Tuple[bool, int, Any]:
        if isinstance(e, Literal):
            L = len(e.value)
            seg = self.text[i : i + L]
            if e.ignore_case:
                if seg.lower() == e.value.lower():
                    return True, i + L, e.value.upper() if e.value.isalpha() else e.value
                self._note_fail(i, f"'{e.value}'")
                return False, i, None
            else:
                if seg == e.value:
                    return True, i + L, e.value
                self._note_fail(i, f"'{e.value}'")
                return False, i, None

        if isinstance(e, CharClass):
            if i >= self.n:
                self._note_fail(i, "character class")
                return False, i, None
            ch = self.text[i]

            def in_class(ch: str) -> bool:
                for a, b in e.ranges:
                    if ord(a) <= ord(ch) <= ord(b):
                        return True
                return False

            ok = in_class(ch)
            ok = not ok if e.negated else ok
            if ok:
                return True, i + 1, ch
            self._note_fail(i, "character class")
            return False, i, None

        if isinstance(e, AnyChar):
            if i < self.n:
                return True, i + 1, self.text[i]
            self._note_fail(i, "any character")
            return False, i, None

        if isinstance(e, RuleRef):
            return self._apply_rule(e.name, i)

        if isinstance(e, Sequence):
            j = i
            fields: Dict[str, Any] = {}
            parts = e.parts
            for idx, p in enumerate(parts):
                ok, j2, v = self._eval(p, j)
                if not ok:
                    return False, i, None
                j = j2
                self._merge_value(fields, v)
                nxt = parts[idx + 1] if idx + 1 < len(parts) else None
                if self._should_skip_between(p, nxt):
                    j = self._skip_ws(j)
            return True, j, fields if fields else None

        if isinstance(e, Choice):
            far_i = i
            far_exp: List[str] = []
            for a in e.alts:
                ok, j, v = self._eval(a, i)
                if ok:
                    return True, j, v
                if self.farthest > far_i:
                    far_i = self.farthest
                    far_exp = list(self.expected)
            if far_i > self.farthest:
                self.farthest = far_i
                self.expected = far_exp
            return False, i, None

        if isinstance(e, OptionalE):
            ok, j, v = self._eval(e.inner, i)
            if ok:
                return True, j, v
            return True, i, None

        if isinstance(e, ZeroOrMore):
            j = i
            items = []
            while True:
                ok, j2, v = self._eval(e.inner, j)
                if not ok:
                    break
                j = j2
                if v is not None:
                    items.append(v)
            # NEW: if repeating a CharClass, join into one string
            if isinstance(e.inner, CharClass):
                s = "".join(items)
                return True, j, (s if s else None)
            return True, j, (items if items else None)

        if isinstance(e, OneOrMore):
            j = i
            items = []
            ok1, j, v = self._eval(e.inner, j)
            if not ok1:
                return False, i, None
            if v is not None:
                items.append(v)
            while True:
                ok, j2, v = self._eval(e.inner, j)
                if not ok:
                    break
                j = j2
                if v is not None:
                    items.append(v)
            # NEW: if repeating a CharClass, join into one string
            if isinstance(e.inner, CharClass):
                return True, j, "".join(items)
            return True, j, items

        if isinstance(e, And):
            ok, _, _ = self._eval(e.inner, i)
            return (True, i, None) if ok else (False, i, None)

        if isinstance(e, Not):
            ok, _, _ = self._eval(e.inner, i)
            return (False, i, None) if ok else (True, i, None)

        if isinstance(e, Labeled):
            ok, j, v = self._eval(e.inner, i)
            if not ok:
                return False, i, None
            # Only synthesize substring when something was consumed AND the inner
            # isn’t a repetition/option wrapper. Also trim spaces.
            if v is None and j > i and not isinstance(e.inner, (ZeroOrMore, OneOrMore, OptionalE)):
                v = self.text[i:j].strip()
            return True, j, {e.label: v}

        raise PEGSyntaxError(f"Unknown expr {e}")


# -------------------------------
# AST simplifier
# -------------------------------


def _simplify_ast(node):
    if isinstance(node, list):
        out = [_simplify_ast(x) for x in node]
        return [x for x in out if x is not None]
    if not isinstance(node, dict):
        return node

    # simplify children first
    simp = {k: _simplify_ast(v) for k, v in node.items()}

    # drop empty
    for k in list(simp.keys()):
        v = simp[k]
        if v in (None, "", [], {}):
            del simp[k]

    t = simp.get("type")

    # normalize comma-lists to plain lists
    def _concat_head_tail(s):
        items = []
        if "head" in s:
            items.append(s["head"])
        tail = s.get("tail")
        if isinstance(tail, list):
            items.extend(tail)
        elif tail is not None:
            items.append(tail)
        return items

    if t in {"output_list", "input_list", "declared_names", "actual_argument_list"}:
        return _concat_head_tail(simp)

    # collapse trivial expression wrappers
    collapse_if_single = {
        "xor_expr",
        "or_expr",
        "and_expr",
        "rel_expr",
        "concat_expr",
        "add_expr",
        "mul_expr",
        "postfix_expr",
    }
    if t in collapse_if_single:
        keys = [k for k in simp.keys() if k != "type"]
        if len(keys) == 1:
            return simp[keys[0]]

    if t == "unary_expr":
        if "sign" in simp:
            return {"type": "unary", "op": simp["sign"], "expr": simp.get("expr")}
        return simp.get("expr")

    if t == "primary":
        for k in ("function_reference", "variable", "constant", "inner"):
            if k in simp:
                return simp[k]

    if t == "variable":
        # Idempotence: if it's already the final form (base + parts only), keep it.
        if "parts" in simp and ("fields" not in simp and "indexes" not in simp):
            return simp

        base = simp.get("base")
        parts_in = simp.get("parts")

        parts_out: List[dict] = []
        if isinstance(parts_in, list):
            for part in parts_in:
                if isinstance(part, dict):
                    if "field" in part:
                        parts_out.append({"field": part["field"]})
                    elif "index" in part:
                        parts_out.append({"index": part["index"]})

        return {"type": "variable", "base": base, "parts": parts_out}

    return simp


# -------------------------------
# Public API
# -------------------------------


class PEGParser:
    def __init__(self, grammar_text: str, start: Optional[str] = None):
        peg = PEG.parse(grammar_text)
        analyzer = _Analyzer(peg.rules)
        cycles = analyzer.detect_left_recursion()
        if cycles:
            msgs = [" -> ".join(c) for c in cycles]
            raise LeftRecursionError("Left recursion detected in grammar:\n  " + "\n  ".join(msgs))
        self.grammar = peg
        if start:
            self.grammar.start = start

    def parse(self, text: str, start_rule: Optional[str] = None) -> Any:
        runtime = _Runtime(self.grammar, text)
        ast = runtime.parse(start_rule)
        return _simplify_ast(ast)


if __name__ == "__main__":
    peg = r"""
        Start <- 'hello'i name:Identifier '!'?
        Identifier <- !Keyword [A-Za-z_] [A-Za-z0-9_]*
        Keyword <- 'HELLO'i
    """
    p = PEGParser(peg, start="Start")
    print(p.parse("Hello world!"))
