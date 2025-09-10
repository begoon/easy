#include "runtime.c"
int a = {0};
int b = {0};
int c = {0};
STR s = {0};
int main()
{
    a = 1;
    b = 2;
    c = ((a + 0) + b);
    output("iA", c, from_cstring("..."));
    s = concat("AA", s, from_cstring("abc"));
    output("A", s);
}
