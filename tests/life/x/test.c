#include "runtime.c"
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
    output("ss", "** [ EASY LIFE ]", " ");
    for (x = 0; x <= ((w + 1) - 17); x += 1)
    {
        output("s", "*");
    }
    output("A", concat("sA", " ", CHARACTER(13)));
    for (y = 0; y <= (h - 1); y += 1)
    {
        output("s", "*");
        for (x = 0; x <= (w - 1); x += 1)
        {
            if (field[y][x] == 1)
            {
                output("s", "x");
            }
            else
            {
                output("s", " ");
            }
        }
        output("A", concat("sA", "*", CHARACTER(13)));
    }
    for (x = 0; x <= (w + 1); x += 1)
    {
        output("s", "*");
    }
    output("A", concat("sA", " ", CHARACTER(13)));
}
void glider(int x, int y)
{
    field[y][x] = 1;
    field[y][(x + 1)] = 1;
    field[y][(x + 2)] = 1;
    field[(y + 1)][x] = 1;
    field[(y + 2)][(x + 1)] = 1;
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
            if (alive == 1)
            {
                if ((n < 2) || (n > 3))
                {
                    alive = 0;
                }
            }
            else
            {
                if (n == 3)
                {
                    alive = 1;
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
            field[y][x] = 0;
        }
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    for (i = 1; i <= 12; i += 1)
    {
        print();
        output("A", concat("si", "GENERATION: ", i));
        evolution();
        if ((i % 10) == 0)
        {
            glider(40, 10);
            glider(30, 15);
        }
    }
    exit(0);
}
