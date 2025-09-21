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
                int x;
                double y;
                int f;
            } data[80 - 0 + 1];
        } a;
        STR x;
    } data[25 - 0 + 1];
} b = {0};
STR $0 = { .data = "..." };
STR $F = { .data = "tests/array1/test.easy" };
int main()
{
    $index(1, 0, 25, &$F, 15, 9);
    $index(2, 0, 80, &$F, 15, 14);
    b.data[(1) - (0)].a.data[(2) - (0)] = b.data[(2) - (0)].a.data[(1) - (0)];
    $output("A", $0);
    exit(0);
}
