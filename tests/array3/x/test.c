#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        struct
        {
            struct
            {
                int i;
                STR s;
            } data[0 + 4 + 1];
        } data[0 + 2 + 1];
    } data[0 + 8 + 1];
} A;
A a = {0};
A b = {0};
int main()
{
    a.data[1].data[2].data[3].i = 123;
    a.data[1].data[2].data[3].s = from_cstring("abc");
    b = a;
    output("iAi", a.data[1].data[2].data[3].i, from_cstring(" "), b.data[1].data[2].data[3].i);
    output("A", concat("AAA", a.data[1].data[2].data[3].s, from_cstring(" "), b.data[1].data[2].data[3].s));
    exit(0);
}
