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
    const auto $r2 = SUBSTR(s, 0, 38);
    const auto $r3 = CHARACTER(34);
    const auto $r4 = CHARACTER(34);
    const auto $r7 = LENGTH(s);
    const auto $r6 = ($r7 - 38);
    const auto $r5 = SUBSTR(s, 39, $r6);
    const auto $r1 = $concat("AAAAA", $r2, $r3, s, $r4, $r5);
    $output("A", $r1);
    exit(0);
}
