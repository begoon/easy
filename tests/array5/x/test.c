#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        int score;
        STR name;
    } data[100 - 1 + 1];
} Field;
Field f = {0};
STR $0 = { .data = "BEFORE: " };
STR $1 = { .data = " " };
STR $2 = { .data = "xyz" };
STR $3 = { .data = "abc" };
STR $4 = { .data = "AFTER: " };
Field a(Field f)
{
    $output("A", $concat("AiAA", $0, ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:10:28>"))->score, $1, ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:10:50>"))->name));
    ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:11:11>"))->score = 123;
    ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:12:11>"))->name = $concat("AA", ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:12:25>"))->name, $2);
    return f;
}
int main()
{
    ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:16:9>"))->score = 321;
    ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:17:9>"))->name = $3;
    f = a(f);
    $output("A", $concat("AiAA", $4, ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:19:25>"))->score, $1, ((typeof(f.data[0]) *)$ref(f.data, 10, 1, 100, sizeof(typeof(f.data[0])), "<10|INTEGER|tests/array5/test.easy:19:47>"))->name));
    exit(0);
}
