#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
int c = 0;
STR s = {0};
STR $0 = { .data = "..." };
STR $1 = { .data = "abc" };
int main()
{
    a = 1;
    b = 2;
    auto $r2 = (a + 0);
    auto $r1 = ($r2 + b);
    c = $r1;
    $output("iA", c, $0);
    auto $r3 = $concat("AA", s, $1);
    s = $r3;
    $output("A", s);
}
