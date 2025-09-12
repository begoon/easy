#include "runtime.c"
typedef /* struct { int sz; unsigned char data[0 + 25 + 1]; } B_ARRAY; */ int B[0 + 25 + 1][0 + 80];
B b = {0};
int main()
{
    b[1][2] = b[2][1];
    output("A", from_cstring("."));
    exit(0);
}
