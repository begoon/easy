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
            struct
            {
                int i;
                STR s;
            } data[4 - 0 + 1];
        } data[2 - 0 + 1];
    } data[8 - 0 + 1];
} A;
A a = {0};
A b = {0};
STR $0 = { .data = "abc" };
STR $1 = { .data = " " };
int main()
{
    ((typeof(a.data[0].data[0].data[0]) *)$ref(((typeof(a.data[0].data[0]) *)$ref(((typeof(a.data[0]) *)$ref(a.data, 1, 0, 8, sizeof(typeof(a.data[0])), "<1|INTEGER|tests/array3/test.easy:11:9>"))->data, 2, 0, 2, sizeof(typeof(a.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:11:12>"))->data, 3, 0, 4, sizeof(typeof(a.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:11:15>"))->i = 123;
    ((typeof(a.data[0].data[0].data[0]) *)$ref(((typeof(a.data[0].data[0]) *)$ref(((typeof(a.data[0]) *)$ref(a.data, 1, 0, 8, sizeof(typeof(a.data[0])), "<1|INTEGER|tests/array3/test.easy:12:9>"))->data, 2, 0, 2, sizeof(typeof(a.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:12:12>"))->data, 3, 0, 4, sizeof(typeof(a.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:12:15>"))->s = $0;
    b = a;
    $output("iAi", ((typeof(a.data[0].data[0].data[0]) *)$ref(((typeof(a.data[0].data[0]) *)$ref(((typeof(a.data[0]) *)$ref(a.data, 1, 0, 8, sizeof(typeof(a.data[0])), "<1|INTEGER|tests/array3/test.easy:14:12>"))->data, 2, 0, 2, sizeof(typeof(a.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:14:15>"))->data, 3, 0, 4, sizeof(typeof(a.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:14:18>"))->i, $1, ((typeof(b.data[0].data[0].data[0]) *)$ref(((typeof(b.data[0].data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 1, 0, 8, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array3/test.easy:14:31>"))->data, 2, 0, 2, sizeof(typeof(b.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:14:34>"))->data, 3, 0, 4, sizeof(typeof(b.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:14:37>"))->i);
    $output("A", $concat("AAA", ((typeof(a.data[0].data[0].data[0]) *)$ref(((typeof(a.data[0].data[0]) *)$ref(((typeof(a.data[0]) *)$ref(a.data, 1, 0, 8, sizeof(typeof(a.data[0])), "<1|INTEGER|tests/array3/test.easy:15:12>"))->data, 2, 0, 2, sizeof(typeof(a.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:15:15>"))->data, 3, 0, 4, sizeof(typeof(a.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:15:18>"))->s, $1, ((typeof(b.data[0].data[0].data[0]) *)$ref(((typeof(b.data[0].data[0]) *)$ref(((typeof(b.data[0]) *)$ref(b.data, 1, 0, 8, sizeof(typeof(b.data[0])), "<1|INTEGER|tests/array3/test.easy:15:35>"))->data, 2, 0, 2, sizeof(typeof(b.data[0].data[0])), "<2|INTEGER|tests/array3/test.easy:15:38>"))->data, 3, 0, 4, sizeof(typeof(b.data[0].data[0].data[0])), "<3|INTEGER|tests/array3/test.easy:15:41>"))->s));
    exit(0);
}
