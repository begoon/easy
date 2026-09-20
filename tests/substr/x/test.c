#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "abcXYZ", .sz = 6 };
STR $1 = { .data = "12345", .sz = 5 };
STR $2 = { .data = "abc", .sz = 3 };
STR $F = { .data = "tests/substr/test.easy", .sz = 22 };
int main_program()
{
    OWN(s, manage_string, $r1);
    __auto_type $r2 = $0;
    manage_string(&$r2, 1);
    manage_string(&s, -1);
    s = $r2;
    {
        const STR $r3 = SUBSTR($1, 1, 2);
        OWN($r3, manage_string, $r4);
        $output("A", $r3);
    }
    {
        const STR $r5 = SUBSTR(s, 1, 2);
        OWN($r5, manage_string, $r6);
        $output("A", $r5);
    }
    {
        const STR $r8 = SUBSTR(s, 3, 2);
        OWN($r8, manage_string, $r9);
        const STR $r7 = $concat("AA", $2, $r8);
        OWN($r7, manage_string, $r10);
        $output("A", $r7);
    }
    $exit();
}
