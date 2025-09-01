#include "preamble.c"
int main()
{
    int i = {0};
    STR s = {0};
    scanf("%d", &i);
    scanf("%s", s.data);
    output(1, concat(3, "i = [", str(i), "]"));
    output(1, concat(3, "str = [", s.data, "]"));
    exit(0);
}
