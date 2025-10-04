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
STR $F = { .data = "tests/array_1000/test.easy", .sz = 26, .immutable = 1 };
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
        void *$r1 AUTOFREE = malloc(sizeof(double) * (n - n + 1));
        struct
        {
            double *data;
        } c = { .data = $r1 };
        void *$r2 AUTOFREE = malloc(sizeof(struct
        {
            double data[1 - 0 + 1];
        }) * (n - n + 1));
        struct
        {
            struct
            {
                double data[1 - 0 + 1];
            } *data;
        } d = { .data = $r2 };
        $index(n, n, n, &$F, 20, 11);
        c.data[(n) - (n)] = 3.14;
        $index(n, n, n, &$F, 21, 14);
        $index(n, n, n, &$F, 21, 14);
        $output("r", c.data[(n) - (n)]);
        $index(n, n, n, &$F, 22, 11);
        c.data[(n) - (n)] = 42.0;
        $index(n, n, n, &$F, 23, 14);
        $index(n, n, n, &$F, 23, 14);
        $output("r", c.data[(n) - (n)]);
        $index(n, n, n, &$F, 25, 11);
        $index(1, 0, 1, &$F, 25, 14);
        d.data[(n) - (n)].data[(1) - (0)] = 0.0;
        $index(n, n, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        $index(n, n, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        $output("r", d.data[(n) - (n)].data[(1) - (0)]);
        $index(n, n, n, &$F, 27, 11);
        $index(1, 0, 1, &$F, 27, 14);
        d.data[(n) - (n)].data[(1) - (0)] = 84.0;
        $index(n, n, n, &$F, 28, 14);
        $index(1, 0, 1, &$F, 28, 17);
        $index(n, n, n, &$F, 28, 14);
        $index(1, 0, 1, &$F, 28, 17);
        $output("r", d.data[(n) - (n)].data[(1) - (0)]);
    }
}
