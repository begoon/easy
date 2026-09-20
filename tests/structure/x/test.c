#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int a;
    STR b;
} T;
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        int a;
        STR b;
    } T;
    manage_string((const char *)value + offsetof(T, b), change);
}
T t = {0};
STR $0 = { .data = ".", .sz = 1 };
STR $F = { .data = "tests/structure/test.easy", .sz = 25 };
int main_program()
{
    OWN(t, $manage0, $r1);
    t.a = 1;
    $output("A", $0);
}
