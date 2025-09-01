#include "preamble.c"
int main()
{
    output(1, concat(2, CHARACTER(48), CHARACTER(50)));
    output(1, CHARACTER(49));
    output(1, concat(2, CHARACTER(13), CHARACTER(10)));
    output(1, concat(2, "123", "!"));
    output(1, concat(3, "a", "b", "c"));
    exit(0);
}
