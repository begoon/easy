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
    return 1;
}
STR fs()
{
    STR s = {0};
    strcpy(s.data, "xyz");
    return s;
}
int main()
{
    output("isrsbsb", 0, " - ", 0.001, " = ", 1, " <> ", 0);
    a = 321;
    r = 1.003;
    b = 1;
    strcpy(s.data, "abc");
    output("sisrsbsSs", "i:", a, " r:", r, " b:", b, " s:", &s, "%%");
    output("Asisisr", CHARACTER(50), " ", LENGTH(from_cstring("123")), " ", FIX(23.56), " ", FLOAT(543));
    output("A", concat("ss", "123", "456"));
    output("sisrsbsAs", "i:", fa(), " r:", fr(), " b:", fb(), " s:", fs(), "$$");
    exit(0);
}
