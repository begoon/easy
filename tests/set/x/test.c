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
                int data[100 - 0 + 1];
            } data[1 - 0 + 1];
        } data[1 - 0 + 1];
    } x;
    int y;
} p = {0};
STR $0 = { .data = " " };
int main()
{
    p.y = 123;
    *(typeof(p.x.data[0].data[0].data[0]) *)$ref(((typeof(p.x.data[0].data[0]) *)$ref(((typeof(p.x.data[0]) *)$ref(p.x.data, 0, 0, 1, sizeof(typeof(p.x.data[0])), "<0|INTEGER|tests/set/test.easy:7:11>"))->data, 1, 0, 1, sizeof(typeof(p.x.data[0].data[0])), "<1|INTEGER|tests/set/test.easy:7:14>"))->data, 100, 0, 100, sizeof(typeof(p.x.data[0].data[0].data[0])), "<100|INTEGER|tests/set/test.easy:7:17>") = (p.y + 321);
    $output("iAi", *(typeof(p.x.data[0].data[0].data[0]) *)$ref(((typeof(p.x.data[0].data[0]) *)$ref(((typeof(p.x.data[0]) *)$ref(p.x.data, 0, 0, 1, sizeof(typeof(p.x.data[0])), "<0|INTEGER|tests/set/test.easy:8:14>"))->data, 1, 0, 1, sizeof(typeof(p.x.data[0].data[0])), "<1|INTEGER|tests/set/test.easy:8:17>"))->data, 100, 0, 100, sizeof(typeof(p.x.data[0].data[0].data[0])), "<100|INTEGER|tests/set/test.easy:8:20>"), $0, p.y);
}
