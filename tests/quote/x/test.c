#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "\"");
    output(1, s);
    output(1, "");
    output(1, "\"");
    output(1, s);
    output(1, "...");
    exit(0);
}
