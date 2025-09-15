#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int main()
{
    s = from_cstring("abcXYZ");
    output("A", SUBSTR(from_cstring("12345"), 1, 2));
    output("A", SUBSTR(s, 1, 2));
    output("A", concat("AA", from_cstring("abc"), SUBSTR(s, 3, 2)));
    exit(0);
}
