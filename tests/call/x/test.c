#include "preamble.c"
int x = {0};
void a(int x)
{
    output("s", concat("si", "a(): ", x));
}
int b(int x)
{
    return (x + 1);
}
int main()
{
    a(100);
    x = b(100);
    output("i", x);
    exit(0);
}
