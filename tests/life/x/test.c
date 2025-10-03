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
STR $F = { .data = "tests/life/test.easy", .sz = 20, .immutable = 1 };
STR $1 = { .data = "** [ EASY LIFE ]", .sz = 16, .immutable = 1 };
STR $2 = { .data = " ", .sz = 1, .immutable = 1 };
STR $3 = { .data = "*", .sz = 1, .immutable = 1 };
STR $4 = { .data = "x", .sz = 1, .immutable = 1 };
STR $5 = { .data = "GENERATION: ", .sz = 12, .immutable = 1 };
int valid(int x, int y)
{
    const int $r15 = (x < 0);
    const int $r16 = (x >= w);
    const int $r14 = ($r15 || $r16);
    const int $r17 = (y < 0);
    const int $r13 = ($r14 || $r17);
    const int $r18 = (y >= h);
    const int $r12 = ($r13 || $r18);
    const int $r11 = (!$r12);
    return $r11;
}
int neighbours(int x, int y)
{
    int n = 0;
    int xx = 0;
    int yy = 0;
    n = 0;
    const int $r19 = (x - 1);
    xx = $r19;
    while (1)
    {
        const int $r20 = (x + 1);
        if (!(xx <= $r20)) break;
        const int $r21 = (y - 1);
        yy = $r21;
        while (1)
        {
            const int $r22 = (y + 1);
            if (!(yy <= $r22)) break;
            const int $r24 = (xx != x);
            const int $r25 = (yy != y);
            const int $r23 = ($r24 || $r25);
            if ($r23)
            {
                const int $r26 = valid(xx, yy);
                if ($r26)
                {
                    $index(yy, 0, 25, &$F, 23, 22);
                    $index(xx, 0, 80, &$F, 23, 26);
                    if (field.data[(yy) - (0)].data[(xx) - (0)])
                    {
                        const int $r27 = (n + 1);
                        n = $r27;
                    }
                }
            }
            yy += 1;
        }
        xx += 1;
    }
    return n;
}
void print()
{
    int x = 0;
    int y = 0;
    $output("AA", $1, $2);
    x = 0;
    while (1)
    {
        const int $r29 = (w + 1);
        const int $r28 = ($r29 - 17);
        if (!(x <= $r28)) break;
        $output("A", $3);
        x += 1;
    }
    const STR $r31 = CHARACTER(13);
    const STR $r30 = $concat("AA", $2, $r31);
    $output("A", $r30);
    y = 0;
    while (1)
    {
        const int $r32 = (h - 1);
        if (!(y <= $r32)) break;
        $output("A", $3);
        x = 0;
        while (1)
        {
            const int $r33 = (w - 1);
            if (!(x <= $r33)) break;
            $index(y, 0, 25, &$F, 40, 18);
            $index(x, 0, 80, &$F, 40, 21);
            const int $r34 = (field.data[(y) - (0)].data[(x) - (0)] == TRUE);
            if ($r34)
            {
                $output("A", $4);
            }
            else
            {
                $output("A", $2);
            }
            x += 1;
        }
        const STR $r36 = CHARACTER(13);
        const STR $r35 = $concat("AA", $3, $r36);
        $output("A", $r35);
        y += 1;
    }
    x = 0;
    while (1)
    {
        const int $r37 = (w + 1);
        if (!(x <= $r37)) break;
        $output("A", $3);
        x += 1;
    }
    const STR $r39 = CHARACTER(13);
    const STR $r38 = $concat("AA", $2, $r39);
    $output("A", $r38);
}
void glider(int x, int y)
{
    $index(y, 0, 25, &$F, 52, 15);
    $index(x, 0, 80, &$F, 52, 18);
    field.data[(y) - (0)].data[(x) - (0)] = TRUE;
    $index(y, 0, 25, &$F, 53, 15);
    const int $r40 = (x + 1);
    $index($r40, 0, 80, &$F, 53, 18);
    field.data[(y) - (0)].data[($r40) - (0)] = TRUE;
    $index(y, 0, 25, &$F, 54, 15);
    const int $r41 = (x + 2);
    $index($r41, 0, 80, &$F, 54, 18);
    field.data[(y) - (0)].data[($r41) - (0)] = TRUE;
    const int $r42 = (y + 1);
    $index($r42, 0, 25, &$F, 55, 15);
    $index(x, 0, 80, &$F, 55, 20);
    field.data[($r42) - (0)].data[(x) - (0)] = TRUE;
    const int $r43 = (y + 2);
    $index($r43, 0, 25, &$F, 56, 15);
    const int $r44 = (x + 1);
    $index($r44, 0, 80, &$F, 56, 20);
    field.data[($r43) - (0)].data[($r44) - (0)] = TRUE;
}
void evolution()
{
    int x = 0;
    int y = 0;
    Field next = {0};
    y = 0;
    while (1)
    {
        const int $r45 = (h - 1);
        if (!(y <= $r45)) break;
        x = 0;
        while (1)
        {
            const int $r46 = (w - 1);
            if (!(x <= $r46)) break;
            int alive = 0;
            int n = 0;
            $index(y, 0, 25, &$F, 68, 28);
            $index(x, 0, 80, &$F, 68, 31);
            alive = field.data[(y) - (0)].data[(x) - (0)];
            const int $r47 = neighbours(x, y);
            n = $r47;
            const int $r48 = (alive == TRUE);
            if ($r48)
            {
                const int $r50 = (n < 2);
                const int $r51 = (n > 3);
                const int $r49 = ($r50 || $r51);
                if ($r49)
                {
                    alive = FALSE;
                }
            }
            else
            {
                const int $r52 = (n == 3);
                if ($r52)
                {
                    alive = TRUE;
                }
            }
            $index(y, 0, 25, &$F, 77, 18);
            $index(x, 0, 80, &$F, 77, 21);
            next.data[(y) - (0)].data[(x) - (0)] = alive;
            x += 1;
        }
        y += 1;
    }
    field = next;
}
int main_program()
{
    w = 80;
    h = 25;
    y = 0;
    while (1)
    {
        const int $r6 = (h - 1);
        if (!(y <= $r6)) break;
        x = 0;
        while (1)
        {
            const int $r7 = (w - 1);
            if (!(x <= $r7)) break;
            $index(y, 0, 25, &$F, 89, 17);
            $index(x, 0, 80, &$F, 89, 20);
            field.data[(y) - (0)].data[(x) - (0)] = FALSE;
            x += 1;
        }
        y += 1;
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    i = 1;
    while (1)
    {
        if (!(i <= 12)) break;
        print();
        const STR $r8 = $concat("Ai", $5, i);
        $output("A", $r8);
        evolution();
        const int $r10 = (i % 10);
        const int $r9 = ($r10 == 0);
        if ($r9)
        {
            glider(40, 10);
            glider(30, 15);
        }
        i += 1;
    }
    exit(0);
}
