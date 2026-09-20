#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
static void $manage1(const void *value, int change)
{
    typedef struct
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
    } T;
    manage_string((const char *)value + offsetof(T, x), change);
}
static void $manage0(const void *value, int change)
{
    typedef struct
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
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        $manage1((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
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
STR $F = { .data = "tests/array1/test.easy", .sz = 22 };
STR $1 = { .data = "...", .sz = 3 };
int main_program()
{
    OWN(b, $manage0, $r1);
    $index(1, 0, 25, &$F, 15, 9);
    $index(2, 0, 80, &$F, 15, 14);
    $index(2, 0, 25, &$F, 15, 22);
    $index(1, 0, 80, &$F, 15, 27);
    b.data[(size_t)((int64_t)(1) - (0))].a.data[(size_t)((int64_t)(2) - (0))] = b.data[(size_t)((int64_t)(2) - (0))].a.data[(size_t)((int64_t)(1) - (0))];
    $output("A", $1);
    $exit();
}
