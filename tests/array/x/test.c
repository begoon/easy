#include "preamble.c"
typedef int B[0 + 25][0 + 80];
B b = {0};
int main()
{
    b[1][2] = b[2][1];
    output(1, ".");
}
