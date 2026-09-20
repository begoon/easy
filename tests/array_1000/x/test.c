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
STR $F = { .data = "tests/array_1000/test.easy", .sz = 26 };
int main_program()
{
    $index(1000, 1000, 1001, &$F, 6, 9);
    a.data[(size_t)((int64_t)(1000) - (1000))] = 123;
    $index(1001, 1000, 1001, &$F, 7, 9);
    a.data[(size_t)((int64_t)(1001) - (1000))] = 456;
    $index(1000, 1000, 1001, &$F, 9, 12);
    $index(1000, 1000, 1001, &$F, 9, 12);
    $output("i", a.data[(size_t)((int64_t)(1000) - (1000))]);
    $index(1001, 1000, 1001, &$F, 10, 12);
    $index(1001, 1000, 1001, &$F, 10, 12);
    $output("i", a.data[(size_t)((int64_t)(1001) - (1000))]);
    $index(1000, 1000, 1000, &$F, 12, 9);
    b.data[(size_t)((int64_t)(1000) - (1000))] = 789;
    $index(1000, 1000, 1000, &$F, 13, 12);
    $index(1000, 1000, 1000, &$F, 13, 12);
    $output("i", b.data[(size_t)((int64_t)(1000) - (1000))]);
    n = 1000;
    {
        struct
        {
            double *data;
        } c = {0};
        OWN(c, manage_array, $r9);
        const int $r10 = n;
        c.data = make_array($r10, n, sizeof(double), NULL);
        struct
        {
            struct
            {
                double data[1 - 0 + 1];
            } *data;
        } d = {0};
        OWN(d, manage_array, $r11);
        const int $r12 = n;
        d.data = make_array($r12, n, sizeof(struct
        {
            double data[1 - 0 + 1];
        }), NULL);
        const size_t $r13 = $array_index(c.data, n, &$F, 20, 11);
        c.data[$r13] = 3.14;
        const size_t $r14 = $array_index(c.data, n, &$F, 21, 14);
        const size_t $r15 = $array_index(c.data, n, &$F, 21, 14);
        $output("r", c.data[$r15]);
        const size_t $r16 = $array_index(c.data, n, &$F, 22, 11);
        c.data[$r16] = 42.0;
        const size_t $r17 = $array_index(c.data, n, &$F, 23, 14);
        const size_t $r18 = $array_index(c.data, n, &$F, 23, 14);
        $output("r", c.data[$r18]);
        const size_t $r19 = $array_index(d.data, n, &$F, 25, 11);
        $index(1, 0, 1, &$F, 25, 14);
        d.data[$r19].data[(size_t)((int64_t)(1) - (0))] = 0.0;
        const size_t $r20 = $array_index(d.data, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        const size_t $r21 = $array_index(d.data, n, &$F, 26, 14);
        $index(1, 0, 1, &$F, 26, 17);
        $output("r", d.data[$r21].data[(size_t)((int64_t)(1) - (0))]);
        const size_t $r22 = $array_index(d.data, n, &$F, 27, 11);
        $index(1, 0, 1, &$F, 27, 14);
        d.data[$r22].data[(size_t)((int64_t)(1) - (0))] = 84.0;
        const size_t $r23 = $array_index(d.data, n, &$F, 28, 14);
        $index(1, 0, 1, &$F, 28, 17);
        const size_t $r24 = $array_index(d.data, n, &$F, 28, 14);
        $index(1, 0, 1, &$F, 28, 17);
        $output("r", d.data[$r24].data[(size_t)((int64_t)(1) - (0))]);
    }
}
