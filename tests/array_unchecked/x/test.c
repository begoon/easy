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
int main()
{
    b.data[(1) - (1)] = 42;
    $output("i", b.data[(1) - (1)]);
    m.data[(2) - (1)].data[(3) - (1)] = 99;
    $output("i", m.data[(2) - (1)].data[(3) - (1)]);
    c.data[(1) - (1)].data[(2) - (1)].data[(3) - (1)] = 123;
    $output("i", c.data[(1) - (1)].data[(2) - (1)].data[(3) - (1)]);
    x.data[(1) - (1)].data[(2) - (1)].i = 456;
    $output("i", x.data[(1) - (1)].data[(2) - (1)].i);
    y.data[(2) - (1)].data[(3) - (1)].i = 789;
    $output("i", y.data[(2) - (1)].data[(3) - (1)].i);
}
