#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
struct
{
    int data[0 + 99 + 1];
} a = {0};
int sz = 0;
int main()
{
    sz = 1024;
    {
        struct
        {
            int *data;
        } b = { .data = malloc(sizeof(int) * (1 + sz + 1)) };
        b.data[1] = 42;
        $output("i", b.data[1]);
    }
}
