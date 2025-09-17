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
            int x;
            double y;
        } data[0 + 80 + 1];
    } data[1 + 25 + 1];
} B;
B b = {0};
STR $0 = { .data = "." };
int main()
{
    b.data[1].data[2].x = b.data[2].data[1].x;
    $output("A", $0);
}
