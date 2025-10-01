#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int n = 0;
STR $0 = { .data = "FizzBuzz", .sz = 8, .immutable = 1 };
STR $1 = { .data = "Fizz", .sz = 4, .immutable = 1 };
STR $2 = { .data = "Buzz", .sz = 4, .immutable = 1 };
STR $F = { .data = "tests/fizzbuzz/test.easy", .sz = 24, .immutable = 1 };
void FizzBuzz(int n)
{
    int i = 0;
    i = 1;
    while (1)
    {
        if (!(i <= n)) break;
        const int $r3 = (i % 3);
        const int $r2 = ($r3 == 0);
        const int $r5 = (i % 5);
        const int $r4 = ($r5 == 0);
        const int $r1 = ($r2 && $r4);
        const int $r7 = (i % 3);
        const int $r6 = ($r7 == 0);
        const int $r9 = (i % 5);
        const int $r8 = ($r9 == 0);
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
int main_program()
{
    FizzBuzz(20);
}
