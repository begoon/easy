#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        int score;
        STR name;
    } data[1 + 100 + 1];
} Field;
Field f = {0};
Field a(Field f)
{
    output("A", concat("AiAA", from_cstring("BEFORE: "), f.data[10].score, from_cstring(" "), f.data[10].name));
    f.data[10].score = 123;
    f.data[10].name = concat("AA", f.data[10].name, from_cstring("xyz"));
    return f;
}
int main()
{
    f.data[10].score = 321;
    f.data[10].name = from_cstring("abc");
    f = a(f);
    output("A", concat("AiAA", from_cstring("AFTER: "), f.data[10].score, from_cstring(" "), f.data[10].name));
    exit(0);
}
