#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
double f = 0.0;
STR s = {0};
int main()
{
    scanf("%d", &i);
    scanf("%lf", &f);
    scanf("%s", s.data);
    output("A", concat("AiA", from_cstring("i = ["), i, from_cstring("]")));
    output("A", concat("ArA", from_cstring("f = ["), f, from_cstring("]")));
    output("A", concat("AAA", from_cstring("s = ["), s, from_cstring("]")));
    exit(0);
}
