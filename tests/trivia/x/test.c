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
    x = $concat("AA", a, $concat("AA", b, SUBSTR($0, 0, 2)));
    exit(0);
}
