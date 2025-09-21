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
    auto $r2 = CHARACTER(48);
    auto $r3 = CHARACTER(50);
    auto $r1 = $concat("AA", $r2, $r3);
    $output("A", $r1);
    auto $r4 = CHARACTER(49);
    $output("A", $r4);
    auto $r6 = CHARACTER(13);
    auto $r7 = CHARACTER(10);
    auto $r5 = $concat("AA", $r6, $r7);
    $output("A", $r5);
    auto $r8 = $concat("AA", $0, $1);
    $output("A", $r8);
    auto $r9 = $concat("AAA", $2, $3, $4);
    $output("A", $r9);
    exit(0);
}
