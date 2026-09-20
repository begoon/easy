#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "PROGRAM Q: DECLARE s STRING; SET s := %; OUTPUT SUBSTR(s, 0, 38) || CHARACTER(34) || s || CHARACTER(34) || SUBSTR(s, 39, LENGTH(s)-39); END PROGRAM Q;", .sz = 150 };
STR $F = { .data = "tests/quine/test.easy", .sz = 21 };
int main_program()
{
    OWN(s, manage_string, $r1);
    __auto_type $r2 = $0;
    manage_string(&$r2, 1);
    manage_string(&s, -1);
    s = $r2;
    {
        const STR $r4 = SUBSTR(s, 0, 38);
        OWN($r4, manage_string, $r5);
        const STR $r6 = CHARACTER(34);
        OWN($r6, manage_string, $r7);
        const STR $r8 = CHARACTER(34);
        OWN($r8, manage_string, $r9);
        const int $r12 = LENGTH(s);
        const int $r11 = ($r12 - 39);
        const STR $r10 = SUBSTR(s, 39, $r11);
        OWN($r10, manage_string, $r13);
        const STR $r3 = $concat("AAAAA", $r4, $r6, s, $r8, $r10);
        OWN($r3, manage_string, $r14);
        $output("A", $r3);
    }
}
