#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int a;
    STR b;
} T;
T t = {0};
STR $0 = { .data = ".", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/structure/test.easy", .sz = 25, .immutable = 1 };
int main_program()
{
    t.a = 1;
    $output("A", $0);
}
