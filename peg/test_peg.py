import json
from pathlib import Path
from typing import Any

import pytest

from peg.peg_parser import PEGParser

CWD = Path(__file__).parent


@pytest.mark.parametrize(
    ["name", "start"],
    [
        ("assignment_subscript_part", "assignment_statement"),
        ("output_subscript_part", "output_statement"),
    ],
)
def test_peg(name: str, start: str):
    test = CWD / "tests" / name

    source = test.with_suffix(".easy").read_text()
    expected = json.loads(test.with_suffix(".json").read_text())

    check(source, start, expected)


def check(source: str, start: str, expected: dict[str, Any]) -> None:
    grammar = (Path(__file__).parent / "easy.peg").read_text()
    parser = PEGParser(grammar, start=start)

    v = parser.parse(source)
    assert v == expected, f"EXPECTED\n{expected}\n!=\nACTUAL\n{v}"
