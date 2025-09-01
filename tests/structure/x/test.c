#include "preamble.c"
typedef struct { int a; STR b; } T;
int main()
{
    T t = {0};
    t.a = 1;
    output(1, ".");
}
