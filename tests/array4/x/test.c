#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
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
STR $0 = { .data = "abc" };
STR $1 = { .data = " " };
STR $F = { .data = "tests/array4/test.easy" };
int main()
{
    $index(1, 0, 8, &$F, 10, 9);
    $index(2, 0, 2, &$F, 10, 12);
    $index(3, 0, 4, &$F, 10, 15);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i = 123;
    $index(1, 0, 8, &$F, 11, 9);
    $index(2, 0, 2, &$F, 11, 12);
    $index(3, 0, 4, &$F, 11, 15);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s = $0;
    $index(1, 0, 8, &$F, 12, 12);
    $index(2, 0, 2, &$F, 12, 15);
    $index(3, 0, 4, &$F, 12, 18);
    $index(1, 0, 8, &$F, 12, 31);
    $index(2, 0, 2, &$F, 12, 34);
    $index(3, 0, 4, &$F, 12, 37);
    $output("iAi", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i, $1, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i);
    $index(1, 0, 8, &$F, 13, 12);
    $index(2, 0, 2, &$F, 13, 15);
    $index(3, 0, 4, &$F, 13, 18);
    $index(1, 0, 8, &$F, 13, 35);
    $index(2, 0, 2, &$F, 13, 38);
    $index(3, 0, 4, &$F, 13, 41);
    const STR $r1 = $concat("AAA", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s, $1, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s);
    $output("A", $r1);
    exit(0);
}
