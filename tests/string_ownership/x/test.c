#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    STR text;
} Box;
typedef struct
{
    Box data[2 - 0 + 1];
} Boxes;
typedef struct
{
    STR *data;
} Dynamic;
static void $manage1(const void *value, int change)
{
    typedef struct
    {
        STR text;
    } T;
    manage_string((const char *)value + offsetof(T, text), change);
}
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        Box data[2 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage1((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
STR s = {0};
STR t = {0};
Boxes a = {0};
Boxes b = {0};
int i = 0;
int n = 0;
int empty = 0;
STR $F = { .data = "tests/string_ownership/test.easy", .sz = 32 };
STR $1 = { .data = "!", .sz = 1 };
STR $2 = { .data = "changed", .sz = 7 };
STR $3 = { .data = "B", .sz = 1 };
STR $4 = { .data = "", .sz = 0 };
STR $5 = { .data = "done", .sz = 4 };
STR identity(STR value)
{
    manage_string(&value, 1);
    OWN(value, manage_string, $r77);
    manage_string(&value, 1);
    return value;
}
Boxes change(Boxes value)
{
    $manage0(&value, 1);
    OWN(value, $manage0, $r78);
    {
        $index(0, 0, 2, &$F, 15, 15);
        $index(0, 0, 2, &$F, 15, 32);
        $index(0, 0, 2, &$F, 15, 32);
        const STR $r79 = $concat("AA", value.data[(0) - (0)].text, $1);
        OWN($r79, manage_string, $r80);
        __auto_type $r81 = $r79;
        manage_string(&$r81, 1);
        manage_string(&value.data[(0) - (0)].text, -1);
        value.data[(0) - (0)].text = $r81;
    }
    $manage0(&value, 1);
    return value;
}
Dynamic dynamic()
{
    const int $r83 = (1 + 1);
    void *$r82 = make_array(($r83 - 0 + 1), sizeof(STR), manage_string);
    Dynamic values = { .data = $r82 };
    OWN(values, manage_array, $r84);
    {
        const int $r85 = (1 + 1);
        $index(0, 0, $r85, &$F, 21, 16);
        const STR $r86 = CHARACTER(68);
        OWN($r86, manage_string, $r87);
        __auto_type $r88 = $r86;
        manage_string(&$r88, 1);
        manage_string(&values.data[(0) - (0)], -1);
        values.data[(0) - (0)] = $r88;
    }
    manage_array(&values, 1);
    return values;
}
void modify(STR value)
{
    manage_string(&value, 1);
    OWN(value, manage_string, $r89);
    {
        const STR $r90 = $concat("AA", value, $2);
        OWN($r90, manage_string, $r91);
        __auto_type $r92 = $r90;
        manage_string(&$r92, 1);
        manage_string(&value, -1);
        value = $r92;
    }
}
int main_program()
{
    OWN(s, manage_string, $r5);
    OWN(t, manage_string, $r6);
    OWN(a, $manage0, $r7);
    OWN(b, $manage0, $r8);
    {
        const STR $r9 = CHARACTER(65);
        OWN($r9, manage_string, $r10);
        __auto_type $r11 = $r9;
        manage_string(&$r11, 1);
        manage_string(&s, -1);
        s = $r11;
    }
    __auto_type $r12 = s;
    manage_string(&$r12, 1);
    manage_string(&t, -1);
    t = $r12;
    __auto_type $r13 = s;
    manage_string(&$r13, 1);
    manage_string(&s, -1);
    s = $r13;
    modify(s);
    {
        const STR $r15 = $concat("AA", s, $3);
        OWN($r15, manage_string, $r16);
        const STR $r14 = identity($r15);
        OWN($r14, manage_string, $r17);
        __auto_type $r18 = $r14;
        manage_string(&$r18, 1);
        manage_string(&s, -1);
        s = $r18;
    }
    {
        const STR $r19 = $concat("AA", s, t);
        OWN($r19, manage_string, $r20);
        $output("A", $r19);
    }
    $index(0, 0, 2, &$F, 35, 9);
    __auto_type $r21 = s;
    manage_string(&$r21, 1);
    manage_string(&a.data[(0) - (0)].text, -1);
    a.data[(0) - (0)].text = $r21;
    __auto_type $r22 = a;
    $manage0(&$r22, 1);
    $manage0(&b, -1);
    b = $r22;
    {
        const Boxes $r23 = change(a);
        OWN($r23, $manage0, $r24);
        __auto_type $r25 = $r23;
        $manage0(&$r25, 1);
        $manage0(&a, -1);
        a = $r25;
    }
    __auto_type $r26 = a;
    $manage0(&$r26, 1);
    $manage0(&a, -1);
    a = $r26;
    {
        $index(0, 0, 2, &$F, 39, 12);
        $index(0, 0, 2, &$F, 39, 12);
        $index(0, 0, 2, &$F, 39, 25);
        $index(0, 0, 2, &$F, 39, 25);
        const STR $r27 = $concat("AA", a.data[(0) - (0)].text, b.data[(0) - (0)].text);
        OWN($r27, manage_string, $r28);
        $output("A", $r27);
    }
    {
        i = 1;
        while (1)
        {
            if (!(i <= 10000)) break;
            {
                const STR $r31 = CHARACTER(65);
                OWN($r31, manage_string, $r32);
                const STR $r33 = CHARACTER(66);
                OWN($r33, manage_string, $r34);
                const STR $r30 = $concat("AA", $r31, $r33);
                OWN($r30, manage_string, $r35);
                const STR $r29 = identity($r30);
                OWN($r29, manage_string, $r36);
                __auto_type $r37 = $r29;
                manage_string(&$r37, 1);
                manage_string(&s, -1);
                s = $r37;
            }
            {
                const STR $r38 = SUBSTR(s, 0, 0);
                OWN($r38, manage_string, $r39);
                __auto_type $r40 = $r38;
                manage_string(&$r40, 1);
                manage_string(&t, -1);
                t = $r40;
            }
            $index(1, 0, 2, &$F, 44, 11);
            __auto_type $r41 = s;
            manage_string(&$r41, 1);
            manage_string(&a.data[(1) - (0)].text, -1);
            a.data[(1) - (0)].text = $r41;
            __auto_type $r42 = a;
            $manage0(&$r42, 1);
            $manage0(&b, -1);
            b = $r42;
            i += 1;
        }
    }
    {
        const int $r44 = (1 + 1);
        void *$r43 = make_array(($r44 - 0 + 1), sizeof(STR), manage_string);
        Dynamic values = { .data = $r43 };
        OWN(values, manage_array, $r45);
        const int $r47 = (1 + 1);
        void *$r46 = make_array(($r47 - 0 + 1), sizeof(STR), manage_string);
        Dynamic copy = { .data = $r46 };
        OWN(copy, manage_array, $r48);
        {
            const Dynamic $r49 = dynamic();
            OWN($r49, manage_array, $r50);
            __auto_type $r51 = $r49;
            manage_array(&$r51, 1);
            manage_array(&values, -1);
            values = $r51;
        }
        __auto_type $r52 = values;
        manage_array(&$r52, 1);
        manage_array(&copy, -1);
        copy = $r52;
        __auto_type $r53 = values;
        manage_array(&$r53, 1);
        manage_array(&values, -1);
        values = $r53;
        {
            const int $r54 = (1 + 1);
            $index(0, 0, $r54, &$F, 53, 16);
            const int $r56 = (1 + 1);
            $index(0, 0, $r56, &$F, 53, 29);
            const int $r57 = (1 + 1);
            $index(0, 0, $r57, &$F, 53, 29);
            const STR $r55 = $concat("AA", values.data[(0) - (0)], $1);
            OWN($r55, manage_string, $r58);
            __auto_type $r59 = $r55;
            manage_string(&$r59, 1);
            manage_string(&values.data[(0) - (0)], -1);
            values.data[(0) - (0)] = $r59;
        }
        const int $r60 = (1 + 1);
        $index(0, 0, $r60, &$F, 54, 17);
        const int $r61 = (1 + 1);
        $index(0, 0, $r61, &$F, 54, 17);
        $output("A", copy.data[(0) - (0)]);
    }
    {
        const STR $r64 = SUBSTR(t, 0, 0);
        OWN($r64, manage_string, $r65);
        const STR $r63 = $concat("AA", $r64, t);
        OWN($r63, manage_string, $r66);
        const int $r62 = LENGTH($r63);
        $output("i", $r62);
    }
    const int $r67 = t.sz == $4.sz && (t.sz ? memcmp(t.data, $4.data, t.sz) : 0) == 0;
    empty = $r67;
    $output("b", empty);
    n = 2;
    {
        void *$r68 = make_array((n - 0 + 1), sizeof(Box), $manage1);
        struct
        {
            Box *data;
        } values = { .data = $r68 };
        OWN(values, manage_array, $r69);
        $index(0, 0, n, &$F, 64, 16);
        __auto_type $r70 = s;
        manage_string(&$r70, 1);
        manage_string(&values.data[(0) - (0)].text, -1);
        values.data[(0) - (0)].text = $r70;
        n = 1;
        $index(0, 0, n, &$F, 66, 19);
        $index(0, 0, n, &$F, 66, 19);
        $output("A", values.data[(0) - (0)].text);
    }
    i = 0;
    again:
    {
        STR local = {0};
        OWN(local, manage_string, $r71);
        {
            const STR $r72 = CHARACTER(67);
            OWN($r72, manage_string, $r73);
            __auto_type $r74 = $r72;
            manage_string(&$r74, 1);
            manage_string(&local, -1);
            local = $r74;
        }
        const int $r75 = (i + 1);
        i = $r75;
        const int $r76 = (i < 100);
        if ($r76)
        {
            goto again;
        }
        $output("A", local);
        goto done;
    }
    done:
    $output("A", $5);
}
