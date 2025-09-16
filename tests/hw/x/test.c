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
    output("A", concat("Ai", $0, a));
    for (a = 0; a <= 10; a += 1)
    {
        output("i", a);
    }
    exit(0);
}
