#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    STR x;
} T;
T t = {0};
struct
{
    STR data[10485760 - 0 + 1];
} huge = {0};
int i = 0;
STR $0 = { .data = "xyz", .sz = 3, .immutable = 1 };
STR $1 = { .data = "  ", .sz = 2, .immutable = 1 };
STR $F = { .data = "tests/string_alloc/test.easy", .sz = 28, .immutable = 1 };
STR $3 = { .data = "abcdefghijklm", .sz = 13, .immutable = 1 };
STR $4 = { .data = "nopqrstuvwxyz", .sz = 13, .immutable = 1 };
T abc()
{
    T a = {0};
    a.x = $0;
    const STR $r4 = CHARACTER(100);
    a.x = $r4;
    return a;
}
int main_program()
{
    const T $r1 = abc();
    t = $r1;
    const STR $r2 = CHARACTER(101);
    $output("AAA", t.x, $r2, $1);
    i = 0;
    while (1)
    {
        if (!(i <= 10485759)) break;
        $index(i, 0, 10485760, &$F, 21, 14);
        const STR $r3 = $concat("AA", $3, $4);
        huge.data[(i) - (0)] = $r3;
        i += 1;
    }
}
