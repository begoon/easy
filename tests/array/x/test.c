#include "preamble.c"
typedef int B[0 + 25][0 + 80];
int main()
{
    B b = {0};
    b[1][2] = b[2][1];
    output(1, ".");
}
