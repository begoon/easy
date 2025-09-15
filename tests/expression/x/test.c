#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int x = 0;
int a = 0;
int b = 0;
int c = 0;
STR s = {0};
int f = 0;
int func(int z)
{
    return z;
}
int main()
{
    x = a;
    x = (a + b);
    x = (+a);
    x = (-a);
    x = (a + (b * c));
    x = (((-b) + (1 - 2)) - (LENGTH(from_cstring("3")) * 7));
    x = (((-b) + (1 - 2)) - func(3));
    output("AiA", from_cstring("x = "), x, from_cstring(" OK"));
    f = (!(a < b));
    f = ((a < b) ^ TRUE);
    f = ((a < b) || ((c >= 0) && FALSE));
    f = (((a >= b) && (c < 0)) || TRUE);
    output("AbA", from_cstring("f = "), f, from_cstring(" OK"));
    s = concat("ii", a, b);
    s = concat("Ai", from_cstring("a"), b);
    s = concat("Aii", from_cstring("a"), b, c);
    s = concat("iA", a, concat("iA", b, SUBSTR(from_cstring("abc"), 0, 2)));
    s = concat("iiA", a, b, SUBSTR(from_cstring("abc"), 0, 2));
    s = concat("iA", a, CHARACTER(b));
    output("AAA", from_cstring("s = "), s, from_cstring(" OK"));
}
