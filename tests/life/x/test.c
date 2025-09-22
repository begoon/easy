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
STR $F = { .data = "tests/life/test.easy" };
int valid(int x, int y)
{
    const auto $r10 = (x < 0);
    const auto $r11 = (x >= w);
    const auto $r9 = ($r10 || $r11);
    const auto $r12 = (y < 0);
    const auto $r8 = ($r9 || $r12);
    const auto $r13 = (y >= h);
    const auto $r7 = ($r8 || $r13);
    const auto $r6 = (!$r7);
    return $r6;
}
int neighbours(int x, int y)
{
    int n = 0;
    int xx = 0;
    int yy = 0;
    n = 0;
    const auto $r14 = (x - 1);
    xx = $r14;
    while (1)
    {
        const auto $r15 = (x + 1);
        if (!(xx <= $r15)) break;
        const auto $r16 = (y - 1);
        yy = $r16;
        while (1)
        {
            const auto $r17 = (y + 1);
            if (!(yy <= $r17)) break;
            const auto $r19 = (xx != x);
            const auto $r20 = (yy != y);
            const auto $r18 = ($r19 || $r20);
            if ($r18)
            {
                const auto $r21 = valid(xx, yy);
                if ($r21)
                {
                    if (field.data[(yy) - (0)].data[(xx) - (0)])
                    {
                        const auto $r22 = (n + 1);
                        n = $r22;
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
    $output("AA", $0, $1);
    x = 0;
    while (1)
    {
        const auto $r24 = (w + 1);
        const auto $r23 = ($r24 - 17);
        if (!(x <= $r23)) break;
        $output("A", $2);
        x += 1;
    }
    const auto $r26 = CHARACTER(13);
    const auto $r25 = $concat("AA", $1, $r26);
    $output("A", $r25);
    y = 0;
    while (1)
    {
        const auto $r27 = (h - 1);
        if (!(y <= $r27)) break;
        $output("A", $2);
        x = 0;
        while (1)
        {
            const auto $r28 = (w - 1);
            if (!(x <= $r28)) break;
            const auto $r29 = (field.data[(y) - (0)].data[(x) - (0)] == TRUE);
            if ($r29)
            {
                $output("A", $3);
            }
            else
            {
                $output("A", $1);
            }
            x += 1;
        }
        const auto $r31 = CHARACTER(13);
        const auto $r30 = $concat("AA", $2, $r31);
        $output("A", $r30);
        y += 1;
    }
    x = 0;
    while (1)
    {
        const auto $r32 = (w + 1);
        if (!(x <= $r32)) break;
        $output("A", $2);
        x += 1;
    }
    const auto $r34 = CHARACTER(13);
    const auto $r33 = $concat("AA", $1, $r34);
    $output("A", $r33);
}
void glider(int x, int y)
{
    $index(y, 0, 25, &$F, 52, 15);
    $index(x, 0, 80, &$F, 52, 18);
    field.data[(y) - (0)].data[(x) - (0)] = TRUE;
    $index(y, 0, 25, &$F, 53, 15);
    const auto $r35 = (x + 1);
    $index($r35, 0, 80, &$F, 53, 18);
    field.data[(y) - (0)].data[($r35) - (0)] = TRUE;
    $index(y, 0, 25, &$F, 54, 15);
    const auto $r36 = (x + 2);
    $index($r36, 0, 80, &$F, 54, 18);
    field.data[(y) - (0)].data[($r36) - (0)] = TRUE;
    const auto $r37 = (y + 1);
    $index($r37, 0, 25, &$F, 55, 15);
    $index(x, 0, 80, &$F, 55, 20);
    field.data[($r37) - (0)].data[(x) - (0)] = TRUE;
    const auto $r38 = (y + 2);
    $index($r38, 0, 25, &$F, 56, 15);
    const auto $r39 = (x + 1);
    $index($r39, 0, 80, &$F, 56, 20);
    field.data[($r38) - (0)].data[($r39) - (0)] = TRUE;
}
void evolution()
{
    int x = 0;
    int y = 0;
    Field next = {0};
    y = 0;
    while (1)
    {
        const auto $r40 = (h - 1);
        if (!(y <= $r40)) break;
        x = 0;
        while (1)
        {
            const auto $r41 = (w - 1);
            if (!(x <= $r41)) break;
            int alive = 0;
            int n = 0;
            alive = field.data[(y) - (0)].data[(x) - (0)];
            const auto $r42 = neighbours(x, y);
            n = $r42;
            const auto $r43 = (alive == TRUE);
            if ($r43)
            {
                const auto $r45 = (n < 2);
                const auto $r46 = (n > 3);
                const auto $r44 = ($r45 || $r46);
                if ($r44)
                {
                    alive = FALSE;
                }
            }
            else
            {
                const auto $r47 = (n == 3);
                if ($r47)
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
int main()
{
    w = 80;
    h = 25;
    y = 0;
    while (1)
    {
        const auto $r1 = (h - 1);
        if (!(y <= $r1)) break;
        x = 0;
        while (1)
        {
            const auto $r2 = (w - 1);
            if (!(x <= $r2)) break;
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
        const auto $r3 = $concat("Ai", $4, i);
        $output("A", $r3);
        evolution();
        const auto $r5 = (i % 10);
        const auto $r4 = ($r5 == 0);
        if ($r4)
        {
            glider(40, 10);
            glider(30, 15);
        }
        i += 1;
    }
    exit(0);
}
