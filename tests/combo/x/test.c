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
STR F1(int x, STR s)
{
    return $concat("Aii", s, LENGTH(s), x);
}
void P1(int xx, STR s, STR xx2)
{
    int x = 0;
    x = 1;
    $output("A", $concat("iAiiAA", x, s, LENGTH(s), xx, xx2, $0));
}
int main()
{
    P1(123, $1, F1(456, $2));
    s.a.s = $3;
    $output("iA", *(typeof(s.m.data[0].data[0]) *)$ref(((typeof(s.m.data[0]) *)$ref(s.m.data, 1, 1, 8, sizeof(typeof(s.m.data[0])), "<1|INTEGER|tests/combo/test.easy:48:14>"))->data, 1, 1, 256, sizeof(typeof(s.m.data[0].data[0])), "<1|INTEGER|tests/combo/test.easy:48:17>"), s.a.s);
    s.a.s = $concat("AA", s.a.s, $4);
    s.m = m1;
    i = 0;
    *(typeof(r.data[0]) *)$ref(r.data, 1, 1, 256, sizeof(typeof(r.data[0])), "<1|INTEGER|tests/combo/test.easy:55:9>") = 1;
    *(typeof(m.data[0].data[0]) *)$ref(((typeof(m.data[0]) *)$ref(m.data, 2, 1, 8, sizeof(typeof(m.data[0])), "<2|INTEGER|tests/combo/test.easy:56:9>"))->data, 3, 1, 256, sizeof(typeof(m.data[0].data[0])), "<3|INTEGER|tests/combo/test.easy:56:12>") = 1;
    *(typeof(r.data[0]) *)$ref(r.data, 2, 1, 256, sizeof(typeof(r.data[0])), "<2|INTEGER|tests/combo/test.easy:57:9>") = 0;
    *(typeof(m.data[0].data[0]) *)$ref(((typeof(m.data[0]) *)$ref(m.data, 2, 1, 8, sizeof(typeof(m.data[0])), "<2|INTEGER|tests/combo/test.easy:57:17>"))->data, 1, 1, 256, sizeof(typeof(m.data[0].data[0])), "<1|INTEGER|tests/combo/test.easy:57:20>") = 0;
    m1 = m;
    for (i = 1; i <= 8; i += 1)
    {
        for (j = 1; j <= 256; j += 1)
        {
            *(typeof(m.data[0].data[0]) *)$ref(((typeof(m.data[0]) *)$ref(m.data, i, 1, 8, sizeof(typeof(m.data[0])), "<i|IDENT|tests/combo/test.easy:63:13>"))->data, j, 1, 256, sizeof(typeof(m.data[0].data[0])), "<j|IDENT|tests/combo/test.easy:63:16>") = ((i * 100) + j);
        }
    }
    $output("Ai", $1, i);
}
