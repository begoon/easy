#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
double r = 0.0;
int b = 0;
STR s = {0};
STR $0 = { .data = "xyz" };
STR $1 = { .data = " - " };
STR $2 = { .data = " = " };
STR $3 = { .data = " <> " };
STR $4 = { .data = "abc" };
STR $5 = { .data = "i:" };
STR $6 = { .data = " r:" };
STR $7 = { .data = " b:" };
STR $8 = { .data = " s:" };
STR $9 = { .data = "%%" };
STR $10 = { .data = " " };
STR $11 = { .data = "123" };
STR $12 = { .data = "456" };
STR $13 = { .data = "$$" };
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
int main()
{
    const auto $r1 = $concat("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
    $output("A", $r1);
    a = 321;
    r = 1.003;
    b = TRUE;
    s = $4;
    const auto $r2 = $concat("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
    $output("A", $r2);
    const auto $r4 = CHARACTER(50);
    const auto $r5 = LENGTH($11);
    const auto $r6 = FIX(23.56);
    const auto $r7 = FLOAT(543);
    const auto $r3 = $concat("AAiAiAr", $r4, $10, $r5, $10, $r6, $10, $r7);
    $output("A", $r3);
    const auto $r8 = $concat("AA", $11, $12);
    $output("A", $r8);
    const auto $r10 = fa();
    const auto $r11 = fr();
    const auto $r12 = fb();
    const auto $r13 = fs();
    const auto $r9 = $concat("AiArAbAAA", $5, $r10, $6, $r11, $7, $r12, $8, $r13, $13);
    $output("A", $r9);
    exit(0);
}
