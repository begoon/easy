#!/usr/bin/env python3
#
import pathlib
import sys
from parser import BuiltinFunction, Parser, common, emit, functions_list, procedures_list, types_list, variables_list
from pathlib import Path

from lexer import InputText, Lexer, Token
from peg.parser import PEGParser
from yamler import yamlizer


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: easy.py <input.easy> [-c <output.c>] [-t] [-a] [-e]")
        print("  -c <output.c>  - specify output C file (default: input.c)")
        print("  -t             - generate tokens file (default: off)")
        print("  -a             - generate YAML AST file (default: off)")
        print("  -e             - generate PEG YAML AST file (default: off)")
        sys.exit(1)

    input_file = pathlib.Path(sys.argv[1])

    source = input_file.read_text()

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

    output_c = Path(arg(sys.argv, "-c") or input_file.with_suffix(".c"))

    code_c = ast.c().strip()

    with open(output_c, "w") as f:
        f.write('#include "runtime.c"\n')
        for name, definition in types_list.items():
            v = f"{definition.typedef(name)};\n"
            f.write(v)
        if common:
            f.write(emit(common) + "\n")
        for name, v in variables_list.items():
            if v.is_const():
                f.write(v.const() + ";\n")
        if functions_list:
            for v in functions_list.values():
                if isinstance(v, BuiltinFunction):
                    continue
                f.write(v.c() + "\n")
        if procedures_list:
            for v in procedures_list.values():
                f.write(v.c() + "\n")
        f.write(code_c + "\n")
