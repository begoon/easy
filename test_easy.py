import pytest

from easy import Lexer, Parser


@pytest.mark.parametrize(
    "input, expected",
    [
        ("a", [("IDENT", "a"), ("EOF", "")]),
        ("a b", [("IDENT", "a"), ("IDENT", "b"), ("EOF", "")]),
        ("a /* b /* \n */ */ c", [("IDENT", "a"), ("IDENT", "c"), ("EOF", "")]),
    ],
)
def test_tokens(input, expected) -> None:
    lexer = Lexer(input)
    tokens = lexer.tokens()
    assert [(t.type, t.value) for t in tokens] == expected


@pytest.mark.parametrize(
    "input, expected",
    [
        ("a", "a"),
        ("a + b", "(a + b)"),
        ("+a", "(+a)"),
        ("-a", "(-a)"),
        ("a + b * с", "(a + (b * с))"),
        (
            '-b + (1 - 2) - LENGTH("3") * 7 XOR 1',
            "((((-b) + (1 - 2)) - (LENGTH('3') * 7)) ^ 1)",
        ),
        ("-b + (1 - 2) - func(" "3" ")", "(((-b) + (1 - 2)) - func(" "3" "))"),
        ("NOT a < b", "(NOT(a < b))"),
        ("a || b", "2:(strconv(a) || strconv(b))"),
        ('"a" || b', "2:('a' || strconv(b))"),
        ('"a" || b || c', "3:('a' || strconv(b) || strconv(c))"),
        (
            'a || (b || SUBSTR("abc", 0, 2))',
            "2:(strconv(a) || 2:(strconv(b) || strconv(SUBSTR('abc', 0, 2))))",
        ),
        (
            'a || b || SUBSTR("abc", 0, 2)',
            "3:(strconv(a) || strconv(b) || strconv(SUBSTR('abc', 0, 2)))",
        ),
        ("a || CHARACTER(b)", "2:(strconv(a) || CHARACTER(b))"),
    ],
)
def test_expression(input, expected) -> None:
    result = Parser(Lexer(input).tokens(), input).expression()
    if not isinstance(expected, str):
        expected = expected.meta()
    assert result.meta() == expected, f"\n{expected}\n!=\n{result}\n"
