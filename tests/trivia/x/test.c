#include "preamble.c"
int x = {0};
STR a, b = {0};
int main()
{
    x = concat(2, a.data, concat(2, b.data, SUBSTR("abc", 0, 2)));
}
