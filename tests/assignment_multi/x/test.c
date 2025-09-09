#include "runtime.c"
int a = {0};
int b = {0};
int main()
{
    a = 123;
    b = 123;
    output("A", concat("sisi", "> ", a, "-", b));
}
