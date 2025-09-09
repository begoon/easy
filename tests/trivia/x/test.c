#include "preamble.c"
int x = {0};
STR a = {0};
STR b = {0};
int main()
{
    x = concat("SA", &a, concat("SA", &b, SUBSTR(from_cstring("abc"), 0, 2)));
    exit(0);
}
