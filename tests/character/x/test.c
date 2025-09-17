#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "123" };
STR $1 = { .data = "!" };
STR $2 = { .data = "a" };
STR $3 = { .data = "b" };
STR $4 = { .data = "c" };
int main()
{
    $output("A", $concat("AA", CHARACTER(48), CHARACTER(50)));
    $output("A", CHARACTER(49));
    $output("A", $concat("AA", CHARACTER(13), CHARACTER(10)));
    $output("A", $concat("AA", $0, $1));
    $output("A", $concat("AAA", $2, $3, $4));
    exit(0);
}
