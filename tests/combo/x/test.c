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
STR F1(int x, STR s)
{
    return concat("Aii", s, LENGTH(s), x);
}
void P1(int xx, STR s, STR xx2)
{
    int x = 0;
    x = 1;
    output("A", concat("iAiiAA", x, s, LENGTH(s), xx, xx2, from_cstring("**")));
}
int main()
{
    P1(123, from_cstring("123"), F1(456, from_cstring("456")));
    s.a.s = from_cstring("abc");
    output("iA", s.m.data[1].data[1], s.a.s);
    s.a.s = concat("AA", s.a.s, from_cstring("XYZ"));
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
    output("Ai", from_cstring("123"), i);
}
