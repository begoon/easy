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
STR $F = { .data = "tests/output/test.easy", .sz = 22 };
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
    OWN(s, manage_string, $r15);
    __auto_type $r16 = $0;
    manage_string(&$r16, 1);
    manage_string(&s, -1);
    s = $r16;
    manage_string(&s, 1);
    return s;
}
int main_program()
{
    OWN(s, manage_string, $r1);
    $output("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
    a = 321;
    r = 1.003;
    b = TRUE;
    __auto_type $r2 = $4;
    manage_string(&$r2, 1);
    manage_string(&s, -1);
    s = $r2;
    $output("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
    {
        const STR $r3 = CHARACTER(50);
        OWN($r3, manage_string, $r4);
        const int $r5 = LENGTH($11);
        const int $r6 = FIX(23.56);
        const double $r7 = FLOAT(543);
        $output("AAiAiAr", $r3, $10, $r5, $10, $r6, $10, $r7);
    }
    {
        const STR $r8 = $concat("AA", $11, $12);
        OWN($r8, manage_string, $r9);
        $output("A", $r8);
    }
    {
        const int $r10 = fa();
        const double $r11 = fr();
        const int $r12 = fb();
        const STR $r13 = fs();
        OWN($r13, manage_string, $r14);
        $output("AiArAbAAA", $5, $r10, $6, $r11, $7, $r12, $8, $r13, $13);
    }
    $exit();
}
