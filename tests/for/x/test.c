#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
STR $0 = { .data = " - ", .sz = 3, .immutable = 1 };
STR $F = { .data = "tests/for/test.easy", .sz = 19, .immutable = 1 };
int main_program()
{
    b = 69;
    a = 0;
    while (1)
    {
        const int $r1 = (a < 7);
        if (!($r1 && a <= 10)) break;
        int b = 0;
        const int $r2 = (a * 11);
        b = $r2;
        $output("iAi", a, $0, b);
        a += 1;
    }
    $output("i", b);
}
