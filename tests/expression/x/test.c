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
STR $0 = { .data = "3" };
STR $1 = { .data = "x = " };
STR $2 = { .data = " OK" };
STR $3 = { .data = "f = " };
STR $4 = { .data = "a" };
STR $5 = { .data = "abc" };
STR $6 = { .data = "s = " };
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
    x = (((-b) + (1 - 2)) - (LENGTH($0) * 7));
    x = (((-b) + (1 - 2)) - func(3));
    output("AiA", $1, x, $2);
    f = (!(a < b));
    f = ((a < b) ^ TRUE);
    f = ((a < b) || ((c >= 0) && FALSE));
    f = (((a >= b) && (c < 0)) || TRUE);
    output("AbA", $3, f, $2);
    s = concat("ii", a, b);
    s = concat("Ai", $4, b);
    s = concat("Aii", $4, b, c);
    s = concat("iA", a, concat("iA", b, SUBSTR($5, 0, 2)));
    s = concat("iiA", a, b, SUBSTR($5, 0, 2));
    s = concat("iA", a, CHARACTER(b));
    output("AAA", $6, s, $2);
}
