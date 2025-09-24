#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR x = {0};
STR a = {0};
STR b = {0};
STR $0 = { .data = "abc" };
int main()
{
    const STR $r3 = SUBSTR($0, 0, 2);
    const STR $r2 = $concat("AA", b, $r3);
    const STR $r1 = $concat("AA", a, $r2);
    x = $r1;
    exit(0);
}
