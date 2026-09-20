#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    STR x;
} T;
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        STR x;
    } T;
    manage_string((const char *)value + offsetof(T, x), change);
}
static void $manage1(const void *value, int change)
{
    typedef struct
    {
        STR data[10485760 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        manage_string((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
T t = {0};
struct
{
    STR data[10485760 - 0 + 1];
} huge = {0};
int i = 0;
STR $0 = { .data = "xyz", .sz = 3 };
STR $1 = { .data = "  ", .sz = 2 };
STR $F = { .data = "tests/string_alloc/test.easy", .sz = 28 };
STR $3 = { .data = "abcdefghijklm", .sz = 13 };
STR $4 = { .data = "nopqrstuvwxyz", .sz = 13 };
T abc()
{
    T a = {0};
    OWN(a, $manage0, $r11);
    __auto_type $r12 = $0;
    manage_string(&$r12, 1);
    manage_string(&a.x, -1);
    a.x = $r12;
    {
        const STR $r13 = CHARACTER(100);
        OWN($r13, manage_string, $r14);
        __auto_type $r15 = $r13;
        manage_string(&$r15, 1);
        manage_string(&a.x, -1);
        a.x = $r15;
    }
    $manage0(&a, 1);
    return a;
}
int main_program()
{
    OWN(t, $manage0, $r1);
    OWN(huge, $manage1, $r2);
    {
        const T $r3 = abc();
        OWN($r3, $manage0, $r4);
        __auto_type $r5 = $r3;
        $manage0(&$r5, 1);
        $manage0(&t, -1);
        t = $r5;
    }
    {
        const STR $r6 = CHARACTER(101);
        OWN($r6, manage_string, $r7);
        $output("AAA", t.x, $r6, $1);
    }
    {
        i = 0;
        while (1)
        {
            if (!(i <= 10485759)) break;
            {
                $index(i, 0, 10485760, &$F, 21, 14);
                const STR $r8 = $concat("AA", $3, $4);
                OWN($r8, manage_string, $r9);
                __auto_type $r10 = $r8;
                manage_string(&$r10, 1);
                manage_string(&huge.data[(size_t)((int64_t)(i) - (0))], -1);
                huge.data[(size_t)((int64_t)(i) - (0))] = $r10;
            }
            i += 1;
        }
    }
}
