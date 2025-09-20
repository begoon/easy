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
int main()
{
    *(typeof(b.data[0].a.data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 1, 0, 25, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array1/test.easy:15:9>"))->a.data, 2, 0, 80, sizeof(typeof(b.data[0].a.data[0])), "<2|INTEGER|tests/array1/test.easy:15:14>") = *(typeof(b.data[0].a.data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 2, 0, 25, sizeof(typeof(b.data[0])), "<2|INTEGER|tests/array1/test.easy:15:22>"))->a.data, 1, 0, 80, sizeof(typeof(b.data[0].a.data[0])), "<1|INTEGER|tests/array1/test.easy:15:27>");
    $output("A", $0);
    exit(0);
}
