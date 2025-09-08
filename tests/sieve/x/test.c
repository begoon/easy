#include "preamble.c"
int topnum = {0};
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
    return 0;
}
int integersqrt(int a)
{
    if (a < 0)
    {
        {
            output(1, "a < 0 in FUNCTION integersqrt.");
        }
        exit(0);
    }
    else if (a == 0)
    {
        return 0;
    }
    else if (a > 0)
    {
        double x = {0};
        double ra = {0};
        double epsilon = {0};
        int sqrt = {0};
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
    return 0;
}
int main()
{
    scanf("%d", &topnum);
    if (topnum > 0)
    {
        int sieve[1 + topnum + /* @ */ 1];
        int i = {0};
        int limit = {0};
        int count = {0};
        topnum = (topnum + 1);
        for (i = 1; i <= topnum; i += 1)
        {
            sieve[i] = 1;
        }
        limit = (integersqrt(topnum) + 1);
        for (i = 2; i <= limit; i += 1)
        {
            if (sieve[i])
            {
                int j = {0};
                for (j = (2 * i); j <= topnum; j += i)
                {
                    sieve[j] = 0;
                }
            }
        }
        count = 0;
        for (i = 1; i <= topnum; i += 1)
        {
            if (sieve[i])
            {
                count = (count + 1);
                {
                    output(1, concat(4, "Prime[", strconv(count), "] = ", strconv(i)));
                }
            }
        }
    }
    else
    {
        {
            output(1, concat(3, "Input value ", strconv(topnum), " non-positive."));
        }
    }
    exit(0);
}
