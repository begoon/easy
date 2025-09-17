#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int x = 0;
STR $0 = { .data = "a(): " };
int b(int x)
{
    return (x + 1);
}
void a(int x)
{
    $output("A", $concat("Ai", $0, x));
}
int main()
{
    a(100);
    x = b(100);
    $output("i", x);
    exit(0);
}
