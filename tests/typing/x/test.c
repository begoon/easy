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
STR $0 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $1 = { .data = " - ", .sz = 3, .immutable = 1 };
STR $2 = { .data = "xzy", .sz = 3, .immutable = 1 };
STR $F = { .data = "tests/typing/test.easy", .sz = 22, .immutable = 1 };
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
int main_program()
{
    const STR $r1 = S();
    const int $r2 = I();
    const double $r3 = R();
    const int $r4 = B();
    $output("AAiArAb", $r1, $1, $r2, $1, $r3, $1, $r4);
    const STR $r6 = S();
    const STR $r5 = $concat("AA", $r6, $2);
    s = $r5;
    $output("A", s);
    const int $r7 = (i + j);
    i = $r7;
    const double $r12 = R();
    const int $r11 = FIX($r12);
    const int $r10 = ($r11 * 2);
    const int $r9 = ($r10 / 7);
    const int $r8 = (100 + $r9);
    i = $r8;
    const double $r14 = R();
    const double $r15 = FLOAT(200);
    const double $r13 = ($r14 + $r15);
    r = $r13;
    const int $r17 = FLOOR(r);
    const int $r16 = ($r17 + i);
    j = $r16;
    const double $r18 = (r + 2.1);
    i = $r18;
    const int $r21 = (i < j);
    const int $r22 = B();
    const int $r20 = ($r21 && $r22);
    const int $r24 = (i >= j);
    const int $r23 = (!$r24);
    const int $r19 = ($r20 || $r23);
    b = $r19;
    $output("iAiArAb", i, $1, j, $1, r, $1, b);
}
