#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int data[32 - 1 + 1];
} Row;
Row row = {0};
int N = 0;
int n = 0;
int i = 0;
STR $F = { .data = "tests/rule_110/test.easy" };
STR $1 = { .data = "X" };
STR $2 = { .data = "." };
STR $3 = { .data = "  " };
void step()
{
    int i = 0;
    Row next = {0};
    i = 1;
    while (1)
    {
        if (!(i <= N)) break;
        $index(i, 1, 32, &$F, 11, 33);
        next.data[(i) - (1)] = 0;
        i += 1;
    }
    i = 1;
    while (1)
    {
        if (!(i <= N)) break;
        int left = 0;
        int c = 0;
        int right = 0;
        int s = 0;
        int j = 0;
        const int $r3 = (i - 1);
        j = $r3;
        const int $r4 = (j < 1);
        if ($r4)
        {
            j = N;
        }
        left = row.data[(j) - (1)];
        c = row.data[(i) - (1)];
        const int $r5 = (i + 1);
        j = $r5;
        const int $r6 = (j > N);
        if ($r6)
        {
            j = 1;
        }
        right = row.data[(j) - (1)];
        const int $r8 = (left + c);
        const int $r7 = ($r8 + right);
        s = $r7;
        const int $r9 = (s == 2);
        const int $r11 = (s == 0);
        const int $r12 = (s == 3);
        const int $r10 = ($r11 || $r12);
        if ($r9)
        {
            $index(i, 1, 32, &$F, 31, 32);
            next.data[(i) - (1)] = 1;
        }
        else if ($r10)
        {
            $index(i, 1, 32, &$F, 32, 40);
            next.data[(i) - (1)] = 0;
        }
        else
        {
            const int $r13 = (c == 1);
            if ($r13)
            {
                $index(i, 1, 32, &$F, 33, 43);
                next.data[(i) - (1)] = 1;
            }
            else
            {
                $index(i, 1, 32, &$F, 33, 66);
                next.data[(i) - (1)] = right;
            }
        }
        i += 1;
    }
    row = next;
}
void print()
{
    int i = 0;
    i = 1;
    while (1)
    {
        if (!(i <= N)) break;
        const int $r14 = (row.data[(i) - (1)] == 1);
        if ($r14)
        {
            $output("A", $1);
        }
        else
        {
            $output("A", $2);
        }
        i += 1;
    }
    $output("A", $3);
}
int main()
{
    N = 32;
    const int $r2 = (N / 2);
    const int $r1 = FIX($r2);
    $index($r1, 1, 32, &$F, 50, 11);
    row.data[($r1) - (1)] = 1;
    print();
    n = 1;
    while (1)
    {
        if (!(n <= 10)) break;
        step();
        print();
        n += 1;
    }
}
