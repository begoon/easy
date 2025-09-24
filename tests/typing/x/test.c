#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int i = 0;
int j = 0;
int v = 0;
double r = 0.0;
STR $0 = { .data = "abc" };
STR $1 = { .data = "xzy" };
STR S()
{
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
int main()
{
    const auto $r1 = S();
    $output("A", $r1);
    const auto $r2 = I();
    $output("i", $r2);
    const auto $r3 = R();
    $output("r", $r3);
    const auto $r4 = B();
    $output("b", $r4);
    const auto $r6 = S();
    const auto $r5 = $concat("AA", $r6, $1);
    s = $r5;
    $output("A", s);
    const auto $r7 = (i + j);
    v = $r7;
    $output("i", v);
    const auto $r8 = (1 + 2.1);
    v = $r8;
    $output("i", v);
}
