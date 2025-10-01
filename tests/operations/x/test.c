#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
int c = 0;
STR s = {0};
STR $0 = { .data = "...", .sz = 3, .immutable = 1 };
STR $1 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $F = { .data = "tests/operations/test.easy", .sz = 26, .immutable = 1 };
int main_program()
{
    a = 1;
    b = 2;
    const int $r2 = (a + 0);
    const int $r1 = ($r2 + b);
    c = $r1;
    $output("iA", c, $0);
    const STR $r3 = $concat("AA", s, $1);
    s = $r3;
    $output("A", s);
}
