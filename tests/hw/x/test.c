#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int main()
{
    a = 100;
    output("A", concat("Ai", from_cstring("abc "), a));
    for (a = 0; a <= 10; a += 1)
    {
        output("i", a);
    }
    exit(0);
}
