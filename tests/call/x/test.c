#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int x = 0;
STR $0 = { .data = "a(): " };
int b(int x)
{
    const auto $r2 = (x + 1);
    return $r2;
}
void a(int x)
{
    const auto $r3 = $concat("Ai", $0, x);
    $output("A", $r3);
}
int main()
{
    a(100);
    const auto $r1 = b(100);
    x = $r1;
    $output("i", x);
    exit(0);
}
