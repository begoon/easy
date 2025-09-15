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
                int data[0 + 100 + 1];
            } data[0 + 1 + 1];
        } data[0 + 1 + 1];
    } x;
    int y;
} p = {0};
int main()
{
    p.y = 123;
    p.x.data[0].data[1].data[100] = (p.y + 321);
    output("iAi", p.x.data[0].data[1].data[100], from_cstring(" "), p.y);
}
