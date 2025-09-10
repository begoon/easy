#include "runtime.c"
typedef struct { int a; STR b; } T;
T t = {0};
int main()
{
    t.a = 1;
    output("A", from_cstring("."));
}
