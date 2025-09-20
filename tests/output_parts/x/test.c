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
int main()
{
    b = 123;
    p.x.z = 456;
    *(typeof(c.t.data[0].data[0].data[0]) *)$ref(((typeof(c.t.data[0].data[0]) *)$ref(((typeof(c.t.data[0]) *)$ref(c.t.data, 0, 0, 2, sizeof(typeof(c.t.data[0])), "<0|INTEGER|tests/output_parts/test.easy:22:11>"))->data, 2, 0, 3, sizeof(typeof(c.t.data[0].data[0])), "<2|INTEGER|tests/output_parts/test.easy:22:14>"))->data, 1, 0, 4, sizeof(typeof(c.t.data[0].data[0].data[0])), "<1|INTEGER|tests/output_parts/test.easy:22:17>") = 789;
    $output("iii", b, p.x.z, *(typeof(c.t.data[0].data[0].data[0]) *)$ref(((typeof(c.t.data[0].data[0]) *)$ref(((typeof(c.t.data[0]) *)$ref(c.t.data, 0, 0, 2, sizeof(typeof(c.t.data[0])), "<0|INTEGER|tests/output_parts/test.easy:23:24>"))->data, 2, 0, 3, sizeof(typeof(c.t.data[0].data[0])), "<2|INTEGER|tests/output_parts/test.easy:23:27>"))->data, 1, 0, 4, sizeof(typeof(c.t.data[0].data[0].data[0])), "<1|INTEGER|tests/output_parts/test.easy:23:30>"));
}
