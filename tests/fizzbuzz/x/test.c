#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int n = 0;
void FizzBuzz(int n)
{
    int i = 0;
    for (i = 1; i <= n; i += 1)
    {
        if (((i % 3) == 0) && ((i % 5) == 0))
        {
            output("A", from_cstring("FizzBuzz"));
        }
        else if ((i % 3) == 0)
        {
            output("A", from_cstring("Fizz"));
        }
        else if ((i % 5) == 0)
        {
            output("A", from_cstring("Buzz"));
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
