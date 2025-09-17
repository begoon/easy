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
    $output("A", SUBSTR($1, 1, 2));
    $output("A", SUBSTR(s, 1, 2));
    $output("A", $concat("AA", $2, SUBSTR(s, 3, 2)));
    exit(0);
}
