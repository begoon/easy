#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
double r = 0.0;
int b = 0;
STR s = {0};
STR $0 = { .data = "xyz", .sz = 3 };
STR $1 = { .data = " - ", .sz = 3 };
STR $2 = { .data = " = ", .sz = 3 };
STR $3 = { .data = " <> ", .sz = 4 };
STR $4 = { .data = "abc", .sz = 3 };
STR $5 = { .data = "i:", .sz = 2 };
STR $6 = { .data = " r:", .sz = 3 };
STR $7 = { .data = " b:", .sz = 3 };
STR $8 = { .data = " s:", .sz = 3 };
STR $9 = { .data = "%%", .sz = 2 };
STR $10 = { .data = " ", .sz = 1 };
STR $11 = { .data = "123", .sz = 3 };
STR $12 = { .data = "456", .sz = 3 };
STR $13 = { .data = "$$", .sz = 2 };
STR $F = { .data = "tests/concat/test.easy", .sz = 22 };
int fa()
{
    return 1;
}
double fr()
{
    return 2.345;
}
int fb()
{
    return TRUE;
}
STR fs()
{
    STR s = {0};
    OWN(s, manage_string, $r23);
    __auto_type $r24 = $0;
    manage_string(&$r24, 1);
    manage_string(&s, -1);
    s = $r24;
    manage_string(&s, 1);
    return s;
}
int main_program()
{
    OWN(s, manage_string, $r1);
    {
        const STR $r2 = $concat("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
        OWN($r2, manage_string, $r3);
        $output("A", $r2);
    }
    a = 321;
    r = 1.003;
    b = TRUE;
    __auto_type $r4 = $4;
    manage_string(&$r4, 1);
    manage_string(&s, -1);
    s = $r4;
    {
        const STR $r5 = $concat("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
        OWN($r5, manage_string, $r6);
        $output("A", $r5);
    }
    {
        const STR $r8 = CHARACTER(50);
        OWN($r8, manage_string, $r9);
        const int $r10 = LENGTH($11);
        const int $r11 = FIX(23.56);
        const double $r12 = FLOAT(543);
        const STR $r7 = $concat("AAiAiAr", $r8, $10, $r10, $10, $r11, $10, $r12);
        OWN($r7, manage_string, $r13);
        $output("A", $r7);
    }
    {
        const STR $r14 = $concat("AA", $11, $12);
        OWN($r14, manage_string, $r15);
        $output("A", $r14);
    }
    {
        const int $r17 = fa();
        const double $r18 = fr();
        const int $r19 = fb();
        const STR $r20 = fs();
        OWN($r20, manage_string, $r21);
        const STR $r16 = $concat("AiArAbAAA", $5, $r17, $6, $r18, $7, $r19, $8, $r20, $13);
        OWN($r16, manage_string, $r22);
        $output("A", $r16);
    }
    $exit();
}
