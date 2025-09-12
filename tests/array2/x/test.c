#include "runtime.c"
typedef struct { int x; double y; } B[1 + 25 + 1][0 + 80];
B b = {0};
int main()
{
    b[1][2].x = b[2][1].x;
    output("A", from_cstring("."));
}
