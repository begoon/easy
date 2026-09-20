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
    const double $r20 = (x < 0.);
    if ($r20)
    {
        const int $r21 = (-x);
        return $r21;
    }
    else
    {
        return x;
    }
}
int integersqrt(int a)
{
    const int $r22 = (a < 0);
    const int $r23 = (a == 0);
    const int $r24 = (a > 0);
    if ($r22)
    {
        $output("A", $0);
        $exit();
    }
    else if ($r23)
    {
        return 0;
    }
    else if ($r24)
    {
        double x = 0.0;
        double ra = 0.0;
        double epsilon = 0.0;
        int sqrt = 0;
        const double $r25 = FLOAT(a);
        ra = $r25;
        const double $r26 = (0.0000001 * ra);
        epsilon = $r26;
        const double $r27 = (ra / 2.);
        x = $r27;
        while (1)
        {
            const double $r31 = (x * x);
            const double $r30 = (ra - $r31);
            const double $r29 = abs($r30);
            const double $r28 = ($r29 > epsilon);
            const double $r34 = (ra / x);
            const double $r33 = ($r34 - x);
            const double $r32 = ($r33 / 2.);
            if (!($r28)) break;
            while (0);
            x += $r32;
        }
        const int $r36 = FIX(x);
        const int $r35 = ($r36 - 1);
        sqrt = $r35;
        while (1)
        {
            const int $r39 = (sqrt + 1);
            const int $r40 = (sqrt + 1);
            const int $r38 = ($r39 * $r40);
            const int $r37 = ($r38 <= a);
            if (!($r37)) break;
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
        const int $r5 = (topnum > 0);
        if ($r5)
        {
            struct
            {
                int *data;
            } sieve = {0};
            OWN(sieve, manage_array, $r6);
            const int $r7 = 1;
            sieve.data = make_array($r7, topnum, sizeof(int), NULL);
            int i = 0;
            int limit = 0;
            int count = 0;
            i = 1;
            while (1)
            {
                if (!(i <= topnum)) break;
                const size_t $r8 = $array_index(sieve.data, i, &$F, 33, 39);
                sieve.data[$r8] = TRUE;
                i += 1;
            }
            const int $r10 = integersqrt(topnum);
            const int $r9 = ($r10 + 1);
            limit = $r9;
            i = 2;
            while (1)
            {
                if (!(i <= limit)) break;
                const size_t $r11 = $array_index(sieve.data, i, &$F, 36, 16);
                if (sieve.data[$r11])
                {
                    int j = 0;
                    const int $r12 = (2 * i);
                    j = $r12;
                    while (1)
                    {
                        if (!(j <= topnum)) break;
                        const size_t $r13 = $array_index(sieve.data, j, &$F, 38, 50);
                        sieve.data[$r13] = FALSE;
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
                        const size_t $r14 = $array_index(sieve.data, i, &$F, 43, 16);
                        if (sieve.data[$r14])
                        {
                            const int $r15 = (count + 1);
                            count = $r15;
                            {
                                const STR $r16 = $concat("AiAi", $2, count, $3, i);
                                OWN($r16, manage_string, $r17);
                                $output("A", $r16);
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
                const STR $r18 = $concat("AiA", $4, topnum, $5);
                OWN($r18, manage_string, $r19);
                $output("A", $r18);
            }
        }
    }
    $exit();
}
