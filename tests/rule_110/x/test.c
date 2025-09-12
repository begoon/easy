#include "runtime.c"
typedef /* struct { int sz; unsigned char data[1 + 32 + 1]; } Row_ARRAY; */ int Row[1 + 32 + 1];
Row row = {0};
int N = {0};
int n = {0};
int i = {0};
void step()
{
    int i = {0};
    Row next = {0};
    for (i = 1; i <= N; i += 1)
    {
        next[i] = 0;
    }
    for (i = 1; i <= N; i += 1)
    {
        int left = {0};
        int c = {0};
        int right = {0};
        int s = {0};
        int j = {0};
        j = (i - 1);
        if (j < 1)
        {
            j = N;
        }
        left = row[j];
        c = row[i];
        j = (i + 1);
        if (j > N)
        {
            j = 1;
        }
        right = row[j];
        s = ((left + c) + right);
        if (s == 2)
        {
            next[i] = 1;
        }
        else if ((s == 0) || (s == 3))
        {
            next[i] = 0;
        }
        else
        {
            if (c == 1)
            {
                next[i] = 1;
            }
            else
            {
                next[i] = right;
            }
        }
    }
    for (i = 1; i <= N; i += 1)
    {
        row[i] = next[i];
    }
}
void print()
{
    int i = {0};
    for (i = 1; i <= N; i += 1)
    {
        if (row[i] == 1)
        {
            output("A", from_cstring("X"));
        }
        else
        {
            output("A", from_cstring("."));
        }
    }
    output("A", from_cstring("  "));
}
int main()
{
    N = 32;
    for (i = 1; i <= N; i += 1)
    {
        row[i] = 0;
    }
    i = FIX((N / 2));
    row[i] = 1;
    print();
    for (n = 1; n <= 10; n += 1)
    {
        step();
        print();
    }
}
