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
STR $F = { .data = "tests/life/test.easy", .sz = 20 };
STR $1 = { .data = "** [ EASY LIFE ]", .sz = 16 };
STR $2 = { .data = " ", .sz = 1 };
STR $3 = { .data = "*", .sz = 1 };
STR $4 = { .data = "x", .sz = 1 };
STR $5 = { .data = "GENERATION: ", .sz = 12 };
int valid(int x, int y)
{
    const int $r16 = (x < 0);
    const int $r17 = (x >= w);
    const int $r15 = ($r16 || $r17);
    const int $r18 = (y < 0);
    const int $r14 = ($r15 || $r18);
    const int $r19 = (y >= h);
    const int $r13 = ($r14 || $r19);
    const int $r12 = (!$r13);
    return $r12;
}
int neighbours(int x, int y)
{
    int n = 0;
    int xx = 0;
    int yy = 0;
    n = 0;
    const int $r20 = (x - 1);
    xx = $r20;
    while (1)
    {
        const int $r21 = (x + 1);
        if (!(xx <= $r21)) break;
        const int $r22 = (y - 1);
        yy = $r22;
        while (1)
        {
            const int $r23 = (y + 1);
            if (!(yy <= $r23)) break;
            const int $r25 = (xx != x);
            const int $r26 = (yy != y);
            const int $r24 = ($r25 || $r26);
            if ($r24)
            {
                const int $r27 = valid(xx, yy);
                if ($r27)
                {
                    $index(yy, 0, 25, &$F, 23, 22);
                    $index(xx, 0, 80, &$F, 23, 26);
                    if (field.data[(size_t)((int64_t)(yy) - (0))].data[(size_t)((int64_t)(xx) - (0))])
                    {
                        const int $r28 = (n + 1);
                        n = $r28;
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
        const int $r30 = (w + 1);
        const int $r29 = ($r30 - 17);
        if (!(x <= $r29)) break;
        $output("A", $3);
        x += 1;
    }
    {
        const STR $r32 = CHARACTER(13);
        OWN($r32, manage_string, $r33);
        const STR $r31 = $concat("AA", $2, $r32);
        OWN($r31, manage_string, $r34);
        $output("A", $r31);
    }
    {
        y = 0;
        while (1)
        {
            const int $r35 = (h - 1);
            if (!(y <= $r35)) break;
            $output("A", $3);
            x = 0;
            while (1)
            {
                const int $r36 = (w - 1);
                if (!(x <= $r36)) break;
                $index(y, 0, 25, &$F, 40, 18);
                $index(x, 0, 80, &$F, 40, 21);
                const int $r37 = (field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)(x) - (0))] == TRUE);
                if ($r37)
                {
                    $output("A", $4);
                }
                else
                {
                    $output("A", $2);
                }
                x += 1;
            }
            {
                const STR $r39 = CHARACTER(13);
                OWN($r39, manage_string, $r40);
                const STR $r38 = $concat("AA", $3, $r39);
                OWN($r38, manage_string, $r41);
                $output("A", $r38);
            }
            y += 1;
        }
    }
    x = 0;
    while (1)
    {
        const int $r42 = (w + 1);
        if (!(x <= $r42)) break;
        $output("A", $3);
        x += 1;
    }
    {
        const STR $r44 = CHARACTER(13);
        OWN($r44, manage_string, $r45);
        const STR $r43 = $concat("AA", $2, $r44);
        OWN($r43, manage_string, $r46);
        $output("A", $r43);
    }
}
void glider(int x, int y)
{
    $index(y, 0, 25, &$F, 52, 15);
    $index(x, 0, 80, &$F, 52, 18);
    field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)(x) - (0))] = TRUE;
    $index(y, 0, 25, &$F, 53, 15);
    const int $r47 = (x + 1);
    $index($r47, 0, 80, &$F, 53, 18);
    field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)($r47) - (0))] = TRUE;
    $index(y, 0, 25, &$F, 54, 15);
    const int $r48 = (x + 2);
    $index($r48, 0, 80, &$F, 54, 18);
    field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)($r48) - (0))] = TRUE;
    const int $r49 = (y + 1);
    $index($r49, 0, 25, &$F, 55, 15);
    $index(x, 0, 80, &$F, 55, 20);
    field.data[(size_t)((int64_t)($r49) - (0))].data[(size_t)((int64_t)(x) - (0))] = TRUE;
    const int $r50 = (y + 2);
    $index($r50, 0, 25, &$F, 56, 15);
    const int $r51 = (x + 1);
    $index($r51, 0, 80, &$F, 56, 20);
    field.data[(size_t)((int64_t)($r50) - (0))].data[(size_t)((int64_t)($r51) - (0))] = TRUE;
}
void evolution()
{
    int x = 0;
    int y = 0;
    Field next = {0};
    y = 0;
    while (1)
    {
        const int $r52 = (h - 1);
        if (!(y <= $r52)) break;
        x = 0;
        while (1)
        {
            const int $r53 = (w - 1);
            if (!(x <= $r53)) break;
            int alive = 0;
            int n = 0;
            $index(y, 0, 25, &$F, 68, 28);
            $index(x, 0, 80, &$F, 68, 31);
            alive = field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)(x) - (0))];
            const int $r54 = neighbours(x, y);
            n = $r54;
            const int $r55 = (alive == TRUE);
            if ($r55)
            {
                const int $r57 = (n < 2);
                const int $r58 = (n > 3);
                const int $r56 = ($r57 || $r58);
                if ($r56)
                {
                    alive = FALSE;
                }
            }
            else
            {
                const int $r59 = (n == 3);
                if ($r59)
                {
                    alive = TRUE;
                }
            }
            $index(y, 0, 25, &$F, 77, 18);
            $index(x, 0, 80, &$F, 77, 21);
            next.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)(x) - (0))] = alive;
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
            field.data[(size_t)((int64_t)(y) - (0))].data[(size_t)((int64_t)(x) - (0))] = FALSE;
            x += 1;
        }
        y += 1;
    }
    glider(30, 15);
    glider(40, 10);
    glider(50, 20);
    {
        i = 1;
        while (1)
        {
            if (!(i <= 12)) break;
            print();
            {
                const STR $r8 = $concat("Ai", $5, i);
                OWN($r8, manage_string, $r9);
                $output("A", $r8);
            }
            evolution();
            const int $r11 = (i % 10);
            const int $r10 = ($r11 == 0);
            if ($r10)
            {
                glider(40, 10);
                glider(30, 15);
            }
            i += 1;
        }
    }
    $exit();
}
