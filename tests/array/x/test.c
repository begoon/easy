#include "runtime.c"
struct { struct { int x; double y; int f; } a[0 + 80 + 1]; STR x; } b[0 + 25 + 1];
int main()
{
    b[1].a[2] = b[2].a[1];
    output("A", from_cstring("..."));
    exit(0);
}
