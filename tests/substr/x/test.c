#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "abcXYZ" };
STR $1 = { .data = "12345" };
STR $2 = { .data = "abc" };
int main()
{
    s = $0;
    const STR $r1 = SUBSTR($1, 1, 2);
    $output("A", $r1);
    const STR $r2 = SUBSTR(s, 1, 2);
    $output("A", $r2);
    const STR $r4 = SUBSTR(s, 3, 2);
    const STR $r3 = $concat("AA", $2, $r4);
    $output("A", $r3);
    exit(0);
}
