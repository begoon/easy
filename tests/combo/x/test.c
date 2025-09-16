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
    int data[1 + 256 + 1];
} Row;
typedef Row RowAlias;
typedef struct
{
    Row data[1 + 8 + 1];
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
STR F1(int x, STR s)
{
    return concat("Aii", s, LENGTH(s), x);
}
void P1(int xx, STR s, STR xx2)
{
    int x = 0;
    x = 1;
    output("A", concat("iAiiAA", x, s, LENGTH(s), xx, xx2, $0));
}
int main()
{
    P1(123, $1, F1(456, $2));
    s.a.s = $3;
    output("iA", s.m.data[1].data[1], s.a.s);
    s.a.s = concat("AA", s.a.s, $4);
    s.m = m1;
    i = 0;
    r.data[1] = 1;
    m.data[2].data[3] = 1;
    r.data[2] = 0;
    m.data[2].data[1] = 0;
    m1 = m;
    for (i = 1; i <= 8; i += 1)
    {
        for (j = 1; j <= 256; j += 1)
        {
            m.data[i].data[j] = ((i * 100) + j);
        }
    }
    output("Ai", $1, i);
}
