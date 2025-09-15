#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
int b = 0;
int main()
{
    a = 123;
    b = 123;
    output("A", concat("AiAi", from_cstring("> "), a, from_cstring("-"), b));
}
