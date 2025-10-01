#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    STR x;
} T;
T b = {0};
STR $0 = { .data = "**", .sz = 2, .immutable = 1 };
STR $F = { .data = "tests/string3/test.easy", .sz = 23, .immutable = 1 };
int main_program()
{
    {
        T a = {0};
        const STR $r1 = CHARACTER(100);
        a.x = $r1;
        $output("A", a.x);
        b = a;
    }
    $output("AA", b.x, $0);
}
