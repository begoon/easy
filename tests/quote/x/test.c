#include "runtime.c"
STR s = {0};
int main()
{
    s = from_cstring("\"");
    output("A", s);
    output("A", from_cstring("<1>"));
    output("A", from_cstring(""));
    output("A", from_cstring("<2>"));
    output("A", from_cstring("\""));
    output("A", from_cstring("<3>"));
    output("A", s);
    output("A", from_cstring("<4>"));
    output("A", from_cstring("..."));
    s = from_cstring("\"");
    s = from_cstring("we said \"ok\"");
    s = from_cstring("str = [");
    s = from_cstring("abc = [");
    exit(0);
}
