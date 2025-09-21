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
    i = 1;
    while (1)
    {
        if (!(i <= n)) break;
        auto $r3 = (i % 3);
        auto $r2 = ($r3 == 0);
        auto $r5 = (i % 5);
        auto $r4 = ($r5 == 0);
        auto $r1 = ($r2 && $r4);
        auto $r7 = (i % 3);
        auto $r6 = ($r7 == 0);
        auto $r9 = (i % 5);
        auto $r8 = ($r9 == 0);
        if ($r1)
        {
            $output("A", $0);
        }
        else if ($r6)
        {
            $output("A", $1);
        }
        else if ($r8)
        {
            $output("A", $2);
        }
        else
        {
            $output("i", i);
        }
        i += 1;
    }
}
int main()
{
    FizzBuzz(20);
}
