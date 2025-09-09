#include "preamble.c"
int i = {0};
STR s = {0};
int main()
{
    scanf("%d", &i);
    scanf("%s", s.data);
    output("A", concat("sis", "i = [", i, "]"));
    output("A", concat("sSs", "str = [", &s, "]"));
    exit(0);
}
