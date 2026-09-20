#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
static void $manage2(const void *value, int change)
{
    typedef struct
    {
        int a;
        double b;
        STR c;
        struct
        {
            int data[2 - 1 + 1];
        } d;
    } T;
    manage_string((const char *)value + offsetof(T, c), change);
}
static void $manage1(const void *value, int change)
{
    typedef struct
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
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage2((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
static void $manage0(const void *value, int change)
{
    typedef struct
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
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage1((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
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
STR $0 = { .data = "=", .sz = 1 };
STR $1 = { .data = "i=", .sz = 2 };
STR $2 = { .data = " ", .sz = 1 };
STR $3 = { .data = "s=", .sz = 2 };
STR $4 = { .data = "v=[", .sz = 3 };
STR $5 = { .data = "]", .sz = 1 };
STR $F = { .data = "tests/eval/test.easy", .sz = 20 };
STR $7 = { .data = "456", .sz = 3 };
STR $8 = { .data = "123", .sz = 3 };
STR $9 = { .data = "X", .sz = 1 };
STR $10 = { .data = "M", .sz = 1 };
STR $11 = { .data = "S", .sz = 1 };
STR F1(int x, STR s)
{
    manage_string(&s, 1);
    OWN(s, manage_string, $r49);
    {
        const int $r51 = LENGTH(s);
        const STR $r50 = $concat("AAiAi", s, $0, $r51, $0, x);
        OWN($r50, manage_string, $r52);
        manage_string(&$r50, 1);
        return $r50;
    }
}
void P1(int i, STR s, STR v)
{
    manage_string(&s, 1);
    OWN(s, manage_string, $r53);
    manage_string(&v, 1);
    OWN(v, manage_string, $r54);
    $output("AiAAAAAAA", $1, i, $2, $3, s, $2, $4, v, $5);
}
int main_program()
{
    OWN(s, manage_string, $r7);
    OWN(t, $manage0, $r8);
    $index(1, 1, 2, &$F, 25, 9);
    $index(2, 1, 4, &$F, 25, 12);
    $index(1, 1, 2, &$F, 25, 17);
    t.data[(size_t)((int64_t)(1) - (1))].data[(size_t)((int64_t)(2) - (1))].d.data[(size_t)((int64_t)(1) - (1))] = 42;
    $index(1, 1, 2, &$F, 26, 12);
    $index(2, 1, 4, &$F, 26, 15);
    $index(1, 1, 2, &$F, 26, 20);
    $index(1, 1, 2, &$F, 26, 12);
    $index(2, 1, 4, &$F, 26, 15);
    $index(1, 1, 2, &$F, 26, 20);
    $output("i", t.data[(size_t)((int64_t)(1) - (1))].data[(size_t)((int64_t)(2) - (1))].d.data[(size_t)((int64_t)(1) - (1))]);
    {
        const STR $r9 = F1(456, $7);
        OWN($r9, manage_string, $r10);
        __auto_type $r11 = $r9;
        manage_string(&$r11, 1);
        manage_string(&s, -1);
        s = $r11;
    }
    {
        const STR $r12 = F1(456, $7);
        OWN($r12, manage_string, $r13);
        P1(123, $8, $r12);
    }
    a = 2;
    const int $r15 = (a / 2);
    const int $r14 = FIX($r15);
    $index($r14, 1, 100, &$F, 33, 9);
    r.data[(size_t)((int64_t)($r14) - (1))] = 1;
    a = 0;
    const int $r16 = (a + 1);
    $index($r16, 1, 4, &$F, 36, 9);
    const int $r17 = (a + 2);
    $index($r17, 1, 2, &$F, 36, 14);
    m.data[(size_t)((int64_t)($r16) - (1))].data[(size_t)((int64_t)($r17) - (1))] = 42;
    const int $r18 = (a + 1);
    $index($r18, 1, 4, &$F, 37, 12);
    const int $r19 = (a + 2);
    $index($r19, 1, 2, &$F, 37, 17);
    const int $r20 = (a + 1);
    $index($r20, 1, 4, &$F, 37, 12);
    const int $r21 = (a + 2);
    $index($r21, 1, 2, &$F, 37, 17);
    $output("i", m.data[(size_t)((int64_t)($r20) - (1))].data[(size_t)((int64_t)($r21) - (1))]);
    const int $r27 = (a + 1);
    const int $r29 = (42 * 3);
    const int $r28 = ($r29 % 3);
    const int $r26 = ($r27 + $r28);
    const int $r30 = (7 / 2);
    const int $r25 = ($r26 - $r30);
    const int $r24 = ($r25 + 2);
    const int $r23 = ($r24 + 3);
    const int $r22 = ($r23 + 69);
    a = $r22;
    $output("ii", a, 123);
    const int $r31 = (1 + 2);
    a = $r31;
    while (1)
    {
        const int $r33 = (10 * 2);
        const int $r32 = ($r33 / 2);
        const int $r36 = (1 * 2);
        const int $r35 = ($r36 * 10);
        const int $r34 = ($r35 / 3);
        if (!(a <= $r32)) break;
        $output("i", a);
        a += $r34;
    }
    const int $r40 = (a > 1);
    const int $r39 = (b || $r40);
    const int $r38 = (!$r39);
    const int $r37 = ($r38 && TRUE);
    b = $r37;
    const int $r42 = (!FALSE);
    const int $r41 = (!$r42);
    b = $r41;
    const int $r44 = (a + 123);
    const int $r43 = (a < $r44);
    if ($r43)
    {
        a = 0;
    }
    const int $r47 = (3 % 2);
    const int $r46 = (100 + $r47);
    const int $r45 = (a > $r46);
    if ($r45)
    {
        $output("A", $9);
    }
    else
    {
        const int $r48 = (a > 50);
        if ($r48)
        {
            $output("A", $10);
        }
        else
        {
            $output("A", $11);
        }
    }
}
