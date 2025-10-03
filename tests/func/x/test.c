#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int f = 0;
STR $F = { .data = "tests/func/test.easy", .sz = 20, .immutable = 1 };
int a(int i)
{
    const int $r2 = (i + 69);
    return $r2;
}
int main_program()
{
    const int $r1 = a(1);
    f = $r1;
    $output("i", f);
}
