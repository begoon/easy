#include "preamble.c"
int i = {0};
STR s = {0};
int main()
{
    scanf("%d", &i);
    scanf("%s", s.data);
    output(1, concat(3, "i = [", strconv(i), "]"));
    output(1, concat(3, "str = [", s.data, "]"));
    exit(0);
}
