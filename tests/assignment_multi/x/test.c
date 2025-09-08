#include "preamble.c"
int a = {0};
int b = {0};
int main()
{
    a = 123;
    b = 123;
    output(1, concat(4, "> ", strconv(a), "-", strconv(b)));
}
