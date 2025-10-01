#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "PROGRAM Q: DECLARE s STRING; SET s := %; OUTPUT SUBSTR(s, 0, 38) || CHARACTER(34) || s || CHARACTER(34) || SUBSTR(s, 39, LENGTH(s)-38); EXIT; END PROGRAM Q;", .sz = 156, .immutable = 1 };;
STR $F = { .data = "tests/quine/test.easy", .sz = 21, .immutable = 1 };;
int main_program()
{
    STR_copy(&s, $0);
    STR $r3 AUTOFREE = {0};
    STR_copy(&$r3, s);
    const STR AUTOFREE $r2 = SUBSTR($r3, 0, 38);
    const STR AUTOFREE $r4 = CHARACTER(34);
    const STR AUTOFREE $r5 = CHARACTER(34);
    STR $r7 AUTOFREE = {0};
    STR_copy(&$r7, s);
    STR $r10 AUTOFREE = {0};
    STR_copy(&$r10, s);
    const int $r9 = LENGTH($r10);
    const int $r8 = ($r9 - 38);
    const STR AUTOFREE $r6 = SUBSTR($r7, 39, $r8);
    const STR AUTOFREE $r1 = $concat("AAAAA", $r2, $r4, s, $r5, $r6);
    $output("A", $r1);
    exit(0);
    STR_free(&s);
}
