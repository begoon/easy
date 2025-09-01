#include "preamble.c"
int main()
{
    int x = {0};
    STR a, b = {0};
    x = concat(2, a.data, concat(2, b.data, str(SUBSTR("abc", 0, 2))));
}
