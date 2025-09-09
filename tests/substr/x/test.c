#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "abcXYZ");
    output("A", SUBSTR(from_cstring("12345"), 1, 2));
    output("A", SUBSTR(s, 1, 2));
    output("A", concat("sA", "abc", SUBSTR(s, 3, 2)));
    exit(0);
}
