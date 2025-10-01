#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
STR $0 = { .data = "> ", .sz = 2, .immutable = 1 };
STR $1 = { .data = "-", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/set_group/test.easy", .sz = 25, .immutable = 1 };
int main_program()
{
    a = 123;
    b = 123;
    const STR $r1 = $concat("AiAi", $0, a, $1, b);
    $output("A", $r1);
}
