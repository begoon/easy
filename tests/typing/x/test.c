#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int i = 0;
int j = 0;
double r = 0.0;
int b = 0;
STR $0 = { .data = "abc", .sz = 3 };
STR $1 = { .data = " - ", .sz = 3 };
STR $2 = { .data = "xzy", .sz = 3 };
STR $F = { .data = "tests/typing/test.easy", .sz = 22 };
STR S()
{
    manage_string(&$0, 1);
    return $0;
}
int I()
{
    return 123;
}
double R()
{
    return 567.89;
}
int B()
{
    return TRUE;
}
int main_program()
{
    OWN(s, manage_string, $r1);
    {
        const STR $r2 = S();
        OWN($r2, manage_string, $r3);
        const int $r4 = I();
        const double $r5 = R();
        const int $r6 = B();
        $output("AAiArAb", $r2, $1, $r4, $1, $r5, $1, $r6);
    }
    {
        const STR $r8 = S();
        OWN($r8, manage_string, $r9);
        const STR $r7 = $concat("AA", $r8, $2);
        OWN($r7, manage_string, $r10);
        __auto_type $r11 = $r7;
        manage_string(&$r11, 1);
        manage_string(&s, -1);
        s = $r11;
    }
    $output("A", s);
    const int $r12 = (i + j);
    i = $r12;
    const double $r17 = R();
    const int $r16 = FIX($r17);
    const int $r15 = ($r16 * 2);
    const int $r14 = ($r15 / 7);
    const int $r13 = (100 + $r14);
    i = $r13;
    const double $r19 = R();
    const double $r20 = FLOAT(200);
    const double $r18 = ($r19 + $r20);
    r = $r18;
    const int $r22 = FLOOR(r);
    const int $r21 = ($r22 + i);
    j = $r21;
    const double $r24 = (r + 2.1);
    const int $r23 = FIX($r24);
    i = $r23;
    const int $r27 = (i < j);
    const int $r28 = B();
    const int $r26 = ($r27 && $r28);
    const int $r30 = (i >= j);
    const int $r29 = (!$r30);
    const int $r25 = ($r26 || $r29);
    b = $r25;
    $output("iAiArAb", i, $1, j, $1, r, $1, b);
}
