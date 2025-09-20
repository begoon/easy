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
    *(typeof(b.data[0]) *)$ref(b.data, 1, 1, 2, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array_checked/test.easy:8:9>") = 42;
    $output("i", *(typeof(b.data[0]) *)$ref(b.data, 1, 1, 2, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array_checked/test.easy:9:12>"));
    *(typeof(m.data[0].data[0]) *)$ref(((typeof(m.data[0]) *)$ref(m.data, 2, 1, 2, sizeof(typeof(m.data[0])), "<2|INTEGER|tests/array_checked/test.easy:11:9>"))->data, 3, 1, 4, sizeof(typeof(m.data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:11:12>") = 99;
    $output("i", *(typeof(m.data[0].data[0]) *)$ref(((typeof(m.data[0]) *)$ref(m.data, 2, 1, 2, sizeof(typeof(m.data[0])), "<2|INTEGER|tests/array_checked/test.easy:12:12>"))->data, 3, 1, 4, sizeof(typeof(m.data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:12:15>"));
    *(typeof(c.data[0].data[0].data[0]) *)$ref(((typeof(c.data[0].data[0]) *)$ref(((typeof(c.data[0]) *)$ref(c.data, 1, 1, 2, sizeof(typeof(c.data[0])), "<1|INTEGER|tests/array_checked/test.easy:14:9>"))->data, 2, 1, 4, sizeof(typeof(c.data[0].data[0])), "<2|INTEGER|tests/array_checked/test.easy:14:12>"))->data, 3, 1, 8, sizeof(typeof(c.data[0].data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:14:15>") = 123;
    $output("i", *(typeof(c.data[0].data[0].data[0]) *)$ref(((typeof(c.data[0].data[0]) *)$ref(((typeof(c.data[0]) *)$ref(c.data, 1, 1, 2, sizeof(typeof(c.data[0])), "<1|INTEGER|tests/array_checked/test.easy:15:12>"))->data, 2, 1, 4, sizeof(typeof(c.data[0].data[0])), "<2|INTEGER|tests/array_checked/test.easy:15:15>"))->data, 3, 1, 8, sizeof(typeof(c.data[0].data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:15:18>"));
    ((typeof(x.data[0].data[0]) *)$ref(((typeof(x.data[0]) *)$ref(x.data, 1, 1, 2, sizeof(typeof(x.data[0])), "<1|INTEGER|tests/array_checked/test.easy:17:9>"))->data, 2, 1, 4, sizeof(typeof(x.data[0].data[0])), "<2|INTEGER|tests/array_checked/test.easy:17:12>"))->i = 456;
    $output("i", ((typeof(x.data[0].data[0]) *)$ref(((typeof(x.data[0]) *)$ref(x.data, 1, 1, 2, sizeof(typeof(x.data[0])), "<1|INTEGER|tests/array_checked/test.easy:18:12>"))->data, 2, 1, 4, sizeof(typeof(x.data[0].data[0])), "<2|INTEGER|tests/array_checked/test.easy:18:15>"))->i);
    ((typeof(y.data[0].data[0]) *)$ref(((typeof(y.data[0]) *)$ref(y.data, 2, 1, 2, sizeof(typeof(y.data[0])), "<2|INTEGER|tests/array_checked/test.easy:20:9>"))->data, 3, 1, 4, sizeof(typeof(y.data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:20:12>"))->i = 789;
    $output("i", ((typeof(y.data[0].data[0]) *)$ref(((typeof(y.data[0]) *)$ref(y.data, 2, 1, 2, sizeof(typeof(y.data[0])), "<2|INTEGER|tests/array_checked/test.easy:21:12>"))->data, 3, 1, 4, sizeof(typeof(y.data[0].data[0])), "<3|INTEGER|tests/array_checked/test.easy:21:15>"))->i);
}
