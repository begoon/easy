#include "preamble.c"
int i = {0};
STR s = {0};
int main()
{
    scanf("%d", &i);
    scanf("%s", s.data);
    output("s", concat("sis", "i = [", i, "]"));
    output("s", concat("sSs", "str = [", &s, "]"));
    exit(0);
}
