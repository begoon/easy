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
                int data[0 + 4 + 1];
            } data[0 + 3 + 1];
        } data[0 + 2 + 1];
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
                int data[0 + 4 + 1];
            } data[0 + 3 + 1];
        } data[0 + 2 + 1];
    } t;
} c = {0};
int main()
{
    b = 123;
    p.x.z = 456;
    c.t.data[0].data[2].data[1] = 789;
    $output("iii", b, p.x.z, c.t.data[0].data[2].data[1]);
}
