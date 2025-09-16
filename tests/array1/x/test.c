#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
struct
{
    struct
    {
        struct
        {
            struct
            {
                int x;
                double y;
                int f;
            } data[0 + 80 + 1];
        } a;
        STR x;
    } data[0 + 25 + 1];
} b = {0};
STR $0 = { .data = "..." };
int main()
{
    b.data[1].a.data[2] = b.data[2].a.data[1];
    output("A", $0);
    exit(0);
}
