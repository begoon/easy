#include "preamble.c"
typedef int Field[0 + 25 + /* @ */ 1][0 + 80];
int w = {0};
int h = {0};
Field field = {0};
int x = {0};
int y = {0};
int i = {0};
int valid(int x, int y)
{
    return (!((((x < 0) || (x >= w)) || (y < 0)) || (y >= h)));
}
int neighbours(int x, int y)
{
    int n = {0};
    int xx = {0};
    int yy = {0};
    n = 0;
    for (xx = (x - 1); xx <= (x + 1); xx += 1)
    {
        for (yy = (y - 1); yy <= (y + 1); yy += 1)
        {
            if ((xx != x) || (yy != y))
            {
                if (valid(xx, yy))
                {
                    if (field[yy][xx])
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
    int x = {0};
    int y = {0};
    output(2, "** [ EASY LIFE ]", " ");
    for (x = 0; x <= ((w + 1) - 17); x += 1)
    {
        output(1, "*");
    }
    output(1, concat(2, " ", CHARACTER(13)));
    for (y = 0; y <= (h - 1); y += 1)
    {
        output(1, "*");
        for (x = 0; x <= (w - 1); x += 1)
        {
            if (field[y][x] == TRUE)
            {
                output(1, "x");
            }
            else
            {
                output(1, " ");
            }
        }
        output(1, concat(2, "*", CHARACTER(13)));
    }
    for (x = 0; x <= (w + 1); x += 1)
    {
        output(1, "*");
    }
    output(1, concat(2, " ", CHARACTER(13)));
}
void glider(int x, int y)
{
    field[y][x] = TRUE;
    field[y][(x + 1)] = TRUE;
    field[y][(x + 2)] = TRUE;
    field[(y + 1)][x] = TRUE;
    field[(y + 2)][(x + 1)] = TRUE;
}
void evolution()
{
    int x = {0};
    int y = {0};
    int n = {0};
    Field next = {0};
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            int alive = {0};
            alive = field[y][x];
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
            next[y][x] = alive;
        }
    }
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            field[y][x] = next[y][x];
        }
    }
}
int main()
{
    w = 80;
    h = 25;
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            field[y][x] = FALSE;
        }
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    for (i = 1; i <= 12; i += 1)
    {
        print();
        output(1, concat(2, "GENERATION: ", strconv(i)));
        evolution();
        if ((i % 10) == 0)
        {
            glider(40, 10);
            glider(30, 15);
        }
    }
    exit(0);
}
