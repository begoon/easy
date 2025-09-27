#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int main()
{
    a = 0;
    while (1)
    {
        const int $r1 = (a < 7);
        if (!($r1 && a <= 10)) break;
        $output("i", a);
        a += 1;
    }
}
