#include "preamble.c"
int a = {0};
int main()
{
    a = 100;
    {
        output(1, concat(2, "abc ", strconv(a)));
    }
    for (a = 0; a <= 10; a += 1)
    {
        {
            const char *fmt_1 = strconv_int(a);
            output(1, fmt_1);
        }
    }
    exit(0);
}
