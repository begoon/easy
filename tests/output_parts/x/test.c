#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int b = 0;
struct
{
    struct
    {
        int y;
        int z;
    } x;
    struct
    {
        struct
        {
            struct
            {
                int data[4 - 0 + 1];
            } data[3 - 0 + 1];
        } data[2 - 0 + 1];
    } t;
} p = {0};
struct
{
    double r;
    STR s;
    struct
    {
        struct
        {
            struct
            {
                int data[4 - 0 + 1];
            } data[3 - 0 + 1];
        } data[2 - 0 + 1];
    } t;
} c = {0};
STR $F = { .data = "tests/output_parts/test.easy", .sz = 28, .immutable = 1 };
int main_program()
{
    b = 123;
    p.x.z = 456;
    $index(0, 0, 2, &$F, 22, 11);
    $index(2, 0, 3, &$F, 22, 14);
    $index(1, 0, 4, &$F, 22, 17);
    c.t.data[(0) - (0)].data[(2) - (0)].data[(1) - (0)] = 789;
    $index(0, 0, 2, &$F, 23, 24);
    $index(2, 0, 3, &$F, 23, 27);
    $index(1, 0, 4, &$F, 23, 30);
    $index(0, 0, 2, &$F, 23, 24);
    $index(2, 0, 3, &$F, 23, 27);
    $index(1, 0, 4, &$F, 23, 30);
    $output("iii", b, p.x.z, c.t.data[(0) - (0)].data[(2) - (0)].data[(1) - (0)]);
}
