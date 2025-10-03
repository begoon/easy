#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int topnum = 0;
STR $0 = { .data = "a < 0 in FUNCTION integersqrt.", .sz = 30, .immutable = 1 };
STR $F = { .data = "tests/sieve/test.easy", .sz = 21, .immutable = 1 };
STR $2 = { .data = "Prime[", .sz = 6, .immutable = 1 };
STR $3 = { .data = "] = ", .sz = 4, .immutable = 1 };
STR $4 = { .data = "Input value ", .sz = 12, .immutable = 1 };
STR $5 = { .data = " non-positive.", .sz = 14, .immutable = 1 };
double abs(double x)
{
    const double $r9 = (x < 0.0);
    if ($r9)
    {
        const int $r10 = (-x);
        return $r10;
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    const int $r11 = (a < 0);
    const int $r12 = (a == 0);
    const int $r13 = (a > 0);
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
        const double $r14 = FLOAT(a);
        ra = $r14;
        const double $r15 = (1e-7 * ra);
        epsilon = $r15;
        const double $r16 = (ra / 2.0);
        x = $r16;
        while (1)
        {
            const double $r20 = (x * x);
            const double $r19 = (ra - $r20);
            const double $r18 = abs($r19);
            const double $r17 = ($r18 > epsilon);
            const double $r23 = (ra / x);
            const double $r22 = ($r23 - x);
            const double $r21 = ($r22 / 2.0);
            if (!($r17)) break;
            while (0);
            x += $r21;
        }
        const int $r25 = FIX(x);
        const int $r24 = ($r25 - 1);
        sqrt = $r24;
        while (1)
        {
            const int $r28 = (sqrt + 1);
            const int $r29 = (sqrt + 1);
            const int $r27 = ($r28 * $r29);
            const int $r26 = ($r27 <= a);
            if (!($r26)) break;
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
        void *$r2 AUTOFREE = malloc(sizeof(int) * (topnum - 1 + 1));
        struct
        {
            int *data;
        } sieve = { .data = $r2 };
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
        const int $r4 = integersqrt(topnum);
        const int $r3 = ($r4 + 1);
        limit = $r3;
        i = 2;
        while (1)
        {
            if (!(i <= limit)) break;
            $index(i, 1, topnum, &$F, 36, 16);
            if (sieve.data[(i) - (1)])
            {
                int j = 0;
                const int $r5 = (2 * i);
                j = $r5;
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
                const int $r6 = (count + 1);
                count = $r6;
                const STR $r7 = $concat("AiAi", $2, count, $3, i);
                $output("A", $r7);
            }
            i += 1;
        }
    }
    else
    {
        const STR $r8 = $concat("AiA", $4, topnum, $5);
        $output("A", $r8);
    }
    exit(0);
}
