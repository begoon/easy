#include "runtime.c"
int a = {0};
int main()
{
    a = 100;
    if (a < 0)
    {
        output("A", from_cstring("a < 0"));
    }
    else if (a == 0)
    {
        output("A", from_cstring("a = 0"));
    }
    else
    {
        output("A", from_cstring("otherwise"));
    }
    exit(0);
}
