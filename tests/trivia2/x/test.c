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
int a = 0;
Point p = {0};
STR $0 = { .data = " ", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/trivia2/test.easy", .sz = 23, .immutable = 1 };
int main_program()
{
    {
        p.x = 1;
        p.y = 2;
        a = p.x;
    }
    $output("iAiAi", a, $0, p.y, $0, p.x);
}
