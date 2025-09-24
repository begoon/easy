#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "PROGRAM Q: DECLARE s STRING; SET s := %; OUTPUT SUBSTR(s, 0, 38) || CHARACTER(34) || s || CHARACTER(34) || SUBSTR(s, 39, LENGTH(s)-38); EXIT; END PROGRAM Q;" };
int main()
{
    s = $0;
    const STR $r2 = SUBSTR(s, 0, 38);
    const STR $r3 = CHARACTER(34);
    const STR $r4 = CHARACTER(34);
    const int $r7 = LENGTH(s);
    const int $r6 = ($r7 - 38);
    const STR $r5 = SUBSTR(s, 39, $r6);
    const STR $r1 = $concat("AAAAA", $r2, $r3, s, $r4, $r5);
    $output("A", $r1);
    exit(0);
}
