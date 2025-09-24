#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef int Int;
typedef double Real;
typedef int Boolean;
typedef STR String;
typedef struct
{
    int data[256 - 1 + 1];
} Row;
typedef Row RowAlias;
typedef struct
{
    Row data[8 - 1 + 1];
} Matrix;
typedef struct
{
    int a;
    STR s;
} Record;
typedef Record StructAlias;
typedef struct
{
    Record a;
    Matrix m;
} Record2;
typedef struct
{
    Matrix matrix;
    Row row;
    Record2 record;
} ComboType;
int i = 0;
int j = 0;
Row r = {0};
Matrix m = {0};
Matrix m1 = {0};
Record2 s = {0};
STR $0 = { .data = "**" };
STR $1 = { .data = "123" };
STR $2 = { .data = "456" };
STR $3 = { .data = "abc" };
STR $4 = { .data = "XYZ" };
STR $F = { .data = "tests/combo/test.easy" };
STR F1(int x, STR s)
{
    const int $r6 = LENGTH(s);
    const STR $r5 = $concat("Aii", s, $r6, x);
    return $r5;
}
void P1(int xx, STR s, STR xx2)
{
    int x = 0;
    x = 1;
    const int $r8 = LENGTH(s);
    const STR $r7 = $concat("iAiiAA", x, s, $r8, xx, xx2, $0);
    $output("A", $r7);
}
int main()
{
    const STR $r1 = F1(456, $2);
    P1(123, $1, $r1);
    s.a.s = $3;
    $index(1, 1, 8, &$F, 48, 14);
    $index(1, 1, 256, &$F, 48, 17);
    $output("iA", s.m.data[(1) - (1)].data[(1) - (1)], s.a.s);
    const STR $r2 = $concat("AA", s.a.s, $4);
    s.a.s = $r2;
    s.m = m1;
    i = 0;
    $index(1, 1, 256, &$F, 55, 9);
    r.data[(1) - (1)] = 1;
    $index(2, 1, 8, &$F, 56, 9);
    $index(3, 1, 256, &$F, 56, 12);
    m.data[(2) - (1)].data[(3) - (1)] = 1;
    $index(2, 1, 256, &$F, 57, 9);
    r.data[(2) - (1)] = 0;
    $index(2, 1, 8, &$F, 57, 17);
    $index(1, 1, 256, &$F, 57, 20);
    m.data[(2) - (1)].data[(1) - (1)] = 0;
    m1 = m;
    i = 1;
    while (1)
    {
        if (!(i <= 8)) break;
        j = 1;
        while (1)
        {
            if (!(j <= 256)) break;
            $index(i, 1, 8, &$F, 63, 13);
            $index(j, 1, 256, &$F, 63, 16);
            const int $r4 = (i * 100);
            const int $r3 = ($r4 + j);
            m.data[(i) - (1)].data[(j) - (1)] = $r3;
            j += 1;
        }
        i += 1;
    }
    $output("Ai", $1, i);
}
