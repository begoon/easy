#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
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
} A;
A a = {0};
A b = {0};
STR $F = { .data = "tests/array3/test.easy", .sz = 22, .immutable = 1 };
STR $1 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $2 = { .data = " ", .sz = 1, .immutable = 1 };
int main_program()
{
    $index(1, 0, 8, &$F, 11, 9);
    $index(2, 0, 2, &$F, 11, 12);
    $index(3, 0, 4, &$F, 11, 15);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i = 123;
    $index(1, 0, 8, &$F, 12, 9);
    $index(2, 0, 2, &$F, 12, 12);
    $index(3, 0, 4, &$F, 12, 15);
    a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s = $1;
    b = a;
    $index(1, 0, 8, &$F, 14, 12);
    $index(2, 0, 2, &$F, 14, 15);
    $index(3, 0, 4, &$F, 14, 18);
    $index(1, 0, 8, &$F, 14, 12);
    $index(2, 0, 2, &$F, 14, 15);
    $index(3, 0, 4, &$F, 14, 18);
    $index(1, 0, 8, &$F, 14, 31);
    $index(2, 0, 2, &$F, 14, 34);
    $index(3, 0, 4, &$F, 14, 37);
    $index(1, 0, 8, &$F, 14, 31);
    $index(2, 0, 2, &$F, 14, 34);
    $index(3, 0, 4, &$F, 14, 37);
    $output("iAi", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i, $2, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].i);
    $index(1, 0, 8, &$F, 15, 12);
    $index(2, 0, 2, &$F, 15, 15);
    $index(3, 0, 4, &$F, 15, 18);
    $index(1, 0, 8, &$F, 15, 12);
    $index(2, 0, 2, &$F, 15, 15);
    $index(3, 0, 4, &$F, 15, 18);
    $index(1, 0, 8, &$F, 15, 35);
    $index(2, 0, 2, &$F, 15, 38);
    $index(3, 0, 4, &$F, 15, 41);
    $index(1, 0, 8, &$F, 15, 35);
    $index(2, 0, 2, &$F, 15, 38);
    $index(3, 0, 4, &$F, 15, 41);
    const STR $r1 = $concat("AAA", a.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s, $2, b.data[(1) - (0)].data[(2) - (0)].data[(3) - (0)].s);
    $output("A", $r1);
    $exit();
}
