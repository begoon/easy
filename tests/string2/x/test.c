#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR v = {0};
STR program = {0};
STR $0 = { .data = "abc-XYZ-0123456789", .sz = 18, .immutable = 1 };
STR $1 = { .data = "1234567890", .sz = 10, .immutable = 1 };
STR $F = { .data = "tests/string2/test.easy", .sz = 23, .immutable = 1 };
STR bf(STR v)
{
    return v;
}
int main_program()
{
    s = $0;
    const STR $r2 = SUBSTR(s, 1, 3);
    const STR $r3 = CHARACTER(100);
    const STR $r1 = $concat("AA", $r2, $r3);
    v = $r1;
    $output("A", v);
    program = $1;
    const STR $r4 = bf(program);
    $output("A", $r4);
}
