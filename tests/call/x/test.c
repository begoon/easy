#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int x = 0;
STR $0 = { .data = "a(): ", .sz = 5, .immutable = 1 };
STR $F = { .data = "tests/call/test.easy", .sz = 20, .immutable = 1 };
int b(int x)
{
    const int $r2 = (x + 1);
    return $r2;
}
void a(int x)
{
    const STR $r3 = $concat("Ai", $0, x);
    $output("A", $r3);
}
int main_program()
{
    a(100);
    const int $r1 = b(100);
    x = $r1;
    $output("i", x);
    exit(0);
}
