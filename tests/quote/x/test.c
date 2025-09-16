#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "\"" };
STR $1 = { .data = "<1>" };
STR $2 = { .data = "" };
STR $3 = { .data = "<2>" };
STR $4 = { .data = "<3>" };
STR $5 = { .data = "<4>" };
STR $6 = { .data = "..." };
STR $7 = { .data = "we said \"ok\"" };
STR $8 = { .data = "str = [" };
STR $9 = { .data = "abc = [" };
int main()
{
    s = $0;
    output("A", s);
    output("A", $1);
    output("A", $2);
    output("A", $3);
    output("A", $0);
    output("A", $4);
    output("A", s);
    output("A", $5);
    output("A", $6);
    s = $0;
    s = $7;
    s = $8;
    s = $9;
    exit(0);
}
