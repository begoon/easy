#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
STR $0 = { .data = "> " };
STR $1 = { .data = "-" };
int main()
{
    a = 123;
    b = 123;
    auto $r1 = $concat("AiAi", $0, a, $1, b);
    $output("A", $r1);
}
