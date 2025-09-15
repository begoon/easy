#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int main()
{
    output("A", concat("AA", CHARACTER(48), CHARACTER(50)));
    output("A", CHARACTER(49));
    output("A", concat("AA", CHARACTER(13), CHARACTER(10)));
    output("A", concat("AA", from_cstring("123"), from_cstring("!")));
    output("A", concat("AAA", from_cstring("a"), from_cstring("b"), from_cstring("c")));
    exit(0);
}
