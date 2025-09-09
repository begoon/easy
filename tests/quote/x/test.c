#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "\"");
    output("S", &s);
    output("s", "<1>");
    output("s", "");
    output("s", "<2>");
    output("s", "\"");
    output("s", "<3>");
    output("S", &s);
    output("s", "<4>");
    output("s", "...");
    strcpy(s.data, "\"");
    strcpy(s.data, "we said \"ok\"");
    strcpy(s.data, "str = [");
    strcpy(s.data, "abc = [");
    exit(0);
}
