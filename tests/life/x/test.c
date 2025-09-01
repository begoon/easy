#include "preamble.c"
typedef int Field[1 + (80 * 25)];
int offset(int x, int y, int w)
{
    return ((y * w) + x);
    return 0;
}
int get(int x, int y, int w, int h, Field field)
{
    if ((((x < 0) || (x >= w)) || (y < 0)) || (y >= h))
    {
        return 0;
    }
    return field[offset(x, y, w)];
    return 0;
}
void set(int x, int y, int w, int h, Field field, int v){
    if ((((x >= 0) && (x < w)) && (y >= 0)) && (y < h))
    {
        field[offset(x, y, w)] = v;
    }
}
int neighbours(int x, int y, int w, int h, Field field)
{
    int n = {0};
    int xx, yy = {0};
    n = 0;
    for (xx = (-1); xx <= 1; xx += 1)
    {
        for (yy = (-1); yy <= 1; yy += 1)
        {
            if ((xx != 0) || (yy != 0))
            {
                n = (n + get((x + xx), (y + yy), w, h, field));
            }
        }
    }
    return n;
    return 0;
}
void clearscreen(){
    output(6, CHARACTER(27), "[", "H", CHARACTER(27), "[", "J");
}
void print(int w, int h, Field field){
    int x, y = {0};
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
            int i = {0};
            i = offset(x, y, w);
            if (field[i] == TRUE)
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
void glider(int w, int h, int x, int y, Field field){
    set(x, y, w, h, field, TRUE);
    set((x + 1), y, w, h, field, TRUE);
    set((x + 2), y, w, h, field, TRUE);
    set(x, (y + 1), w, h, field, TRUE);
    set((x + 1), (y + 2), w, h, field, TRUE);
}
void evolution(int w, int h, Field field){
    int x, y = {0};
    int n = {0};
    Field next = {0};
    for (y = 0; y <= (h - 1); y += 1)
    {
        for (x = 0; x <= (w - 1); x += 1)
        {
            int alive = {0};
            alive = get(x, y, w, h, field);
            n = neighbours(x, y, w, h, field);
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
            set(x, y, w, h, next, alive);
        }
    }
    for (n = 0; n <= ((w * h) - 1); n += 1)
    {
        field[n] = next[n];
    }
}
int main()
{
    int w, h = {0};
    Field field = {0};
    int i = {0};
    w = 80;
    h = 25;
    for (i = 0; i <= (w * h); i += 1)
    {
        field[i] = 0;
    }
    glider(w, h, 30, 15, field);
    glider(w, h, 40, 10, field);
    glider(w, h, 50, 20, field);
    for (i = 1; i <= 2; i += 1)
    {
        evolution(w, h, field);
    }
    print(w, h, field);
    output(1, concat(2, "GENERATION: ", str(i)));
    exit(0);
}
