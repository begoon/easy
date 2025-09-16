#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        int data[0 + 80 + 1];
    } data[0 + 25 + 1];
} Field;
int w = 0;
int h = 0;
Field field = {0};
int x = 0;
int y = 0;
int i = 0;
STR $0 = { .data = "** [ EASY LIFE ]" };
STR $1 = { .data = " " };
STR $2 = { .data = "*" };
STR $3 = { .data = "x" };
STR $4 = { .data = "GENERATION: " };
int valid(int x, int y)
{
    return (!((((x < 0) || (x >= w)) || (y < 0)) || (y >= h)));
}
int neighbours(int x, int y)
{
    int n = 0;
    int xx = 0;
    int yy = 0;
    n = 0;
    for (xx = (x - 1); xx <= (x + 1); xx += 1)
    {
        for (yy = (y - 1); yy <= (y + 1); yy += 1)
        {
            if ((xx != x) || (yy != y))
            {
                if (valid(xx, yy))
                {
                    if (field.data[yy].data[xx])
                    {
                        n = (n + 1);
                    }
                }
            }
        }
    }
    return n;
}
void print()
{
    int x = 0;
    int y = 0;
    output("AA", $0, $1);
    for (x = 0; x <= ((w + 1) - 17); x += 1)
    {
        output("A", $2);
    }
    output("A", concat("AA", $1, CHARACTER(13)));
    for (y = 0; y <= (h - 1); y += 1)
    {
        output("A", $2);
        for (x = 0; x <= (w - 1); x += 1)
        {
            if (field.data[y].data[x] == TRUE)
            {
                output("A", $3);
            }
            else
            {
                output("A", $1);
            }
        }
        output("A", concat("AA", $2, CHARACTER(13)));
    }
    for (x = 0; x <= (w + 1); x += 1)
    {
        output("A", $2);
    }
    output("A", concat("AA", $1, CHARACTER(13)));
}
void glider(int x, int y)
{
    field.data[y].data[x] = TRUE;
    field.data[y].data[(x + 1)] = TRUE;
    field.data[y].data[(x + 2)] = TRUE;
    field.data[(y + 1)].data[x] = TRUE;
    field.data[(y + 2)].data[(x + 1)] = TRUE;
}
void evolution()
{
    int x = 0;
    int y = 0;
    Field next = {0};
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            int alive = 0;
            int n = 0;
            alive = field.data[y].data[x];
            n = neighbours(x, y);
            if (alive == TRUE)
            {
                if ((n < 2) || (n > 3))
                {
                    alive = FALSE;
                }
            }
            else
            {
                if (n == 3)
                {
                    alive = TRUE;
                }
            }
            next.data[y].data[x] = alive;
        }
    }
    field = next;
}
int main()
{
    w = 80;
    h = 25;
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            field.data[y].data[x] = FALSE;
        }
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    for (i = 1; i <= 12; i += 1)
    {
        print();
        output("A", concat("Ai", $4, i));
        evolution();
        if ((i % 10) == 0)
        {
            glider(40, 10);
            glider(30, 15);
        }
    }
    exit(0);
}
