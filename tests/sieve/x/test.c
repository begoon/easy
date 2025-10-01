#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int topnum = 0;
STR $0 = { .data = "a < 0 in FUNCTION integersqrt.", .sz = 30, .immutable = 1 };
STR $1 = { .data = "Prime[", .sz = 6, .immutable = 1 };
STR $2 = { .data = "] = ", .sz = 4, .immutable = 1 };
STR $3 = { .data = "Input value ", .sz = 12, .immutable = 1 };
STR $4 = { .data = " non-positive.", .sz = 14, .immutable = 1 };
STR $F = { .data = "tests/sieve/test.easy", .sz = 21, .immutable = 1 };
double abs(double x)
{
    const double $r8 = (x < 0.0);
    if ($r8)
    {
        const int $r9 = (-x);
        return $r9;
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    const int $r10 = (a < 0);
    const int $r11 = (a == 0);
    const int $r12 = (a > 0);
    if ($r10)
    {
        $output("A", $0);
        exit(0);
    }
    else if ($r11)
    {
        return 0;
    }
    else if ($r12)
    {
        double x = 0.0;
        double ra = 0.0;
        double epsilon = 0.0;
        int sqrt = 0;
        const double $r13 = FLOAT(a);
        ra = $r13;
        const double $r14 = (1e-7 * ra);
        epsilon = $r14;
        const double $r15 = (ra / 2.0);
        x = $r15;
        while (1)
        {
            const double $r19 = (x * x);
            const double $r18 = (ra - $r19);
            const double $r17 = abs($r18);
            const double $r16 = ($r17 > epsilon);
            const double $r22 = (ra / x);
            const double $r21 = ($r22 - x);
            const double $r20 = ($r21 / 2.0);
            if (!($r16)) break;
            while (0);
            x += $r20;
        }
        const int $r24 = FIX(x);
        const int $r23 = ($r24 - 1);
        sqrt = $r23;
        while (1)
        {
            const int $r27 = (sqrt + 1);
            const int $r28 = (sqrt + 1);
            const int $r26 = ($r27 * $r28);
            const int $r25 = ($r26 <= a);
            if (!($r25)) break;
            while (0);
            sqrt += 1;
        }
        return sqrt;
    }
}
int main_program()
{
    scanf("%d", &topnum);
    const int $r1 = (topnum > 0);
    if ($r1)
    {
        struct
        {
            int *data;
        } sieve = { .data = malloc(sizeof(int) * (topnum - 1 + 1)) };
        int i = 0;
        int limit = 0;
        int count = 0;
        i = 1;
        while (1)
        {
            if (!(i <= topnum)) break;
            $index(i, 1, topnum, &$F, 33, 39);
            sieve.data[(i) - (1)] = TRUE;
            i += 1;
        }
        const int $r3 = integersqrt(topnum);
        const int $r2 = ($r3 + 1);
        limit = $r2;
        i = 2;
        while (1)
        {
            if (!(i <= limit)) break;
            $index(i, 1, topnum, &$F, 36, 16);
            if (sieve.data[(i) - (1)])
            {
                int j = 0;
                const int $r4 = (2 * i);
                j = $r4;
                while (1)
                {
                    if (!(j <= topnum)) break;
                    $index(j, 1, topnum, &$F, 38, 50);
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
            $index(i, 1, topnum, &$F, 43, 16);
            if (sieve.data[(i) - (1)])
            {
                const int $r5 = (count + 1);
                count = $r5;
                const STR $r6 = $concat("AiAi", $1, count, $2, i);
                $output("A", $r6);
            }
            i += 1;
        }
    }
    else
    {
        const STR $r7 = $concat("AiA", $3, topnum, $4);
        $output("A", $r7);
    }
    exit(0);
}
