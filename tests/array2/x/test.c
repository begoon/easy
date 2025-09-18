#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        struct
        {
            int x;
            double y;
        } data[80 - 0 + 1];
    } data[25 - 1 + 1];
} B;
B b = {0};
STR $0 = { .data = "." };
int main()
{
    ((typeof(b.data[0].data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 1, 1, 25, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array2/test.easy:10:9"))->data, 2, 0, 80, sizeof(typeof(b.data[0].data[0])), "<2|INTEGER|tests/array2/test.easy:10:12"))->x = ((typeof(b.data[0].data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 2, 1, 25, sizeof(typeof(b.data[0])), "<2|INTEGER|tests/array2/test.easy:10:22"))->data, 1, 0, 80, sizeof(typeof(b.data[0].data[0])), "<1|INTEGER|tests/array2/test.easy:10:25"))->x;
    $output("A", $0);
}
