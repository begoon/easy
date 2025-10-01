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
                int data[100 - 0 + 1];
            } data[1 - 0 + 1];
        } data[1 - 0 + 1];
    } x;
    int y;
} p = {0};
STR $0 = { .data = " ", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/set/test.easy", .sz = 19, .immutable = 1 };
int main_program()
{
    p.y = 123;
    $index(0, 0, 1, &$F, 7, 11);
    $index(1, 0, 1, &$F, 7, 14);
    $index(100, 0, 100, &$F, 7, 17);
    const int $r1 = (p.y + 321);
    p.x.data[(0) - (0)].data[(1) - (0)].data[(100) - (0)] = $r1;
    $index(0, 0, 1, &$F, 8, 14);
    $index(1, 0, 1, &$F, 8, 17);
    $index(100, 0, 100, &$F, 8, 20);
    $index(0, 0, 1, &$F, 8, 14);
    $index(1, 0, 1, &$F, 8, 17);
    $index(100, 0, 100, &$F, 8, 20);
    $output("iAi", p.x.data[(0) - (0)].data[(1) - (0)].data[(100) - (0)], $0, p.y);
}
