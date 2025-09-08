#include "preamble.c"
int x = {0};
void a(int x)
{
    {
        output(1, concat(2, "a(): ", strconv(x)));
    }
}
int b(int x)
{
    return (x + 1);
}
int main()
{
    a(100);
    x = b(100);
    {
        const char *fmt_1 = strconv_int(x);
        output(1, fmt_1);
    }
    exit(0);
}
