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
        output(1, "a < 0 in FUNCTION integersqrt.");
        exit(0);
    }
    if (a == 0)
    {
        return 0;
    }
    if (a > 0)
    {
        double x, ra;
        double epsilon;
        int sqrt;
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
    int topnum;
    scanf("%d", &topnum);
    if (topnum > 0)
    {
        int sieve[1 + topnum];
        int i, limit, count;
        topnum = (topnum + 1);
        for (i = 1; i <= topnum; i += 1)
        {
            sieve[i] = TRUE;
        }
        limit = (integersqrt(topnum) + 1);
        for (i = 2; i <= limit; i += 1)
        {
            if (sieve[i])
            {
                int j;
                for (j = (2 * i); j <= topnum; j += i)
                {
                    sieve[j] = FALSE;
                }
            }
        }
        count = 0;
        for (i = 1; i <= topnum; i += 1)
        {
            if (sieve[i])
            {
                count = (count + 1);
                output(1, concat(4, "Prime[", str(count), "] = ", str(i)));
            }
        }
    }
    else
    {
        output(1, concat(3, "Input value ", str(topnum), " non-positive."));
    }
    exit(0);
}
