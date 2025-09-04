#include "preamble.c"
void a(int x)
{
    output(1, concat(2, "a(): ", strconv(x)));
}
int main()
{
    a(100);
    exit(0);
}
