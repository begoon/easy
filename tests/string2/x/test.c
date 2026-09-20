#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR v = {0};
STR program = {0};
STR $0 = { .data = "abc-XYZ-0123456789", .sz = 18 };
STR $1 = { .data = "1234567890", .sz = 10 };
STR $F = { .data = "tests/string2/test.easy", .sz = 23 };
STR bf(STR v)
{
    manage_string(&v, 1);
    OWN(v, manage_string, $r15);
    manage_string(&v, 1);
    return v;
}
int main_program()
{
    OWN(s, manage_string, $r1);
    OWN(v, manage_string, $r2);
    OWN(program, manage_string, $r3);
    __auto_type $r4 = $0;
    manage_string(&$r4, 1);
    manage_string(&s, -1);
    s = $r4;
    {
        const STR $r6 = SUBSTR(s, 1, 3);
        OWN($r6, manage_string, $r7);
        const STR $r8 = CHARACTER(100);
        OWN($r8, manage_string, $r9);
        const STR $r5 = $concat("AA", $r6, $r8);
        OWN($r5, manage_string, $r10);
        __auto_type $r11 = $r5;
        manage_string(&$r11, 1);
        manage_string(&v, -1);
        v = $r11;
    }
    $output("A", v);
    __auto_type $r12 = $1;
    manage_string(&$r12, 1);
    manage_string(&program, -1);
    program = $r12;
    {
        const STR $r13 = bf(program);
        OWN($r13, manage_string, $r14);
        $output("A", $r13);
    }
}
