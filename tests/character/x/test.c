#include "preamble.c"
int main()
{
    output("s", concat("AA", CHARACTER(48), CHARACTER(50)));
    output("A", CHARACTER(49));
    output("s", concat("AA", CHARACTER(13), CHARACTER(10)));
    output("s", concat("ss", "123", "!"));
    output("s", concat("sss", "a", "b", "c"));
    exit(0);
}
