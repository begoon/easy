#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
STR $0 = { .data = "a < 0" };
STR $1 = { .data = "a = 0" };
STR $2 = { .data = "otherwise" };
int main()
{
    a = 100;
    if (a < 0)
    {
        $output("A", $0);
    }
    else if (a == 0)
    {
        $output("A", $1);
    }
    else
    {
        $output("A", $2);
    }
    exit(0);
}
