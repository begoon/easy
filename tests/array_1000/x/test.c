#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
struct
{
    int data[1001 - 1000 + 1];
} a = {0};
struct
{
    int data[1000 - 1000 + 1];
} b = {0};
int n = 0;
STR $F = { .data = "tests/array_1000/test.easy", .sz = 26, .immutable = 1 };;
int main_program()
{
    $index(1000, 1000, 1001, &$F, 6, 9);
    a.data[(1000) - (1000)] = 123;
    $index(1001, 1000, 1001, &$F, 7, 9);
    a.data[(1001) - (1000)] = 456;
    $index(1000, 1000, 1001, &$F, 9, 12);
    $index(1000, 1000, 1001, &$F, 9, 12);
    $output("i", a.data[(1000) - (1000)]);
    $index(1001, 1000, 1001, &$F, 10, 12);
    $index(1001, 1000, 1001, &$F, 10, 12);
    $output("i", a.data[(1001) - (1000)]);
    $index(1000, 1000, 1000, &$F, 12, 9);
    b.data[(1000) - (1000)] = 789;
    $index(1000, 1000, 1000, &$F, 13, 12);
    $index(1000, 1000, 1000, &$F, 13, 12);
    $output("i", b.data[(1000) - (1000)]);
    n = 1000;
    {
        struct
        {
            double *data;
        } c = { .data = malloc(sizeof(double) * (n - n + 1)) };
        struct
        {
            struct
            {
                double data[1 - 0 + 1];
            } *data;
        } d = { .data = malloc(sizeof(struct
        {
            double data[1 - 0 + 1];
        }) * (n - n + 1)) };
        $index(n, n, n, &$F, 20, 14);
        $index(n, n, n, &$F, 20, 14);
        $output("r", c.data[(n) - (n)]);
        $index(n, n, n, &$F, 21, 11);
        c.data[(n) - (n)] = 42;
        $index(n, n, n, &$F, 22, 14);
        $index(n, n, n, &$F, 22, 14);
        $output("r", c.data[(n) - (n)]);
        $index(n, n, n, &$F, 24, 14);
        $index(1, 0, 1, &$F, 24, 17);
        $index(n, n, n, &$F, 24, 14);
        $index(1, 0, 1, &$F, 24, 17);
        $output("r", d.data[(n) - (n)].data[(1) - (0)]);
        $index(n, n, n, &$F, 25, 11);
        $index(1, 0, 1, &$F, 25, 14);
        d.data[(n) - (n)].data[(1) - (0)] = 84;
        $index(n, n, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        $index(n, n, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        $output("r", d.data[(n) - (n)].data[(1) - (0)]);
    }
}
