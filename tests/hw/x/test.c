#include "preamble.c"
int a = {0};
int main()
{
    a = 100;
    output("s", concat("si", "abc ", a));
    for (a = 0; a <= 10; a += 1)
    {
        output("i", a);
    }
    exit(0);
}
