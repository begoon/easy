import pytest

from easy import Lexer, Parser, dump


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
        ('a || (b || SUBSTR("abc", 0, 2))', "(a || (b || SUBSTR('abc', 0, 2)))"),
        ('-b + (1 - 2) - LENGTH("3") * 7 XOR 1', "((((-b) + (1 - 2)) - (LENGTH('3') * 7)) XOR 1)"),
        ("NOT a < b", "(NOT(a < b))"),
    ],
)
def test_expression(input, expected) -> None:
    lexer = Lexer(input)
    tokens = lexer.tokens()
    actual = Parser(tokens).expression()
    if not isinstance(expected, str):
        expected = dump(expected)
    assert dump(actual) == expected, f"\n{expected}\n!=\n{actual}\n"
