#!/usr/bin/env python3
#
import json
import pathlib
import sys
from pathlib import Path

from lexer import Lexer, Token
from nodes import Array, common, emit, python_imports, types_registry
from parser import Parser
from peg.parser import PEGParser
from yamler import yamlizer


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: easy.py <input.easy> [-c <output.c>] [-t] [-a] [-j] [-p <output.py>]")
        print("  -c <output.c>  - specify output C file (default: input.c)")
        print("  -t             - generate tokens file (default: off)")
        print("  -a             - generate AST file (default: off)")
        print("  -y             - generate YAML AST file (default: off)")
        print("  -j             - generate PEG JSON AST file (default: off)")
        print("  -p <output.py> - generate Python file (default: input.py)")
        sys.exit(1)

    input_file = pathlib.Path(sys.argv[1])

    source = input_file.read_text()

    lexer = Lexer(source, input_file)

    tokens = lexer.tokens()
    if "-t" in sys.argv:
        tokens_file = input_file.with_suffix(".tokens")

        def format_token(token: Token) -> str:
            return f"{input_file}:{token.line}:{token.col}\t {token.value} / {token.type}"

        tokens_file.write_text("\n".join(format_token(t) for t in tokens) + "\n")

    ast = Parser(tokens, source).program()
    if "-a" in sys.argv:
        ast_file = input_file.with_suffix(".ast")
        ast_file.write_text(ast.meta() + "\n")

    if "-y" in sys.argv:
        yaml_file = input_file.with_suffix(".yaml")
        yaml_file.write_text(yamlizer(ast) + "\n")

    if "-j" in sys.argv:
        peg_ast_file = input_file.with_suffix(".json")

        grammar = open("peg/easy.peg").read()
        peg_ast = PEGParser(grammar, start="compilation").parse(source)

        peg_ast_file.write_text(json.dumps(peg_ast, indent=4) + "\n")

    output_c = Path(arg(sys.argv, "-c") or input_file.with_suffix(".c"))

    code_c = ast.c().strip()

    with open(output_c, "w") as f:
        f.write('#include "runtime.c"\n')
        if types_registry:
            for name, definition in types_registry.items():
                v = "typedef "
                if isinstance(definition, Array):
                    v += definition.c(variable=name)
                else:
                    v += definition.c() + " " + name
                v += ";\n"
                f.write(v)
        if common:
            f.write(emit(common) + "\n")
        f.write(code_c + "\n")

    output_py = Path(arg(sys.argv, "-p") or input_file.with_suffix(".py"))
    if flag(sys.argv, "-p") is not None:
        code_py = ast.py().strip()

        with open(output_py, "w") as f:
            if python_imports:
                imports = ", ".join(sorted(python_imports))
                f.write(f"from runtime import {imports}\n\n")
            f.write(code_py + "\n")
        code_py = ast.py().strip()

        with open(output_py, "w") as f:
            if python_imports:
                imports = ", ".join(sorted(python_imports))
                f.write(f"from runtime import {imports}\n\n")
            f.write(code_py + "\n")
