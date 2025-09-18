#!/usr/bin/env python3
#
import pathlib
import re
import sys
from pathlib import Path

from lexer import InputText, Lexer, Token
from parser import BuiltinFunction, Parser, context, emit
from peg.parser import PEGParser
from yamler import yamlizer


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


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: easy.py <input.easy> [-c <output.c>] [-t] [-a] [-e]")
        print("  -c <output.c>  - specify output C file (default: input.c)")
        print("  -t             - generate tokens file (default: input.tokens)")
        print("  -a             - generate YAML AST file (default: input.yaml)")
        print("  -e             - generate PEG YAML AST file (default: input.peg.yaml)")
        print("  -s <output.s>  - generate symbols file (default: input.s)")
        sys.exit(1)

    input_file = pathlib.Path(sys.argv[1])

    source = input_file.read_text()
    flags_comment = source.splitlines()[0].strip()
    if flags_comment.startswith("//easy:flags "):
        flags_pairs = flags_comment.split()[1:]
        flags = {k: v for k, v in (pair.split("=") for pair in flags_pairs)}
        context.flags.update(flags)

    lexer = Lexer(InputText(filename=input_file))

    tokens = lexer.tokens()
    if "-t" in sys.argv:
        tokens_file = input_file.with_suffix(".tokens")

        def format_token(token: Token) -> str:
            return f"{token.input.filename}:{token.line}:{token.character}\t {token.value} / {token.type}"

        tokens_file.write_text("\n".join(format_token(t) for t in tokens) + "\n")

    ast = Parser(tokens).program()

    if "-a" in sys.argv:
        ast_file = input_file.with_suffix(".yaml")
        ast_file.write_text(yamlizer(ast))

    if "-e" in sys.argv:
        grammar = Path("peg/easy.peg").read_text()
        peg_ast = PEGParser(grammar, start="compilation").parse(source)

        peg_ast_file = input_file.with_suffix(".peg.yaml")
        peg_ast_file.write_text(yamlizer(peg_ast))

    output_s = Path(arg(sys.argv, "-s") or input_file.with_suffix(".s"))
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

    output_c = Path(arg(sys.argv, "-c") or input_file.with_suffix(".c"))

    code_c = ast.c().strip()

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


##########################################################################
