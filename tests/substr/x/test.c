#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "abcXYZ", .sz = 6, .immutable = 1 };;
STR $1 = { .data = "12345", .sz = 5, .immutable = 1 };;
STR $2 = { .data = "abc", .sz = 3, .immutable = 1 };;
STR $F = { .data = "tests/substr/test.easy", .sz = 22, .immutable = 1 };;
int main_program()
{
    STR_copy(&s, $0);
    STR $r2 AUTOFREE = {0};
    STR_copy(&$r2, $1);
    const STR AUTOFREE $r1 = SUBSTR($r2, 1, 2);
    $output("A", $r1);
    STR $r4 AUTOFREE = {0};
    STR_copy(&$r4, s);
    const STR AUTOFREE $r3 = SUBSTR($r4, 1, 2);
    $output("A", $r3);
    STR $r7 AUTOFREE = {0};
    STR_copy(&$r7, s);
    const STR AUTOFREE $r6 = SUBSTR($r7, 3, 2);
    const STR AUTOFREE $r5 = $concat("AA", $2, $r6);
    $output("A", $r5);
    exit(0);
    STR_free(&s);
}
