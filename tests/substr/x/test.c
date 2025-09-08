#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "abcXYZ");
    {
        output(1, SUBSTR("12345", 1, 2));
    }
    {
        output(1, SUBSTR(s.data, 1, 2));
    }
    {
        output(1, concat(2, "abc", SUBSTR(s.data, 3, 2)));
    }
    exit(0);
}
