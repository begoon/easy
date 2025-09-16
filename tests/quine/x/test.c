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
    output("A", concat("AAAAA", SUBSTR(s, 0, 38), CHARACTER(34), s, CHARACTER(34), SUBSTR(s, 39, (LENGTH(s) - 38))));
    exit(0);
}
