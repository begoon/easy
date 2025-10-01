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
STR $F = { .data = "tests/output/test.easy", .sz = 22, .immutable = 1 };
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
    $output("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
    a = 321;
    r = 1.003;
    b = TRUE;
    s = $4;
    $output("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
    const STR $r1 = CHARACTER(50);
    const int $r2 = LENGTH($11);
    const int $r3 = FIX(23.56);
    const double $r4 = FLOAT(543);
    $output("AAiAiAr", $r1, $10, $r2, $10, $r3, $10, $r4);
    const STR $r5 = $concat("AA", $11, $12);
    $output("A", $r5);
    const int $r6 = fa();
    const double $r7 = fr();
    const int $r8 = fb();
    const STR $r9 = fs();
    $output("AiArAbAAA", $5, $r6, $6, $r7, $7, $r8, $8, $r9, $13);
    exit(0);
}
