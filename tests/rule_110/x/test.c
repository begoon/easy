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
STR $F = { .data = "tests/rule_110/test.easy", .sz = 24, .immutable = 1 };
STR $1 = { .data = "X", .sz = 1, .immutable = 1 };
STR $2 = { .data = ".", .sz = 1, .immutable = 1 };
STR $3 = { .data = "  ", .sz = 2, .immutable = 1 };
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
        const int $r5 = (i - 1);
        j = $r5;
        const int $r6 = (j < 1);
        if ($r6)
        {
            j = N;
        }
        $index(j, 1, 32, &$F, 20, 23);
        left = row.data[(j) - (1)];
        $index(i, 1, 32, &$F, 22, 20);
        c = row.data[(i) - (1)];
        const int $r7 = (i + 1);
        j = $r7;
        const int $r8 = (j > N);
        if ($r8)
        {
            j = 1;
        }
        $index(j, 1, 32, &$F, 26, 24);
        right = row.data[(j) - (1)];
        const int $r10 = (left + c);
        const int $r9 = ($r10 + right);
        s = $r9;
        const int $r11 = (s == 2);
        const int $r13 = (s == 0);
        const int $r14 = (s == 3);
        const int $r12 = ($r13 || $r14);
        if ($r11)
        {
            $index(i, 1, 32, &$F, 31, 32);
            next.data[(i) - (1)] = 1;
        }
        else if ($r12)
        {
            $index(i, 1, 32, &$F, 32, 40);
            next.data[(i) - (1)] = 0;
        }
        else
        {
            const int $r15 = (c == 1);
            if ($r15)
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
        $index(i, 1, 32, &$F, 43, 14);
        const int $r16 = (row.data[(i) - (1)] == 1);
        if ($r16)
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
int main_program()
{
    N = 32;
    const int $r4 = (N / 2);
    const int $r3 = FIX($r4);
    $index($r3, 1, 32, &$F, 50, 11);
    row.data[($r3) - (1)] = 1;
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
