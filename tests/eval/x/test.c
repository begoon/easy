#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int main()
{
    a = (1 + (42 * 3));
    $output("i", a);
}
