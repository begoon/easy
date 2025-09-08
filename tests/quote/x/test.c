#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "\"");
    {
        const char *fmt_1 = strconv_cstr(&s);
        output(1, fmt_1);
    }
    {
        output(1, "<1>");
    }
    {
        output(1, "");
    }
    {
        output(1, "<2>");
    }
    {
        output(1, "\"");
    }
    {
        output(1, "<3>");
    }
    {
        const char *fmt_1 = strconv_cstr(&s);
        output(1, fmt_1);
    }
    {
        output(1, "<4>");
    }
    {
        output(1, "...");
    }
    strcpy(s.data, "\"");
    strcpy(s.data, "we said \"ok\"");
    strcpy(s.data, "str = [");
    strcpy(s.data, "abc = [");
    exit(0);
}
