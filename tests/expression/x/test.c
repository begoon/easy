#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int x = 0;
int a = 0;
int b = 0;
int c = 0;
STR s = {0};
int f = 0;
STR $0 = { .data = "3", .sz = 1 };
STR $1 = { .data = "x = ", .sz = 4 };
STR $2 = { .data = " OK", .sz = 3 };
STR $3 = { .data = "f = ", .sz = 4 };
STR $4 = { .data = "a", .sz = 1 };
STR $5 = { .data = "abc", .sz = 3 };
STR $6 = { .data = "s = ", .sz = 4 };
STR $F = { .data = "tests/expression/test.easy", .sz = 26 };
int func(int z)
{
    return z;
}
int main_program()
{
    OWN(s, manage_string, $r1);
    x = a;
    const int $r2 = (a + b);
    x = $r2;
    const int $r3 = (+a);
    x = $r3;
    const int $r4 = (-a);
    x = $r4;
    const int $r6 = (b * c);
    const int $r5 = (a + $r6);
    x = $r5;
    const int $r9 = (-b);
    const int $r10 = (1 - 2);
    const int $r8 = ($r9 + $r10);
    const int $r12 = LENGTH($0);
    const int $r11 = ($r12 * 7);
    const int $r7 = ($r8 - $r11);
    x = $r7;
    const int $r15 = (-b);
    const int $r16 = (1 - 2);
    const int $r14 = ($r15 + $r16);
    const int $r17 = func(3);
    const int $r13 = ($r14 - $r17);
    x = $r13;
    $output("AiA", $1, x, $2);
    const int $r19 = (a < b);
    const int $r18 = (!$r19);
    f = $r18;
    const int $r21 = (a < b);
    const int $r20 = ($r21 ^ TRUE);
    f = $r20;
    const int $r23 = (a < b);
    const int $r25 = (c >= 0);
    const int $r24 = ($r25 && FALSE);
    const int $r22 = ($r23 || $r24);
    f = $r22;
    const int $r28 = (a >= b);
    const int $r29 = (c < 0);
    const int $r27 = ($r28 && $r29);
    const int $r26 = ($r27 || TRUE);
    f = $r26;
    $output("AbA", $3, f, $2);
    {
        const STR $r30 = $concat("ii", a, b);
        OWN($r30, manage_string, $r31);
        __auto_type $r32 = $r30;
        manage_string(&$r32, 1);
        manage_string(&s, -1);
        s = $r32;
    }
    {
        const STR $r33 = $concat("Ai", $4, b);
        OWN($r33, manage_string, $r34);
        __auto_type $r35 = $r33;
        manage_string(&$r35, 1);
        manage_string(&s, -1);
        s = $r35;
    }
    {
        const STR $r36 = $concat("Aii", $4, b, c);
        OWN($r36, manage_string, $r37);
        __auto_type $r38 = $r36;
        manage_string(&$r38, 1);
        manage_string(&s, -1);
        s = $r38;
    }
    {
        const STR $r41 = SUBSTR($5, 0, 2);
        OWN($r41, manage_string, $r42);
        const STR $r40 = $concat("iA", b, $r41);
        OWN($r40, manage_string, $r43);
        const STR $r39 = $concat("iA", a, $r40);
        OWN($r39, manage_string, $r44);
        __auto_type $r45 = $r39;
        manage_string(&$r45, 1);
        manage_string(&s, -1);
        s = $r45;
    }
    {
        const STR $r47 = SUBSTR($5, 0, 2);
        OWN($r47, manage_string, $r48);
        const STR $r46 = $concat("iiA", a, b, $r47);
        OWN($r46, manage_string, $r49);
        __auto_type $r50 = $r46;
        manage_string(&$r50, 1);
        manage_string(&s, -1);
        s = $r50;
    }
    {
        const STR $r52 = CHARACTER(b);
        OWN($r52, manage_string, $r53);
        const STR $r51 = $concat("iA", a, $r52);
        OWN($r51, manage_string, $r54);
        __auto_type $r55 = $r51;
        manage_string(&$r55, 1);
        manage_string(&s, -1);
        s = $r55;
    }
    $output("AAA", $6, s, $2);
}
