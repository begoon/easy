#include "runtime.c"
int a = {0};
int main()
{
    a = 100;
    if (a < 0)
    {
        output("s", "a < 0");
    }
    else if (a == 0)
    {
        output("s", "a = 0");
    }
    else
    {
        output("s", "otherwise");
    }
    exit(0);
}
