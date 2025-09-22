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
STR $F = { .data = "tests/array5/test.easy" };
Field a(Field f)
{
    $index(10, 1, 100, &$F, 10, 28);
    $index(10, 1, 100, &$F, 10, 50);
    const auto $r3 = $concat("AiAA", $0, f.data[(10) - (1)].score, $1, f.data[(10) - (1)].name);
    $output("A", $r3);
    $index(10, 1, 100, &$F, 11, 11);
    f.data[(10) - (1)].score = 123;
    $index(10, 1, 100, &$F, 12, 11);
    $index(10, 1, 100, &$F, 12, 25);
    const auto $r4 = $concat("AA", f.data[(10) - (1)].name, $2);
    f.data[(10) - (1)].name = $r4;
    return f;
}
int main()
{
    $index(10, 1, 100, &$F, 16, 9);
    f.data[(10) - (1)].score = 321;
    $index(10, 1, 100, &$F, 17, 9);
    f.data[(10) - (1)].name = $3;
    const auto $r1 = a(f);
    f = $r1;
    $index(10, 1, 100, &$F, 19, 25);
    $index(10, 1, 100, &$F, 19, 47);
    const auto $r2 = $concat("AiAA", $4, f.data[(10) - (1)].score, $1, f.data[(10) - (1)].name);
    $output("A", $r2);
    exit(0);
}
