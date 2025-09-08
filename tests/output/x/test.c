#include "preamble.c"
int a = {0};
double r = {0};
int b = {0};
STR s = {0};
int fa()
{
    return 1;
}
double fr()
{
    return 2.345;
}
int fb()
{
    return 1;
}
STR fs()
{
    STR s = {0};
    strcpy(s.data, "xyz");
    return s;
}
int main()
{
    {
        const char *fmt_1 = strconv_int(0);
        const char *fmt_3 = strconv_double(0.001);
        const char *fmt_5 = strconv_boolean(1);
        const char *fmt_7 = strconv_boolean(0);
        output(7, fmt_1, " - ", fmt_3, " = ", fmt_5, " <> ", fmt_7);
    }
    a = 321;
    r = 1.003;
    b = 1;
    strcpy(s.data, "abc");
    {
        const char *fmt_2 = strconv_int(a);
        const char *fmt_4 = strconv_double(r);
        const char *fmt_6 = strconv_boolean(b);
        const char *fmt_8 = strconv_cstr(&s);
        output(9, "i:", fmt_2, " r:", fmt_4, " b:", fmt_6, " s:", fmt_8, "%%");
    }
    {
        const char *fmt_3 = strconv_int(LENGTH("123"));
        const char *fmt_5 = strconv_int(FIX(23.56));
        const char *fmt_7 = strconv_double(FLOAT(543));
        output(7, CHARACTER(50), " ", fmt_3, " ", fmt_5, " ", fmt_7);
    }
    {
        output(1, concat(2, "123", "456"));
    }
    {
        const char *fmt_2 = strconv_int(fa());
        const char *fmt_4 = strconv_double(fr());
        const char *fmt_6 = strconv_boolean(fb());
        output(9, "i:", fmt_2, " r:", fmt_4, " b:", fmt_6, " s:", fs().data, "$$");
    }
    exit(0);
}
