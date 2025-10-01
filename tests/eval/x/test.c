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
int b = 0;
STR $0 = { .data = "=", .sz = 1, .immutable = 1 };
STR $1 = { .data = "i=", .sz = 2, .immutable = 1 };
STR $2 = { .data = " ", .sz = 1, .immutable = 1 };
STR $3 = { .data = "s=", .sz = 2, .immutable = 1 };
STR $4 = { .data = "v=[", .sz = 3, .immutable = 1 };
STR $5 = { .data = "]", .sz = 1, .immutable = 1 };
STR $6 = { .data = "456", .sz = 3, .immutable = 1 };
STR $7 = { .data = "123", .sz = 3, .immutable = 1 };
STR $8 = { .data = "X", .sz = 1, .immutable = 1 };
STR $9 = { .data = "M", .sz = 1, .immutable = 1 };
STR $10 = { .data = "S", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/eval/test.easy", .sz = 20, .immutable = 1 };
STR F1(int x, STR s)
{
    const int $r39 = LENGTH(s);
    const STR $r38 = $concat("AAiAi", s, $0, $r39, $0, x);
    return $r38;
}
void P1(int i, STR s, STR v)
{
    $output("AiAAAAAAA", $1, i, $2, $3, s, $2, $4, v, $5);
}
int main_program()
{
    $index(1, 1, 2, &$F, 25, 9);
    $index(2, 1, 4, &$F, 25, 12);
    $index(1, 1, 2, &$F, 25, 17);
    t.data[(1) - (1)].data[(2) - (1)].d.data[(1) - (1)] = 42;
    $index(1, 1, 2, &$F, 26, 12);
    $index(2, 1, 4, &$F, 26, 15);
    $index(1, 1, 2, &$F, 26, 20);
    $index(1, 1, 2, &$F, 26, 12);
    $index(2, 1, 4, &$F, 26, 15);
    $index(1, 1, 2, &$F, 26, 20);
    $output("i", t.data[(1) - (1)].data[(2) - (1)].d.data[(1) - (1)]);
    const STR $r1 = F1(456, $6);
    s = $r1;
    const STR $r2 = F1(456, $6);
    P1(123, $7, $r2);
    a = 2;
    const int $r4 = (a / 2);
    const int $r3 = FIX($r4);
    $index($r3, 1, 100, &$F, 33, 9);
    r.data[($r3) - (1)] = 1;
    a = 0;
    const int $r5 = (a + 1);
    $index($r5, 1, 4, &$F, 36, 9);
    const int $r6 = (a + 2);
    $index($r6, 1, 2, &$F, 36, 14);
    m.data[($r5) - (1)].data[($r6) - (1)] = 42;
    const int $r7 = (a + 1);
    $index($r7, 1, 4, &$F, 37, 12);
    const int $r8 = (a + 2);
    $index($r8, 1, 2, &$F, 37, 17);
    const int $r9 = (a + 1);
    $index($r9, 1, 4, &$F, 37, 12);
    const int $r10 = (a + 2);
    $index($r10, 1, 2, &$F, 37, 17);
    $output("i", m.data[($r9) - (1)].data[($r10) - (1)]);
    const int $r16 = (a + 1);
    const int $r18 = (42 * 3);
    const int $r17 = ($r18 % 3);
    const int $r15 = ($r16 + $r17);
    const int $r19 = (7 / 2);
    const int $r14 = ($r15 - $r19);
    const int $r13 = ($r14 + 2);
    const int $r12 = ($r13 + 3);
    const int $r11 = ($r12 + 69);
    a = $r11;
    $output("ii", a, 123);
    const int $r20 = (1 + 2);
    a = $r20;
    while (1)
    {
        const int $r22 = (10 * 2);
        const int $r21 = ($r22 / 2);
        const int $r25 = (1 * 2);
        const int $r24 = ($r25 * 10);
        const int $r23 = ($r24 / 3);
        if (!(a <= $r21)) break;
        $output("i", a);
        a += $r23;
    }
    const int $r29 = (a > 1);
    const int $r28 = (b || $r29);
    const int $r27 = (!$r28);
    const int $r26 = ($r27 && TRUE);
    b = $r26;
    const int $r31 = (!FALSE);
    const int $r30 = (!$r31);
    b = $r30;
    const int $r33 = (a + 123);
    const int $r32 = (a < $r33);
    if ($r32)
    {
        a = 0;
    }
    const int $r36 = (3 % 2);
    const int $r35 = (100 + $r36);
    const int $r34 = (a > $r35);
    if ($r34)
    {
        $output("A", $8);
    }
    else
    {
        const int $r37 = (a > 50);
        if ($r37)
        {
            $output("A", $9);
        }
        else
        {
            $output("A", $10);
        }
    }
}
