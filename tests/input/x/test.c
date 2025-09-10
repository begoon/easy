#include "runtime.c"
int i = {0};
STR s = {0};
int main()
{
    scanf("%d", &i);
    scanf("%s", s.data);
    output("A", concat("AiA", from_cstring("i = ["), i, from_cstring("]")));
    output("A", concat("AAA", from_cstring("str = ["), s, from_cstring("]")));
    exit(0);
}
