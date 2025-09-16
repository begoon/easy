#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int data[1 + 32 + 1];
} Row;
Row row = {0};
int N = 0;
int n = 0;
int i = 0;
STR $0 = { .data = "X" };
STR $1 = { .data = "." };
STR $2 = { .data = "  " };
void step()
{
    int i = 0;
    Row next = {0};
    for (i = 1; i <= N; i += 1)
    {
        next.data[i] = 0;
    }
    for (i = 1; i <= N; i += 1)
    {
        int left = 0;
        int c = 0;
        int right = 0;
        int s = 0;
        int j = 0;
        j = (i - 1);
        if (j < 1)
        {
            j = N;
        }
        left = row.data[j];
        c = row.data[i];
        j = (i + 1);
        if (j > N)
        {
            j = 1;
        }
        right = row.data[j];
        s = ((left + c) + right);
        if (s == 2)
        {
            next.data[i] = 1;
        }
        else if ((s == 0) || (s == 3))
        {
            next.data[i] = 0;
        }
        else
        {
            if (c == 1)
            {
                next.data[i] = 1;
            }
            else
            {
                next.data[i] = right;
            }
        }
    }
    row = next;
}
void print()
{
    int i = 0;
    for (i = 1; i <= N; i += 1)
    {
        if (row.data[i] == 1)
        {
            output("A", $0);
        }
        else
        {
            output("A", $1);
        }
    }
    output("A", $2);
}
int main()
{
    N = 32;
    row.data[FIX((N / 2))] = 1;
    print();
    for (n = 1; n <= 10; n += 1)
    {
        step();
        print();
    }
}
