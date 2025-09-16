#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int n = 0;
STR $0 = { .data = "FizzBuzz" };
STR $1 = { .data = "Fizz" };
STR $2 = { .data = "Buzz" };
void FizzBuzz(int n)
{
    int i = 0;
    for (i = 1; i <= n; i += 1)
    {
        if (((i % 3) == 0) && ((i % 5) == 0))
        {
            output("A", $0);
        }
        else if ((i % 3) == 0)
        {
            output("A", $1);
        }
        else if ((i % 5) == 0)
        {
            output("A", $2);
        }
        else
        {
            output("i", i);
        }
    }
}
int main()
{
    FizzBuzz(20);
}
