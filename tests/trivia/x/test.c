#include "runtime.c"
STR x = {0};
STR a = {0};
STR b = {0};
int main()
{
    x = concat("AA", a, concat("AA", b, SUBSTR(from_cstring("abc"), 0, 2)));
    exit(0);
}
