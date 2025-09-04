import pathlib
from typing import Any

from peg.peg_parser import PEGParser


def check(code: str, start: str, expected: dict[str, Any]) -> None:
    parent = pathlib.Path(__file__).parent
    grammar = (parent / "easy.peg").read_text()
    parser = PEGParser(grammar, start=start)

    v = parser.parse(code)

    assert v == expected, f"EXPECTED\n{expected}\n!=\nACTUAL\n{v}"
