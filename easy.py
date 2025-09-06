#!/usr/bin/env python3
#
import json
import pathlib
import sys
from pathlib import Path

from easy_lexer import Lexer, Token
from easy_nodes import Array, ProgramStatement, block, common, types_registry
from easy_parser import Parser
from peg.peg_parser import PEGParser


def parse(code: str) -> ProgramStatement:
    lexer = Lexer(code)
    tokens = lexer.tokens()
    return Parser(tokens, code).program()


# ---


def compile(source: str) -> ProgramStatement:
    return parse(source)


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: easy.py <input.easy> [-o <output.c>] [-t] [-a]")
        print("  -o <output.c> - specify output C file (default: input.c)")
        print("  -t            - generate tokens file (default: off)")
        print("  -a            - generate AST file (default: off)")
        print("  -j            - generate PEG JSON AST file (default: off)")
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

    if "-j" in sys.argv:
        peg_ast_file = input_file.with_suffix(".json")

        grammar = open("peg/easy.peg").read()
        peg_ast = PEGParser(grammar, start="compilation").parse(source)

        peg_ast_file.write_text(json.dumps(peg_ast, indent=4) + "\n")

    output = Path(arg(sys.argv, "-o") or input_file.with_suffix(".c"))

    is_py = output.suffix == ".py"

    code = (ast.py() if is_py else ast.c()).strip()

    with open(output, "w") as f:
        if is_py:
            f.write("import preamble # noqa:  F401\n\n")
        else:
            f.write('#include "preamble.c"\n')
        if types_registry:
            if is_py:
                assert False, "TODO: Python output does not support typedefs"
            for name, definition in types_registry.items():
                v = "typedef "
                if isinstance(definition, Array):
                    v += definition.c(variable=name)
                else:
                    v += definition.c() + " " + name
                v += ";\n"
                f.write(v)
        if common:
            f.write(block(common) + "\n")
        f.write(code + "\n")
