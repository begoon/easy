#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
struct
{
    int data[99 - 0 + 1];
} a = {0};
int sz = 0;
int main()
{
    sz = 1024;
    {
        struct
        {
            int *data;
        } b = { .data = malloc(sizeof(int) * (sz - 1 + 1)) };
        *(typeof(b.data[0]) *)$ref(b.data, 1, 1, sz, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array_dynamic/test.easy:9:11>") = 42;
        $output("i", *(typeof(b.data[0]) *)$ref(b.data, 1, 1, sz, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array_dynamic/test.easy:10:14>"));
    }
}
