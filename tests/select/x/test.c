#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
STR $0 = { .data = "a < 0", .sz = 5, .immutable = 1 };
STR $1 = { .data = "a = 0", .sz = 5, .immutable = 1 };
STR $2 = { .data = "otherwise", .sz = 9, .immutable = 1 };
STR $F = { .data = "tests/select/test.easy", .sz = 22, .immutable = 1 };
int main_program()
{
    a = 100;
    const int $r1 = (a < 0);
    const int $r2 = (a == 0);
    if ($r1)
    {
        $output("A", $0);
    }
    else if ($r2)
    {
        $output("A", $1);
    }
    else
    {
        $output("A", $2);
    }
    exit(0);
}
