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
STR $0 = { .data = "." };
int main()
{
    t.a = 1;
    output("A", $0);
}
