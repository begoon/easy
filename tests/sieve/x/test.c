#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int topnum = 0;
STR $0 = { .data = "a < 0 in FUNCTION integersqrt." };
STR $1 = { .data = "Prime[" };
STR $2 = { .data = "] = " };
STR $3 = { .data = "Input value " };
STR $4 = { .data = " non-positive." };
double abs(double x)
{
    if (x < 0)
    {
        return (-x);
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    if (a < 0)
    {
        $output("A", $0);
        exit(0);
    }
    else if (a == 0)
    {
        return 0;
    }
    else if (a > 0)
    {
        double x = 0.0;
        double ra = 0.0;
        double epsilon = 0.0;
        int sqrt = 0;
        ra = FLOAT(a);
        epsilon = (1e-07 * ra);
        for (x = (ra / 2.0); (abs((ra - (x * x))) > epsilon); x += (((ra / x) - x) / 2.0))
        {
            ;
        }
        for (sqrt = (FIX(x) - 1); (((sqrt + 1) * (sqrt + 1)) <= a); sqrt += 1)
        {
            ;
        }
        return sqrt;
    }
}
int main()
{
    scanf("%d", &topnum);
    if (topnum > 0)
    {
        struct
        {
            int *data;
        } sieve = { .data = malloc(sizeof(int) * (1 + topnum + 1)) };
        int i = 0;
        int limit = 0;
        int count = 0;
        topnum = (topnum + 1);
        for (i = 1; i <= topnum; i += 1)
        {
            sieve.data[i] = TRUE;
        }
        limit = (integersqrt(topnum) + 1);
        for (i = 2; i <= limit; i += 1)
        {
            if (sieve.data[i])
            {
                int j = 0;
                for (j = (2 * i); j <= topnum; j += i)
                {
                    sieve.data[j] = FALSE;
                }
            }
        }
        count = 0;
        for (i = 1; i <= topnum; i += 1)
        {
            if (sieve.data[i])
            {
                count = (count + 1);
                $output("A", $concat("AiAi", $1, count, $2, i));
            }
        }
    }
    else
    {
        $output("A", $concat("AiA", $3, topnum, $4));
    }
    exit(0);
}
