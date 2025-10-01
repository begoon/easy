#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
struct
{
    int data[2 - 1 + 1];
} b = {0};
struct
{
    struct
    {
        int data[4 - 1 + 1];
    } data[2 - 1 + 1];
} m = {0};
struct
{
    struct
    {
        struct
        {
            int data[8 - 1 + 1];
        } data[4 - 1 + 1];
    } data[2 - 1 + 1];
} c = {0};
struct
{
    struct
    {
        struct
        {
            int i;
        } data[4 - 1 + 1];
    } data[2 - 1 + 1];
} x = {0};
struct
{
    struct
    {
        struct
        {
            int i;
            struct
            {
                int data[4 - 1 + 1];
            } t;
        } data[4 - 1 + 1];
    } data[2 - 1 + 1];
} y = {0};
STR $F = { .data = "tests/array_checked/test.easy", .sz = 29, .immutable = 1 };
int main_program()
{
    $index(1, 1, 2, &$F, 8, 9);
    b.data[(1) - (1)] = 42;
    $index(1, 1, 2, &$F, 9, 12);
    $index(1, 1, 2, &$F, 9, 12);
    $output("i", b.data[(1) - (1)]);
    $index(2, 1, 2, &$F, 11, 9);
    $index(3, 1, 4, &$F, 11, 12);
    m.data[(2) - (1)].data[(3) - (1)] = 99;
    $index(2, 1, 2, &$F, 12, 12);
    $index(3, 1, 4, &$F, 12, 15);
    $index(2, 1, 2, &$F, 12, 12);
    $index(3, 1, 4, &$F, 12, 15);
    $output("i", m.data[(2) - (1)].data[(3) - (1)]);
    $index(1, 1, 2, &$F, 14, 9);
    $index(2, 1, 4, &$F, 14, 12);
    $index(3, 1, 8, &$F, 14, 15);
    c.data[(1) - (1)].data[(2) - (1)].data[(3) - (1)] = 123;
    $index(1, 1, 2, &$F, 15, 12);
    $index(2, 1, 4, &$F, 15, 15);
    $index(3, 1, 8, &$F, 15, 18);
    $index(1, 1, 2, &$F, 15, 12);
    $index(2, 1, 4, &$F, 15, 15);
    $index(3, 1, 8, &$F, 15, 18);
    $output("i", c.data[(1) - (1)].data[(2) - (1)].data[(3) - (1)]);
    $index(1, 1, 2, &$F, 17, 9);
    $index(2, 1, 4, &$F, 17, 12);
    x.data[(1) - (1)].data[(2) - (1)].i = 456;
    $index(1, 1, 2, &$F, 18, 12);
    $index(2, 1, 4, &$F, 18, 15);
    $index(1, 1, 2, &$F, 18, 12);
    $index(2, 1, 4, &$F, 18, 15);
    $output("i", x.data[(1) - (1)].data[(2) - (1)].i);
    $index(2, 1, 2, &$F, 20, 9);
    $index(3, 1, 4, &$F, 20, 12);
    y.data[(2) - (1)].data[(3) - (1)].i = 789;
    $index(2, 1, 2, &$F, 21, 12);
    $index(3, 1, 4, &$F, 21, 15);
    $index(2, 1, 2, &$F, 21, 12);
    $index(3, 1, 4, &$F, 21, 15);
    $output("i", y.data[(2) - (1)].data[(3) - (1)].i);
}
