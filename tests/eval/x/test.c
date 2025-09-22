#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
struct
{
    struct
    {
        int data[2 - 1 + 1];
    } data[4 - 1 + 1];
} m = {0};
struct
{
    int data[100 - 1 + 1];
} r = {0};
STR s = {0};
struct
{
    struct
    {
        struct
        {
            int a;
            double b;
            STR c;
            struct
            {
                int data[2 - 1 + 1];
            } d;
        } data[4 - 1 + 1];
    } data[2 - 1 + 1];
} t = {0};
STR $0 = { .data = "=" };
STR $1 = { .data = "i=" };
STR $2 = { .data = " " };
STR $3 = { .data = "s=" };
STR $4 = { .data = "v=[" };
STR $5 = { .data = "]" };
STR $6 = { .data = "456" };
STR $7 = { .data = "123" };
STR $8 = { .data = "X" };
STR $9 = { .data = "M" };
STR $10 = { .data = "S" };
STR $F = { .data = "tests/eval/test.easy" };
STR F1(int x, STR s)
{
    const auto $r38 = LENGTH(s);
    const auto $r37 = $concat("AAiAi", s, $0, $r38, $0, x);
    return $r37;
}
void P1(int i, STR s, STR v)
{
    $output("AiAAAAAAA", $1, i, $2, $3, s, $2, $4, v, $5);
}
int main()
{
    $index(1, 1, 2, &$F, 24, 9);
    $index(2, 1, 4, &$F, 24, 12);
    $index(1, 1, 2, &$F, 24, 17);
    t.data[(1) - (1)].data[(2) - (1)].d.data[(1) - (1)] = 42;
    $index(1, 1, 2, &$F, 25, 12);
    $index(2, 1, 4, &$F, 25, 15);
    $index(1, 1, 2, &$F, 25, 20);
    $output("i", t.data[(1) - (1)].data[(2) - (1)].d.data[(1) - (1)]);
    const auto $r1 = F1(456, $6);
    s = $r1;
    const auto $r2 = F1(456, $6);
    P1(123, $7, $r2);
    a = 2;
    const auto $r4 = (a / 2);
    const auto $r3 = FIX($r4);
    $index($r3, 1, 100, &$F, 32, 9);
    r.data[($r3) - (1)] = 1;
    a = 0;
    const auto $r5 = (a + 1);
    $index($r5, 1, 4, &$F, 35, 9);
    const auto $r6 = (a + 2);
    $index($r6, 1, 2, &$F, 35, 14);
    m.data[($r5) - (1)].data[($r6) - (1)] = 42;
    const auto $r9 = (a + 1);
    $index($r9, 1, 4, &$F, 36, 12);
    const auto $r10 = (a + 2);
    $index($r10, 1, 2, &$F, 36, 17);
    $output("i", m.data[($r9) - (1)].data[($r10) - (1)]);
    const auto $r16 = (a + 1);
    const auto $r18 = (42 * 3);
    const auto $r17 = ($r18 % 3);
    const auto $r15 = ($r16 + $r17);
    const auto $r19 = (7 / 2);
    const auto $r14 = ($r15 - $r19);
    const auto $r13 = ($r14 + 2);
    const auto $r12 = ($r13 + 3);
    const auto $r11 = ($r12 + 69);
    a = $r11;
    $output("ii", a, 123);
    const auto $r20 = (1 + 2);
    a = $r20;
    while (1)
    {
        const auto $r22 = (10 * 2);
        const auto $r21 = ($r22 / 2);
        const auto $r25 = (1 * 2);
        const auto $r24 = ($r25 * 10);
        const auto $r23 = ($r24 / 3);
        if (!(a <= $r21)) break;
        $output("i", a);
        a += $r23;
    }
    const auto $r28 = (a || 7);
    const auto $r27 = (!$r28);
    const auto $r26 = ($r27 && 3);
    a = $r26;
    const auto $r30 = (!2);
    const auto $r29 = (!$r30);
    a = $r29;
    const auto $r32 = (a + 123);
    const auto $r31 = (a < $r32);
    if ($r31)
    {
        a = 0;
    }
    const auto $r35 = (3 % 2);
    const auto $r34 = (100 + $r35);
    const auto $r33 = (a > $r34);
    if ($r33)
    {
        $output("A", $8);
    }
    else
    {
        const auto $r36 = (a > 50);
        if ($r36)
        {
            $output("A", $9);
        }
        else
        {
            $output("A", $10);
        }
    }
}
