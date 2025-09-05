#include "preamble.c"
int x = {0};
void a(int x)
{
    output(1, concat(2, "a(): ", strconv(x)));
}
int b(int x)
{
    return (x + 1);
}
int main()
{
    a(100);
    x = b(100);
    output(1, strconv(x));
    exit(0);
}
