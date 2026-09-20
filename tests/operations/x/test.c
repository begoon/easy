#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
int c = 0;
STR s = {0};
STR $0 = { .data = "...", .sz = 3 };
STR $1 = { .data = "abc", .sz = 3 };
STR $F = { .data = "tests/operations/test.easy", .sz = 26 };
int main_program()
{
    OWN(s, manage_string, $r1);
    a = 1;
    b = 2;
    const int $r3 = (a + 0);
    const int $r2 = ($r3 + b);
    c = $r2;
    $output("iA", c, $0);
    {
        const STR $r4 = $concat("AA", s, $1);
        OWN($r4, manage_string, $r5);
        __auto_type $r6 = $r4;
        manage_string(&$r6, 1);
        manage_string(&s, -1);
        s = $r6;
    }
    $output("A", s);
}
