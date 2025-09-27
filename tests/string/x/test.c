#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int b = 0;
STR $0 = { .data = "abcXYZ" };
STR $1 = { .data = "12345" };
STR $2 = { .data = "abc" };
int main()
{
    s = $0;
    const int $r1 = strcmp(s.data, $0.data) == 0;
    b = $r1;
    const int $r2 = strcmp($0.data, s.data) == 0;
    b = $r2;
    const int $r3 = strcmp($0.data, $0.data) == 0;
    b = $r3;
    const int $r4 = strcmp(s.data, s.data) == 0;
    b = $r4;
    s = $0;
    const int $r5 = strcmp(s.data, $0.data) != 0;
    b = $r5;
    const int $r6 = strcmp($0.data, s.data) != 0;
    b = $r6;
    const int $r7 = strcmp($0.data, $0.data) != 0;
    b = $r7;
    const int $r8 = strcmp(s.data, s.data) != 0;
    b = $r8;
    const STR $r9 = SUBSTR($1, 1, 2);
    $output("A", $r9);
    const STR $r10 = SUBSTR(s, 1, 2);
    $output("A", $r10);
    const STR $r12 = SUBSTR(s, 3, 2);
    const STR $r11 = $concat("AA", $2, $r12);
    $output("A", $r11);
    exit(0);
}
