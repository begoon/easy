#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int topnum = 0;
STR $0 = { .data = "a < 0 in FUNCTION integersqrt.", .sz = 30 };
STR $F = { .data = "tests/sieve/test.easy", .sz = 21 };
STR $2 = { .data = "Prime[", .sz = 6 };
STR $3 = { .data = "] = ", .sz = 4 };
STR $4 = { .data = "Input value ", .sz = 12 };
STR $5 = { .data = " non-positive.", .sz = 14 };
double abs(double x)
{
    const double $r12 = (x < 0.);
    if ($r12)
    {
        const int $r13 = (-x);
        return $r13;
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    const int $r14 = (a < 0);
    const int $r15 = (a == 0);
    const int $r16 = (a > 0);
    if ($r14)
    {
        $output("A", $0);
        $exit();
    }
    else if ($r15)
    {
        return 0;
    }
    else if ($r16)
    {
        double x = 0.0;
        double ra = 0.0;
        double epsilon = 0.0;
        int sqrt = 0;
        const double $r17 = FLOAT(a);
        ra = $r17;
        const double $r18 = (0.0000001 * ra);
        epsilon = $r18;
        const double $r19 = (ra / 2.);
        x = $r19;
        while (1)
        {
            const double $r23 = (x * x);
            const double $r22 = (ra - $r23);
            const double $r21 = abs($r22);
            const double $r20 = ($r21 > epsilon);
            const double $r26 = (ra / x);
            const double $r25 = ($r26 - x);
            const double $r24 = ($r25 / 2.);
            if (!($r20)) break;
            while (0);
            x += $r24;
        }
        const int $r28 = FIX(x);
        const int $r27 = ($r28 - 1);
        sqrt = $r27;
        while (1)
        {
            const int $r31 = (sqrt + 1);
            const int $r32 = (sqrt + 1);
            const int $r30 = ($r31 * $r32);
            const int $r29 = ($r30 <= a);
            if (!($r29)) break;
            while (0);
            sqrt += 1;
        }
        return sqrt;
    }
}
int main_program()
{
    scanf("%d", &topnum);
    {
        const int $r1 = (topnum > 0);
        if ($r1)
        {
            void *$r2 = make_array((topnum - 1 + 1), sizeof(int), NULL);
            struct
            {
                int *data;
            } sieve = { .data = $r2 };
            OWN(sieve, manage_array, $r3);
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
            const int $r5 = integersqrt(topnum);
            const int $r4 = ($r5 + 1);
            limit = $r4;
            i = 2;
            while (1)
            {
                if (!(i <= limit)) break;
                $index(i, 1, topnum, &$F, 36, 16);
                if (sieve.data[(i) - (1)])
                {
                    int j = 0;
                    const int $r6 = (2 * i);
                    j = $r6;
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
            {
                i = 1;
                while (1)
                {
                    if (!(i <= topnum)) break;
                    {
                        $index(i, 1, topnum, &$F, 43, 16);
                        if (sieve.data[(i) - (1)])
                        {
                            const int $r7 = (count + 1);
                            count = $r7;
                            {
                                const STR $r8 = $concat("AiAi", $2, count, $3, i);
                                OWN($r8, manage_string, $r9);
                                $output("A", $r8);
                            }
                        }
                    }
                    i += 1;
                }
            }
        }
        else
        {
            {
                const STR $r10 = $concat("AiA", $4, topnum, $5);
                OWN($r10, manage_string, $r11);
                $output("A", $r10);
            }
        }
    }
    $exit();
}
