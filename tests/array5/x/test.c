#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        int score;
        STR name;
    } data[100 - 1 + 1];
} Field;
static void $manage1(const void *value, int change)
{
    typedef struct
    {
        int score;
        STR name;
    } T;
    manage_string((const char *)value + offsetof(T, name), change);
}
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        struct
        {
            int score;
            STR name;
        } data[100 - 1 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage1((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
Field f = {0};
STR $0 = { .data = "BEFORE: ", .sz = 8 };
STR $F = { .data = "tests/array5/test.easy", .sz = 22 };
STR $2 = { .data = " ", .sz = 1 };
STR $3 = { .data = "xyz", .sz = 3 };
STR $4 = { .data = "abc", .sz = 3 };
STR $5 = { .data = "AFTER: ", .sz = 7 };
Field a(Field f)
{
    $manage0(&f, 1);
    OWN(f, $manage0, $r8);
    {
        $index(10, 1, 100, &$F, 10, 28);
        $index(10, 1, 100, &$F, 10, 28);
        $index(10, 1, 100, &$F, 10, 50);
        $index(10, 1, 100, &$F, 10, 50);
        const STR $r9 = $concat("AiAA", $0, f.data[(10) - (1)].score, $2, f.data[(10) - (1)].name);
        OWN($r9, manage_string, $r10);
        $output("A", $r9);
    }
    $index(10, 1, 100, &$F, 11, 11);
    f.data[(10) - (1)].score = 123;
    {
        $index(10, 1, 100, &$F, 12, 11);
        $index(10, 1, 100, &$F, 12, 25);
        $index(10, 1, 100, &$F, 12, 25);
        const STR $r11 = $concat("AA", f.data[(10) - (1)].name, $3);
        OWN($r11, manage_string, $r12);
        __auto_type $r13 = $r11;
        manage_string(&$r13, 1);
        manage_string(&f.data[(10) - (1)].name, -1);
        f.data[(10) - (1)].name = $r13;
    }
    $manage0(&f, 1);
    return f;
}
int main_program()
{
    OWN(f, $manage0, $r1);
    $index(10, 1, 100, &$F, 16, 9);
    f.data[(10) - (1)].score = 321;
    $index(10, 1, 100, &$F, 17, 9);
    __auto_type $r2 = $4;
    manage_string(&$r2, 1);
    manage_string(&f.data[(10) - (1)].name, -1);
    f.data[(10) - (1)].name = $r2;
    {
        const Field $r3 = a(f);
        OWN($r3, $manage0, $r4);
        __auto_type $r5 = $r3;
        $manage0(&$r5, 1);
        $manage0(&f, -1);
        f = $r5;
    }
    {
        $index(10, 1, 100, &$F, 19, 25);
        $index(10, 1, 100, &$F, 19, 25);
        $index(10, 1, 100, &$F, 19, 47);
        $index(10, 1, 100, &$F, 19, 47);
        const STR $r6 = $concat("AiAA", $5, f.data[(10) - (1)].score, $2, f.data[(10) - (1)].name);
        OWN($r6, manage_string, $r7);
        $output("A", $r6);
    }
    $exit();
}
