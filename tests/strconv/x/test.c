#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "abc", .sz = 3, .immutable = 1 };;
STR $F = { .data = "tests/strconv/test.easy", .sz = 23, .immutable = 1 };;
int main_program()
{
    $output("A", $0);
}
