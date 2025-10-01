#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int a = 0;
double r = 0.0;
int b = 0;
STR s = {0};
STR $0 = { .data = "xyz", .sz = 3, .immutable = 1 };;
STR $1 = { .data = " - ", .sz = 3, .immutable = 1 };;
STR $2 = { .data = " = ", .sz = 3, .immutable = 1 };;
STR $3 = { .data = " <> ", .sz = 4, .immutable = 1 };;
STR $4 = { .data = "abc", .sz = 3, .immutable = 1 };;
STR $5 = { .data = "i:", .sz = 2, .immutable = 1 };;
STR $6 = { .data = " r:", .sz = 3, .immutable = 1 };;
STR $7 = { .data = " b:", .sz = 3, .immutable = 1 };;
STR $8 = { .data = " s:", .sz = 3, .immutable = 1 };;
STR $9 = { .data = "%%", .sz = 2, .immutable = 1 };;
STR $10 = { .data = " ", .sz = 1, .immutable = 1 };;
STR $11 = { .data = "123", .sz = 3, .immutable = 1 };;
STR $12 = { .data = "456", .sz = 3, .immutable = 1 };;
STR $13 = { .data = "$$", .sz = 2, .immutable = 1 };;
STR $F = { .data = "tests/concat/test.easy", .sz = 22, .immutable = 1 };;
int fa()
{
    int $result = 0;
    $result = 1;
    goto $end;
$end:
    return $result;
}
double fr()
{
    double $result = 0.0;
    $result = 2.345;
    goto $end;
$end:
    return $result;
}
int fb()
{
    int $result = 0;
    $result = TRUE;
    goto $end;
$end:
    return $result;
}
STR fs()
{
    STR $result = {0};
    STR AUTOFREE s = {0};
    STR_copy(&s, $0);
    STR_copy(&$result, s);
    goto $end;
$end:
    return $result;
}
int main_program()
{
    const STR AUTOFREE $r1 = $concat("iArAbAb", 0, $1, 0.001, $2, TRUE, $3, FALSE);
    $output("A", $r1);
    a = 321;
    r = 1.003;
    b = TRUE;
    STR_copy(&s, $4);
    const STR AUTOFREE $r2 = $concat("AiArAbAAA", $5, a, $6, r, $7, b, $8, s, $9);
    $output("A", $r2);
    const STR AUTOFREE $r4 = CHARACTER(50);
    STR $r6 AUTOFREE = {0};
    STR_copy(&$r6, $11);
    const int $r5 = LENGTH($r6);
    const int $r7 = FIX(23.56);
    const double $r8 = FLOAT(543);
    const STR AUTOFREE $r3 = $concat("AAiAiAr", $r4, $10, $r5, $10, $r7, $10, $r8);
    $output("A", $r3);
    const STR AUTOFREE $r9 = $concat("AA", $11, $12);
    $output("A", $r9);
    const int $r11 = fa();
    const double $r12 = fr();
    const int $r13 = fb();
    const STR AUTOFREE $r14 = fs();
    const STR AUTOFREE $r10 = $concat("AiArAbAAA", $5, $r11, $6, $r12, $7, $r13, $8, $r14, $13);
    $output("A", $r10);
    exit(0);
    STR_free(&s);
}
