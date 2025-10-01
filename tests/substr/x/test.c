#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "abcXYZ", .sz = 6, .immutable = 1 };
STR $1 = { .data = "12345", .sz = 5, .immutable = 1 };
STR $2 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $F = { .data = "tests/substr/test.easy", .sz = 22, .immutable = 1 };
int main_program()
{
    s = $0;
    const STR $r1 = SUBSTR($1, 1, 2);
    $output("A", $r1);
    const STR $r2 = SUBSTR(s, 1, 2);
    $output("A", $r2);
    const STR $r4 = SUBSTR(s, 3, 2);
    const STR $r3 = $concat("AA", $2, $r4);
    $output("A", $r3);
    exit(0);
}
