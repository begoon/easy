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
STR $F = { .data = "tests/array1/test.easy", .sz = 22, .immutable = 1 };
STR $1 = { .data = "...", .sz = 3, .immutable = 1 };
int main_program()
{
    $index(1, 0, 25, &$F, 15, 9);
    $index(2, 0, 80, &$F, 15, 14);
    $index(2, 0, 25, &$F, 15, 22);
    $index(1, 0, 80, &$F, 15, 27);
    b.data[(1) - (0)].a.data[(2) - (0)] = b.data[(2) - (0)].a.data[(1) - (0)];
    $output("A", $1);
    $exit();
}
