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
STR $F = { .data = "tests/array_dynamic/test.easy", .sz = 29 };
int main_program()
{
    sz = 1024;
    {
        void *$r1 = make_array((sz - 1 + 1), sizeof(int), NULL);
        struct
        {
            int *data;
        } b = { .data = $r1 };
        OWN(b, manage_array, $r2);
        $index(1, 1, sz, &$F, 9, 11);
        b.data[(1) - (1)] = 42;
        $index(1, 1, sz, &$F, 10, 14);
        $index(1, 1, sz, &$F, 10, 14);
        $output("i", b.data[(1) - (1)]);
    }
}
