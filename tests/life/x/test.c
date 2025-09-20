#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    struct
    {
        int data[80 - 0 + 1];
    } data[25 - 0 + 1];
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
                    if (*(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, yy, 0, 25, sizeof(typeof(field.data[0])), "<yy|IDENT|tests/life/test.easy:23:22>"))->data, xx, 0, 80, sizeof(typeof(field.data[0].data[0])), "<xx|IDENT|tests/life/test.easy:23:26>"))
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
    $output("AA", $0, $1);
    for (x = 0; x <= ((w + 1) - 17); x += 1)
    {
        $output("A", $2);
    }
    $output("A", $concat("AA", $1, CHARACTER(13)));
    for (y = 0; y <= (h - 1); y += 1)
    {
        $output("A", $2);
        for (x = 0; x <= (w - 1); x += 1)
        {
            if (*(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:40:18>"))->data, x, 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:40:21>") == TRUE)
            {
                $output("A", $3);
            }
            else
            {
                $output("A", $1);
            }
        }
        $output("A", $concat("AA", $2, CHARACTER(13)));
    }
    for (x = 0; x <= (w + 1); x += 1)
    {
        $output("A", $2);
    }
    $output("A", $concat("AA", $1, CHARACTER(13)));
}
void glider(int x, int y)
{
    *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:52:15>"))->data, x, 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:52:18>") = TRUE;
    *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:53:15>"))->data, (x + 1), 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:53:18>") = TRUE;
    *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:54:15>"))->data, (x + 2), 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:54:18>") = TRUE;
    *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, (y + 1), 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:55:15>"))->data, x, 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:55:20>") = TRUE;
    *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, (y + 2), 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:56:15>"))->data, (x + 1), 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:56:20>") = TRUE;
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
            alive = *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:68:28>"))->data, x, 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:68:31>");
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
            *(typeof(next.data[0].data[0]) *)$ref(((typeof(next.data[0]) *)$ref(next.data, y, 0, 25, sizeof(typeof(next.data[0])), "<y|IDENT|tests/life/test.easy:77:18>"))->data, x, 0, 80, sizeof(typeof(next.data[0].data[0])), "<x|IDENT|tests/life/test.easy:77:21>") = alive;
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
            *(typeof(field.data[0].data[0]) *)$ref(((typeof(field.data[0]) *)$ref(field.data, y, 0, 25, sizeof(typeof(field.data[0])), "<y|IDENT|tests/life/test.easy:89:17>"))->data, x, 0, 80, sizeof(typeof(field.data[0].data[0])), "<x|IDENT|tests/life/test.easy:89:20>") = FALSE;
        }
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    for (i = 1; i <= 12; i += 1)
    {
        print();
        $output("A", $concat("Ai", $4, i));
        evolution();
        if ((i % 10) == 0)
        {
            glider(40, 10);
            glider(30, 15);
        }
    }
    exit(0);
}
