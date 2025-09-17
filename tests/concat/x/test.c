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
    $output("A", $concat("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE));
    a = 321;
    r = 1.003;
    b = TRUE;
    s = $4;
    $output("A", $concat("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9));
    $output("A", $concat("AAiAiAr", CHARACTER(50), $10, LENGTH($11), $10, FIX(23.56), $10, FLOAT(543)));
    $output("A", $concat("AA", $11, $12));
    $output("A", $concat("AiArAbAAA", $5, fa(), $6, fr(), $7, fb(), $8, fs(), $13));
    exit(0);
}
