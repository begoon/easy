import pathlib
import sys

from easy_lexer import Lexer, Token
from easy_nodes import Array, ProgramStatement, block, common, types_registry
from easy_parser import Parser


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

    c_code = ast.c().strip()

    output = arg(sys.argv, "-o") or input_file.with_suffix(".c")

    with open(output, "w") as f:
        f.write('#include "preamble.c"\n')
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
            f.write(block(common) + "\n")
        f.write(c_code + "\n")
