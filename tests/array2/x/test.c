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
            int x;
            double y;
        } data[80 - 0 + 1];
    } data[25 - 1 + 1];
} B;
B b = {0};
STR $0 = { .data = "." };
STR $F = { .data = "tests/array2/test.easy" };
int main()
{
    $index(1, 1, 25, &$F, 10, 9);
    $index(2, 0, 80, &$F, 10, 12);
    b.data[(1) - (1)].data[(2) - (0)].x = b.data[(2) - (1)].data[(1) - (0)].x;
    $output("A", $0);
}
