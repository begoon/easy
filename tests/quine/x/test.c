#include "preamble.c"
STR s = {0};
int main()
{
    strcpy(s.data, "PROGRAM Q: DECLARE s STRING; SET s := %; OUTPUT SUBSTR(s, 0, 38) || CHARACTER(34) || s || CHARACTER(34) || SUBSTR(s, 39, LENGTH(s)-38); EXIT; END PROGRAM Q;");
    output(1, concat(5, SUBSTR(s.data, 0, 38), CHARACTER(34), s.data, CHARACTER(34), SUBSTR(s.data, 39, (LENGTH(s.data) - 38))));
    exit(0);
}
