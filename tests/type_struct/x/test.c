#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int x;
    int y;
} Point;
Point t = {0};
STR $F = { .data = "tests/type_struct/test.easy", .sz = 27, .immutable = 1 };
int main_program()
{
    $exit();
}
