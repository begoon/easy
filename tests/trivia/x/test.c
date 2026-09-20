#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR x = {0};
STR a = {0};
STR b = {0};
STR $0 = { .data = "abc", .sz = 3 };
STR $F = { .data = "tests/trivia/test.easy", .sz = 22 };
int main_program()
{
    OWN(x, manage_string, $r1);
    OWN(a, manage_string, $r2);
    OWN(b, manage_string, $r3);
    {
        const STR $r6 = SUBSTR($0, 0, 2);
        OWN($r6, manage_string, $r7);
        const STR $r5 = $concat("AA", b, $r6);
        OWN($r5, manage_string, $r8);
        const STR $r4 = $concat("AA", a, $r5);
        OWN($r4, manage_string, $r9);
        __auto_type $r10 = $r4;
        manage_string(&$r10, 1);
        manage_string(&x, -1);
        x = $r10;
    }
    $exit();
}
