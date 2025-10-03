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
STR $F = { .data = "tests/eval/test.easy", .sz = 20, .immutable = 1 };
STR $7 = { .data = "456", .sz = 3, .immutable = 1 };
STR $8 = { .data = "123", .sz = 3, .immutable = 1 };
STR $9 = { .data = "X", .sz = 1, .immutable = 1 };
STR $10 = { .data = "M", .sz = 1, .immutable = 1 };
STR $11 = { .data = "S", .sz = 1, .immutable = 1 };
STR F1(int x, STR s)
{
    const int $r45 = LENGTH(s);
    const STR $r44 = $concat("AAiAi", s, $0, $r45, $0, x);
    return $r44;
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
    const STR $r7 = F1(456, $7);
    s = $r7;
    const STR $r8 = F1(456, $7);
    P1(123, $8, $r8);
    a = 2;
    const int $r10 = (a / 2);
    const int $r9 = FIX($r10);
    $index($r9, 1, 100, &$F, 33, 9);
    r.data[($r9) - (1)] = 1;
    a = 0;
    const int $r11 = (a + 1);
    $index($r11, 1, 4, &$F, 36, 9);
    const int $r12 = (a + 2);
    $index($r12, 1, 2, &$F, 36, 14);
    m.data[($r11) - (1)].data[($r12) - (1)] = 42;
    const int $r13 = (a + 1);
    $index($r13, 1, 4, &$F, 37, 12);
    const int $r14 = (a + 2);
    $index($r14, 1, 2, &$F, 37, 17);
    const int $r15 = (a + 1);
    $index($r15, 1, 4, &$F, 37, 12);
    const int $r16 = (a + 2);
    $index($r16, 1, 2, &$F, 37, 17);
    $output("i", m.data[($r15) - (1)].data[($r16) - (1)]);
    const int $r22 = (a + 1);
    const int $r24 = (42 * 3);
    const int $r23 = ($r24 % 3);
    const int $r21 = ($r22 + $r23);
    const int $r25 = (7 / 2);
    const int $r20 = ($r21 - $r25);
    const int $r19 = ($r20 + 2);
    const int $r18 = ($r19 + 3);
    const int $r17 = ($r18 + 69);
    a = $r17;
    $output("ii", a, 123);
    const int $r26 = (1 + 2);
    a = $r26;
    while (1)
    {
        const int $r28 = (10 * 2);
        const int $r27 = ($r28 / 2);
        const int $r31 = (1 * 2);
        const int $r30 = ($r31 * 10);
        const int $r29 = ($r30 / 3);
        if (!(a <= $r27)) break;
        $output("i", a);
        a += $r29;
    }
    const int $r35 = (a > 1);
    const int $r34 = (b || $r35);
    const int $r33 = (!$r34);
    const int $r32 = ($r33 && TRUE);
    b = $r32;
    const int $r37 = (!FALSE);
    const int $r36 = (!$r37);
    b = $r36;
    const int $r39 = (a + 123);
    const int $r38 = (a < $r39);
    if ($r38)
    {
        a = 0;
    }
    const int $r42 = (3 % 2);
    const int $r41 = (100 + $r42);
    const int $r40 = (a > $r41);
    if ($r40)
    {
        $output("A", $9);
    }
    else
    {
        const int $r43 = (a > 50);
        if ($r43)
        {
            $output("A", $10);
        }
        else
        {
            $output("A", $11);
        }
    }
}
