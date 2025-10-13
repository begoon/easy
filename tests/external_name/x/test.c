#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int f = 0;
STR $F = { .data = "tests/external_name/test.easy", .sz = 29, .immutable = 1 };
int a(int i)
{
    const int $r2 = (i + 69);
    return $r2;
}
void b(int i, int j)
{
    const int $r3 = (i + j);
    f = $r3;
}
int main_program()
{
    const int $r1 = a(1);
    f = $r1;
    $output("i", f);
}
