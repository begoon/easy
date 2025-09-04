#include "preamble.c"
int a = {0};
int main()
{
    output(1, "00");
    output(2, "11", "22");
    a = 123;
    output(1, strconv(a));
    exit(0);
}
