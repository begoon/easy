#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "PROGRAM Q: DECLARE s STRING; SET s := %; OUTPUT SUBSTR(s, 0, 38) || CHARACTER(34) || s || CHARACTER(34) || SUBSTR(s, 39, LENGTH(s)-38); EXIT; END PROGRAM Q;");
    output("A", concat("AASAA", SUBSTR(s, 0, 38), CHARACTER(34), &s, CHARACTER(34), SUBSTR(s, 39, (LENGTH(s) - 38))));
    exit(0);
}
