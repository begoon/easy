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
        struct
        {
            int *data;
        } b = {0};
        OWN(b, manage_array, $r3);
        const int $r4 = 1;
        b.data = make_array($r4, sz, sizeof(int), NULL);
        const size_t $r5 = $array_index(b.data, 1, &$F, 9, 11);
        b.data[$r5] = 42;
        const size_t $r6 = $array_index(b.data, 1, &$F, 10, 14);
        const size_t $r7 = $array_index(b.data, 1, &$F, 10, 14);
        $output("i", b.data[$r7]);
    }
}
