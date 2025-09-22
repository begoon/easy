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
    const auto $r2 = CHARACTER(48);
    const auto $r3 = CHARACTER(50);
    const auto $r1 = $concat("AA", $r2, $r3);
    $output("A", $r1);
    const auto $r4 = CHARACTER(49);
    $output("A", $r4);
    const auto $r6 = CHARACTER(13);
    const auto $r7 = CHARACTER(10);
    const auto $r5 = $concat("AA", $r6, $r7);
    $output("A", $r5);
    const auto $r8 = $concat("AA", $0, $1);
    $output("A", $r8);
    const auto $r9 = $concat("AAA", $2, $3, $4);
    $output("A", $r9);
    exit(0);
}
