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
int main()
{
    *(typeof(a.data[0]) *)$ref(a.data, 1000, 1000, 1001, sizeof(typeof(a.data[0])), "<1000|INTEGER|tests/array_1000/test.easy:6:9") = 123;
    *(typeof(a.data[0]) *)$ref(a.data, 1001, 1000, 1001, sizeof(typeof(a.data[0])), "<1001|INTEGER|tests/array_1000/test.easy:7:9") = 456;
    $output("i", *(typeof(a.data[0]) *)$ref(a.data, 1000, 1000, 1001, sizeof(typeof(a.data[0])), "<1000|INTEGER|tests/array_1000/test.easy:9:12"));
    $output("i", *(typeof(a.data[0]) *)$ref(a.data, 1001, 1000, 1001, sizeof(typeof(a.data[0])), "<1001|INTEGER|tests/array_1000/test.easy:10:12"));
    *(typeof(b.data[0]) *)$ref(b.data, 1000, 1000, 1000, sizeof(typeof(b.data[0])), "<1000|INTEGER|tests/array_1000/test.easy:12:9") = 789;
    $output("i", *(typeof(b.data[0]) *)$ref(b.data, 1000, 1000, 1000, sizeof(typeof(b.data[0])), "<1000|INTEGER|tests/array_1000/test.easy:13:12"));
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
        $output("r", *(typeof(c.data[0]) *)$ref(c.data, n, n, n, sizeof(typeof(c.data[0])), "<n|IDENT|tests/array_1000/test.easy:20:14"));
        *(typeof(c.data[0]) *)$ref(c.data, n, n, n, sizeof(typeof(c.data[0])), "<n|IDENT|tests/array_1000/test.easy:21:11") = 42;
        $output("r", *(typeof(c.data[0]) *)$ref(c.data, n, n, n, sizeof(typeof(c.data[0])), "<n|IDENT|tests/array_1000/test.easy:22:14"));
        $output("r", *(typeof(d.data[0].data[0]) *)$ref(((typeof(d.data[0]) *)$ref(d.data, n, n, n, sizeof(typeof(d.data[0])), "<n|IDENT|tests/array_1000/test.easy:24:14"))->data, 1, 0, 1, sizeof(typeof(d.data[0].data[0])), "<1|INTEGER|tests/array_1000/test.easy:24:17"));
        *(typeof(d.data[0].data[0]) *)$ref(((typeof(d.data[0]) *)$ref(d.data, n, n, n, sizeof(typeof(d.data[0])), "<n|IDENT|tests/array_1000/test.easy:25:11"))->data, 1, 0, 1, sizeof(typeof(d.data[0].data[0])), "<1|INTEGER|tests/array_1000/test.easy:25:14") = 84;
        $output("r", *(typeof(d.data[0].data[0]) *)$ref(((typeof(d.data[0]) *)$ref(d.data, n, n, n, sizeof(typeof(d.data[0])), "<n|IDENT|tests/array_1000/test.easy:26:14"))->data, 1, 0, 1, sizeof(typeof(d.data[0].data[0])), "<1|INTEGER|tests/array_1000/test.easy:26:17"));
    }
}
