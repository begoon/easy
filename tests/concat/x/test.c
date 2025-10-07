#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
double r = 0.0;
int b = 0;
STR s = {0};
STR $0 = { .data = "xyz", .sz = 3, .immutable = 1 };
STR $1 = { .data = " - ", .sz = 3, .immutable = 1 };
STR $2 = { .data = " = ", .sz = 3, .immutable = 1 };
STR $3 = { .data = " <> ", .sz = 4, .immutable = 1 };
STR $4 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $5 = { .data = "i:", .sz = 2, .immutable = 1 };
STR $6 = { .data = " r:", .sz = 3, .immutable = 1 };
STR $7 = { .data = " b:", .sz = 3, .immutable = 1 };
STR $8 = { .data = " s:", .sz = 3, .immutable = 1 };
STR $9 = { .data = "%%", .sz = 2, .immutable = 1 };
STR $10 = { .data = " ", .sz = 1, .immutable = 1 };
STR $11 = { .data = "123", .sz = 3, .immutable = 1 };
STR $12 = { .data = "456", .sz = 3, .immutable = 1 };
STR $13 = { .data = "$$", .sz = 2, .immutable = 1 };
STR $F = { .data = "tests/concat/test.easy", .sz = 22, .immutable = 1 };
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
    s = $0;
    return s;
}
int main_program()
{
    const STR $r1 = $concat("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
    $output("A", $r1);
    a = 321;
    r = 1.003;
    b = TRUE;
    s = $4;
    const STR $r2 = $concat("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
    $output("A", $r2);
    const STR $r4 = CHARACTER(50);
    const int $r5 = LENGTH($11);
    const int $r6 = FIX(23.56);
    const double $r7 = FLOAT(543);
    const STR $r3 = $concat("AAiAiAr", $r4, $10, $r5, $10, $r6, $10, $r7);
    $output("A", $r3);
    const STR $r8 = $concat("AA", $11, $12);
    $output("A", $r8);
    const int $r10 = fa();
    const double $r11 = fr();
    const int $r12 = fb();
    const STR $r13 = fs();
    const STR $r9 = $concat("AiArAbAAA", $5, $r10, $6, $r11, $7, $r12, $8, $r13, $13);
    $output("A", $r9);
    $exit();
}
