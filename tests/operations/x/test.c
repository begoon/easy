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
    c = ((a + 0) + b);
    $output("iA", c, $0);
    s = $concat("AA", s, $1);
    $output("A", s);
}
