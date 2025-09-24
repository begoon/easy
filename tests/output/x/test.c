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
