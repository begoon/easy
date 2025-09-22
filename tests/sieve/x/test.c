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
STR $F = { .data = "tests/sieve/test.easy" };
double abs(double x)
{
    const auto $r9 = (x < 0);
    if ($r9)
    {
        const auto $r10 = (-x);
        return $r10;
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    const auto $r11 = (a < 0);
    const auto $r12 = (a == 0);
    const auto $r13 = (a > 0);
    if ($r11)
    {
        $output("A", $0);
        exit(0);
    }
    else if ($r12)
    {
        return 0;
    }
    else if ($r13)
    {
        double x = 0.0;
        double ra = 0.0;
        double epsilon = 0.0;
        int sqrt = 0;
        const auto $r14 = FLOAT(a);
        ra = $r14;
        const auto $r15 = (1e-7 * ra);
        epsilon = $r15;
        const auto $r16 = (ra / 2.0);
        x = $r16;
        while (1)
        {
            const auto $r20 = (x * x);
            const auto $r19 = (ra - $r20);
            const auto $r18 = abs($r19);
            const auto $r17 = ($r18 > epsilon);
            const auto $r23 = (ra / x);
            const auto $r22 = ($r23 - x);
            const auto $r21 = ($r22 / 2.0);
            if (!($r17)) break;
            while (0);
            x += $r21;
        }
        const auto $r25 = FIX(x);
        const auto $r24 = ($r25 - 1);
        sqrt = $r24;
        while (1)
        {
            const auto $r28 = (sqrt + 1);
            const auto $r29 = (sqrt + 1);
            const auto $r27 = ($r28 * $r29);
            const auto $r26 = ($r27 <= a);
            if (!($r26)) break;
            while (0);
            sqrt += 1;
        }
        return sqrt;
    }
}
int main()
{
    scanf("%d", &topnum);
    const auto $r1 = (topnum > 0);
    if ($r1)
    {
        struct
        {
            int *data;
        } sieve = { .data = malloc(sizeof(int) * (topnum - 1 + 1)) };
        int i = 0;
        int limit = 0;
        int count = 0;
        const auto $r2 = (topnum + 1);
        topnum = $r2;
        i = 1;
        while (1)
        {
            if (!(i <= topnum)) break;
            $index(i, 1, topnum, &$F, 34, 39);
            sieve.data[(i) - (1)] = TRUE;
            i += 1;
        }
        const auto $r4 = integersqrt(topnum);
        const auto $r3 = ($r4 + 1);
        limit = $r3;
        i = 2;
        while (1)
        {
            if (!(i <= limit)) break;
            if (sieve.data[(i) - (1)])
            {
                int j = 0;
                const auto $r5 = (2 * i);
                j = $r5;
                while (1)
                {
                    if (!(j <= topnum)) break;
                    $index(j, 1, topnum, &$F, 39, 50);
                    sieve.data[(j) - (1)] = FALSE;
                    j += i;
                }
            }
            i += 1;
        }
        count = 0;
        i = 1;
        while (1)
        {
            if (!(i <= topnum)) break;
            if (sieve.data[(i) - (1)])
            {
                const auto $r6 = (count + 1);
                count = $r6;
                const auto $r7 = $concat("AiAi", $1, count, $2, i);
                $output("A", $r7);
            }
            i += 1;
        }
    }
    else
    {
        const auto $r8 = $concat("AiA", $3, topnum, $4);
        $output("A", $r8);
    }
    exit(0);
}
