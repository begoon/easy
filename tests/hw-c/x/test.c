#include "preamble.c"
int a = {0};
int main()
{
    a = 100;
    output(1, concat(2, "abc ", strconv(a)));
    for (a = 0; a <= 10; a += 1)
    {
        output(1, strconv(a));
    }
    exit(0);
}
