#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "123", .sz = 3, .immutable = 1 };
STR $1 = { .data = "!", .sz = 1, .immutable = 1 };
STR $2 = { .data = "a", .sz = 1, .immutable = 1 };
STR $3 = { .data = "b", .sz = 1, .immutable = 1 };
STR $4 = { .data = "c", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/character/test.easy", .sz = 25, .immutable = 1 };
int main_program()
{
    const STR $r2 = CHARACTER(48);
    const STR $r3 = CHARACTER(50);
    const STR $r1 = $concat("AA", $r2, $r3);
    $output("A", $r1);
    const STR $r4 = CHARACTER(49);
    $output("A", $r4);
    const STR $r6 = CHARACTER(13);
    const STR $r7 = CHARACTER(10);
    const STR $r5 = $concat("AA", $r6, $r7);
    $output("A", $r5);
    const STR $r8 = $concat("AA", $0, $1);
    $output("A", $r8);
    const STR $r9 = $concat("AAA", $2, $3, $4);
    $output("A", $r9);
    $exit();
}
