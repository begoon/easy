#include "preamble.c"
int b = {0};
int p = {0};
int c = {0};
int main()
{
    {
        const char *fmt_1 = strconv_int(b);
        const char *fmt_2 = strconv_int(p.x.z);
        const char *fmt_3 = strconv_int(c.t[0][2][1]);
        output(3, fmt_1, fmt_2, fmt_3);
    }
}
