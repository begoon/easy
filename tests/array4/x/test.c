#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
static void $manage3(const void *value, int change)
{
    typedef struct
    {
        int i;
        STR s;
    } T;
    manage_string((const char *)value + offsetof(T, s), change);
}
static void $manage2(const void *value, int change)
{
    typedef struct
    {
        struct
        {
            int i;
            STR s;
        } data[4 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage3((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
static void $manage1(const void *value, int change)
{
    typedef struct
    {
        struct
        {
            struct
            {
                int i;
                STR s;
            } data[4 - 0 + 1];
        } data[2 - 0 + 1];
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
                struct
                {
                    int i;
                    STR s;
                } data[4 - 0 + 1];
            } data[2 - 0 + 1];
        } data[8 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage1((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
struct
{
    struct
    {
        struct
        {
            struct
            {
                int i;
                STR s;
            } data[4 - 0 + 1];
        } data[2 - 0 + 1];
    } data[8 - 0 + 1];
} a = {0};
struct
{
    struct
    {
        struct
        {
            struct
            {
                int i;
                STR s;
            } data[4 - 0 + 1];
        } data[2 - 0 + 1];
    } data[8 - 0 + 1];
} b = {0};
STR $F = { .data = "tests/array4/test.easy", .sz = 22 };
STR $1 = { .data = "abc", .sz = 3 };
STR $2 = { .data = " ", .sz = 1 };
int main_program()
{
    OWN(a, $manage0, $r1);
    OWN(b, $manage0, $r2);
    $index(1, 0, 8, &$F, 10, 9);
    $index(2, 0, 2, &$F, 10, 12);
    $index(3, 0, 4, &$F, 10, 15);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i = 123;
    $index(1, 0, 8, &$F, 11, 9);
    $index(2, 0, 2, &$F, 11, 12);
    $index(3, 0, 4, &$F, 11, 15);
    __auto_type $r3 = $1;
    manage_string(&$r3, 1);
    manage_string(&a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s, -1);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s = $r3;
    $index(1, 0, 8, &$F, 12, 12);
    $index(2, 0, 2, &$F, 12, 15);
    $index(3, 0, 4, &$F, 12, 18);
    $index(1, 0, 8, &$F, 12, 12);
    $index(2, 0, 2, &$F, 12, 15);
    $index(3, 0, 4, &$F, 12, 18);
    $index(1, 0, 8, &$F, 12, 31);
    $index(2, 0, 2, &$F, 12, 34);
    $index(3, 0, 4, &$F, 12, 37);
    $index(1, 0, 8, &$F, 12, 31);
    $index(2, 0, 2, &$F, 12, 34);
    $index(3, 0, 4, &$F, 12, 37);
    $output("iAi", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i, $2, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i);
    {
        $index(1, 0, 8, &$F, 13, 12);
        $index(2, 0, 2, &$F, 13, 15);
        $index(3, 0, 4, &$F, 13, 18);
        $index(1, 0, 8, &$F, 13, 12);
        $index(2, 0, 2, &$F, 13, 15);
        $index(3, 0, 4, &$F, 13, 18);
        $index(1, 0, 8, &$F, 13, 35);
        $index(2, 0, 2, &$F, 13, 38);
        $index(3, 0, 4, &$F, 13, 41);
        $index(1, 0, 8, &$F, 13, 35);
        $index(2, 0, 2, &$F, 13, 38);
        $index(3, 0, 4, &$F, 13, 41);
        const STR $r4 = $concat("AAA", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s, $2, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s);
        OWN($r4, manage_string, $r5);
        $output("A", $r4);
    }
    $exit();
}
