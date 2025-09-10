#include "runtime.c"
int a = {0};
double r = {0};
int b = {0};
STR s = {0};
int fa()
{
    return 1;
}
double fr()
{
    return 2.345;
}
int fb()
{
    return TRUE;
}
STR fs()
{
    STR s = {0};
    s = from_cstring("xyz");
    return s;
}
int main()
{
    output("A", concat("iArAbAb", 0, from_cstring(" - "), 0.001, from_cstring(" = "), TRUE, from_cstring(" <> "), FALSE));
    a = 321;
    r = 1.003;
    b = TRUE;
    s = from_cstring("abc");
    output("A", concat("AiArAbAAA", from_cstring("i:"), a, from_cstring(" r:"), r, from_cstring(" b:"), b, from_cstring(" s:"), s, from_cstring("%%")));
    output("A", concat("AAiAiAr", CHARACTER(50), from_cstring(" "), LENGTH(from_cstring("123")), from_cstring(" "), FIX(23.56), from_cstring(" "), FLOAT(543)));
    output("A", concat("AA", from_cstring("123"), from_cstring("456")));
    output("A", concat("AiArAbAAA", from_cstring("i:"), fa(), from_cstring(" r:"), fr(), from_cstring(" b:"), fb(), from_cstring(" s:"), fs(), from_cstring("$$")));
    exit(0);
}
