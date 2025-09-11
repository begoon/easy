#include "runtime.c"
int p = {0};
int main()
{
    p.x[0][1][100] = p.y;
}
