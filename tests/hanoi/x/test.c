#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "move ", .sz = 5, .immutable = 1 };
STR $1 = { .data = " from ", .sz = 6, .immutable = 1 };
STR $2 = { .data = " to ", .sz = 4, .immutable = 1 };
STR $F = { .data = "tests/hanoi/test.easy", .sz = 21, .immutable = 1 };
void move(int n, int a, int b)
{
    $output("AiAiAi", $0, n, $1, a, $2, b);
}
void hanoi(int n, int a, int b, int c)
{
    const int $r1 = (n == 1);
    if ($r1)
    {
        move(1, a, c);
    }
    else
    {
        const int $r2 = (n - 1);
        hanoi($r2, a, c, b);
        move(n, a, c);
        const int $r3 = (n - 1);
        hanoi($r3, b, a, c);
    }
}
int main_program()
{
    hanoi(4, 1, 2, 3);
}
