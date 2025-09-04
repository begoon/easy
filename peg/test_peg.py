import json
from pathlib import Path
from typing import Any

import pytest

from peg.peg_parser import PEGParser

CWD = Path(__file__).parent


@pytest.mark.parametrize(
    ["name", "start"],
    [
        ("peg_array", "compilation"),
        ("peg_assignment", "assignment_statement"),
        ("peg_trivia", "compilation"),
        ("peg_type_definition", "type_definition"),
        ("peg_output", "output_statement"),
        #
        ("array", "compilation"),
        ("trivia2", "compilation"),
        ("output", "compilation"),
    ],
)
def test_peg(name: str, start: str):
    if name.startswith("peg_"):
        test = CWD / "tests" / name
        expected_ = test
    else:
        test = CWD.parent / "tests" / name / "test"
        expected_ = CWD / "tests" / (Path("x-" + name))

    code = test.with_suffix(".easy").read_text()
    expected = json.loads(expected_.with_suffix(".json").read_text())

    check(code, start, expected)


def check(code: str, start: str, expected: dict[str, Any]) -> None:
    parent = Path(__file__).parent
    grammar = (parent / "easy.peg").read_text()
    parser = PEGParser(grammar, start=start)

    v = parser.parse(code)
    assert v == expected, f"EXPECTED\n{expected}\n!=\nACTUAL\n{v}"
