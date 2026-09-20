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
T b = {0};
STR $0 = { .data = "**", .sz = 2 };
STR $F = { .data = "tests/string3/test.easy", .sz = 23 };
int main_program()
{
    OWN(b, $manage0, $r1);
    {
        T a = {0};
        OWN(a, $manage0, $r2);
        {
            const STR $r3 = CHARACTER(100);
            OWN($r3, manage_string, $r4);
            __auto_type $r5 = $r3;
            manage_string(&$r5, 1);
            manage_string(&a.x, -1);
            a.x = $r5;
        }
        $output("A", a.x);
        __auto_type $r6 = a;
        $manage0(&$r6, 1);
        $manage0(&b, -1);
        b = $r6;
    }
    $output("AA", b.x, $0);
}
