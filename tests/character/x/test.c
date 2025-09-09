#include "preamble.c"
int main()
{
    output("A", concat("AA", CHARACTER(48), CHARACTER(50)));
    output("A", CHARACTER(49));
    output("A", concat("AA", CHARACTER(13), CHARACTER(10)));
    output("A", concat("ss", "123", "!"));
    output("A", concat("sss", "a", "b", "c"));
    exit(0);
}
