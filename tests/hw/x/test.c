#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
STR $0 = { .data = "abc " };
int main()
{
    a = 100;
    const STR $r1 = $concat("Ai", $0, a);
    $output("A", $r1);
    a = 0;
    while (1)
    {
        if (!(a <= 10)) break;
        $output("i", a);
        a += 1;
    }
    exit(0);
}
