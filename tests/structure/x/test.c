#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int a;
    STR b;
} T;
T t = {0};
int main()
{
    t.a = 1;
    output("A", from_cstring("."));
}
