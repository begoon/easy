#include "preamble.c"
int main()
{
    int a = {0};
    a = 100;
    if (a < 0)
    {
        output(1, "a < 0");
    } else
    if (a == 0)
    {
        output(1, "a = 0");
    }
    else
    {
        output(1, "otherwise");
    }
    exit(0);
}
