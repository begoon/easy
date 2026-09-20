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
    OWN(value, manage_string, $r82);
    manage_string(&value, 1);
    return value;
}
Boxes change(Boxes value)
{
    $manage0(&value, 1);
    OWN(value, $manage0, $r83);
    {
        $index(0, 0, 2, &$F, 15, 15);
        $index(0, 0, 2, &$F, 15, 32);
        $index(0, 0, 2, &$F, 15, 32);
        const STR $r84 = $concat("AA", value.data[(size_t)((int64_t)(0) - (0))].text, $1);
        OWN($r84, manage_string, $r85);
        __auto_type $r86 = $r84;
        manage_string(&$r86, 1);
        manage_string(&value.data[(size_t)((int64_t)(0) - (0))].text, -1);
        value.data[(size_t)((int64_t)(0) - (0))].text = $r86;
    }
    $manage0(&value, 1);
    return value;
}
Dynamic dynamic()
{
    Dynamic values = {0};
    OWN(values, manage_array, $r87);
    const int $r88 = 0;
    const int $r89 = (1 + 1);
    values.data = make_array($r88, $r89, sizeof(STR), manage_string);
    {
        const size_t $r90 = $array_index(values.data, 0, &$F, 21, 16);
        const STR $r91 = CHARACTER(68);
        OWN($r91, manage_string, $r92);
        __auto_type $r93 = $r91;
        manage_string(&$r93, 1);
        manage_string(&values.data[$r90], -1);
        values.data[$r90] = $r93;
    }
    manage_array(&values, 1);
    return values;
}
void modify(STR value)
{
    manage_string(&value, 1);
    OWN(value, manage_string, $r94);
    {
        const STR $r95 = $concat("AA", value, $2);
        OWN($r95, manage_string, $r96);
        __auto_type $r97 = $r95;
        manage_string(&$r97, 1);
        manage_string(&value, -1);
        value = $r97;
    }
}
int main_program()
{
    OWN(s, manage_string, $r7);
    OWN(t, manage_string, $r8);
    OWN(a, $manage0, $r9);
    OWN(b, $manage0, $r10);
    {
        const STR $r11 = CHARACTER(65);
        OWN($r11, manage_string, $r12);
        __auto_type $r13 = $r11;
        manage_string(&$r13, 1);
        manage_string(&s, -1);
        s = $r13;
    }
    __auto_type $r14 = s;
    manage_string(&$r14, 1);
    manage_string(&t, -1);
    t = $r14;
    __auto_type $r15 = s;
    manage_string(&$r15, 1);
    manage_string(&s, -1);
    s = $r15;
    modify(s);
    {
        const STR $r17 = $concat("AA", s, $3);
        OWN($r17, manage_string, $r18);
        const STR $r16 = identity($r17);
        OWN($r16, manage_string, $r19);
        __auto_type $r20 = $r16;
        manage_string(&$r20, 1);
        manage_string(&s, -1);
        s = $r20;
    }
    {
        const STR $r21 = $concat("AA", s, t);
        OWN($r21, manage_string, $r22);
        $output("A", $r21);
    }
    $index(0, 0, 2, &$F, 35, 9);
    __auto_type $r23 = s;
    manage_string(&$r23, 1);
    manage_string(&a.data[(size_t)((int64_t)(0) - (0))].text, -1);
    a.data[(size_t)((int64_t)(0) - (0))].text = $r23;
    __auto_type $r24 = a;
    $manage0(&$r24, 1);
    $manage0(&b, -1);
    b = $r24;
    {
        const Boxes $r25 = change(a);
        OWN($r25, $manage0, $r26);
        __auto_type $r27 = $r25;
        $manage0(&$r27, 1);
        $manage0(&a, -1);
        a = $r27;
    }
    __auto_type $r28 = a;
    $manage0(&$r28, 1);
    $manage0(&a, -1);
    a = $r28;
    {
        $index(0, 0, 2, &$F, 39, 12);
        $index(0, 0, 2, &$F, 39, 12);
        $index(0, 0, 2, &$F, 39, 25);
        $index(0, 0, 2, &$F, 39, 25);
        const STR $r29 = $concat("AA", a.data[(size_t)((int64_t)(0) - (0))].text, b.data[(size_t)((int64_t)(0) - (0))].text);
        OWN($r29, manage_string, $r30);
        $output("A", $r29);
    }
    {
        i = 1;
        while (1)
        {
            if (!(i <= 10000)) break;
            {
                const STR $r33 = CHARACTER(65);
                OWN($r33, manage_string, $r34);
                const STR $r35 = CHARACTER(66);
                OWN($r35, manage_string, $r36);
                const STR $r32 = $concat("AA", $r33, $r35);
                OWN($r32, manage_string, $r37);
                const STR $r31 = identity($r32);
                OWN($r31, manage_string, $r38);
                __auto_type $r39 = $r31;
                manage_string(&$r39, 1);
                manage_string(&s, -1);
                s = $r39;
            }
            {
                const STR $r40 = SUBSTR(s, 0, 0);
                OWN($r40, manage_string, $r41);
                __auto_type $r42 = $r40;
                manage_string(&$r42, 1);
                manage_string(&t, -1);
                t = $r42;
            }
            $index(1, 0, 2, &$F, 44, 11);
            __auto_type $r43 = s;
            manage_string(&$r43, 1);
            manage_string(&a.data[(size_t)((int64_t)(1) - (0))].text, -1);
            a.data[(size_t)((int64_t)(1) - (0))].text = $r43;
            __auto_type $r44 = a;
            $manage0(&$r44, 1);
            $manage0(&b, -1);
            b = $r44;
            i += 1;
        }
    }
    {
        Dynamic values = {0};
        OWN(values, manage_array, $r45);
        const int $r46 = 0;
        const int $r47 = (1 + 1);
        values.data = make_array($r46, $r47, sizeof(STR), manage_string);
        Dynamic copy = {0};
        OWN(copy, manage_array, $r48);
        const int $r49 = 0;
        const int $r50 = (1 + 1);
        copy.data = make_array($r49, $r50, sizeof(STR), manage_string);
        {
            const Dynamic $r51 = dynamic();
            OWN($r51, manage_array, $r52);
            __auto_type $r53 = $r51;
            manage_array(&$r53, 1);
            manage_array(&values, -1);
            values = $r53;
        }
        __auto_type $r54 = values;
        manage_array(&$r54, 1);
        manage_array(&copy, -1);
        copy = $r54;
        __auto_type $r55 = values;
        manage_array(&$r55, 1);
        manage_array(&values, -1);
        values = $r55;
        {
            const size_t $r56 = $array_index(values.data, 0, &$F, 53, 16);
            const size_t $r58 = $array_index(values.data, 0, &$F, 53, 29);
            const size_t $r59 = $array_index(values.data, 0, &$F, 53, 29);
            const STR $r57 = $concat("AA", values.data[$r59], $1);
            OWN($r57, manage_string, $r60);
            __auto_type $r61 = $r57;
            manage_string(&$r61, 1);
            manage_string(&values.data[$r56], -1);
            values.data[$r56] = $r61;
        }
        const size_t $r62 = $array_index(copy.data, 0, &$F, 54, 17);
        const size_t $r63 = $array_index(copy.data, 0, &$F, 54, 17);
        $output("A", copy.data[$r63]);
    }
    {
        const STR $r66 = SUBSTR(t, 0, 0);
        OWN($r66, manage_string, $r67);
        const STR $r65 = $concat("AA", $r66, t);
        OWN($r65, manage_string, $r68);
        const int $r64 = LENGTH($r65);
        $output("i", $r64);
    }
    const int $r69 = t.sz == $4.sz && (t.sz ? memcmp(t.data, $4.data, t.sz) : 0) == 0;
    empty = $r69;
    $output("b", empty);
    n = 2;
    {
        struct
        {
            Box *data;
        } values = {0};
        OWN(values, manage_array, $r70);
        const int $r71 = 0;
        values.data = make_array($r71, n, sizeof(Box), $manage1);
        const size_t $r72 = $array_index(values.data, 0, &$F, 64, 16);
        __auto_type $r73 = s;
        manage_string(&$r73, 1);
        manage_string(&values.data[$r72].text, -1);
        values.data[$r72].text = $r73;
        n = 1;
        const size_t $r74 = $array_index(values.data, 0, &$F, 66, 19);
        const size_t $r75 = $array_index(values.data, 0, &$F, 66, 19);
        $output("A", values.data[$r75].text);
    }
    i = 0;
    again:
    {
        STR local = {0};
        OWN(local, manage_string, $r76);
        {
            const STR $r77 = CHARACTER(67);
            OWN($r77, manage_string, $r78);
            __auto_type $r79 = $r77;
            manage_string(&$r79, 1);
            manage_string(&local, -1);
            local = $r79;
        }
        const int $r80 = (i + 1);
        i = $r80;
        const int $r81 = (i < 100);
        if ($r81)
        {
            goto again;
        }
        $output("A", local);
        goto done;
    }
    done:
    $output("A", $5);
}
