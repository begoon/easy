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
STR $0 = { .data = "BEFORE: ", .sz = 8, .immutable = 1 };
STR $F = { .data = "tests/array5/test.easy", .sz = 22, .immutable = 1 };
STR $2 = { .data = " ", .sz = 1, .immutable = 1 };
STR $3 = { .data = "xyz", .sz = 3, .immutable = 1 };
STR $4 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $5 = { .data = "AFTER: ", .sz = 7, .immutable = 1 };
Field a(Field f)
{
    $index(10, 1, 100, &$F, 10, 28);
    $index(10, 1, 100, &$F, 10, 28);
    $index(10, 1, 100, &$F, 10, 50);
    $index(10, 1, 100, &$F, 10, 50);
    const STR $r3 = $concat("AiAA", $0, f.data[(10) - (1)].score, $2, f.data[(10) - (1)].name);
    $output("A", $r3);
    $index(10, 1, 100, &$F, 11, 11);
    f.data[(10) - (1)].score = 123;
    $index(10, 1, 100, &$F, 12, 11);
    $index(10, 1, 100, &$F, 12, 25);
    $index(10, 1, 100, &$F, 12, 25);
    const STR $r4 = $concat("AA", f.data[(10) - (1)].name, $3);
    f.data[(10) - (1)].name = $r4;
    return f;
}
int main_program()
{
    $index(10, 1, 100, &$F, 16, 9);
    f.data[(10) - (1)].score = 321;
    $index(10, 1, 100, &$F, 17, 9);
    f.data[(10) - (1)].name = $4;
    const Field $r1 = a(f);
    f = $r1;
    $index(10, 1, 100, &$F, 19, 25);
    $index(10, 1, 100, &$F, 19, 25);
    $index(10, 1, 100, &$F, 19, 47);
    $index(10, 1, 100, &$F, 19, 47);
    const STR $r2 = $concat("AiAA", $5, f.data[(10) - (1)].score, $2, f.data[(10) - (1)].name);
    $output("A", $r2);
    exit(0);
}
