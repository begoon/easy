#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int b = 0;
STR $0 = { .data = "abcXYZ", .sz = 6, .immutable = 1 };
STR $1 = { .data = "abcXYZ-*", .sz = 8, .immutable = 1 };
STR $2 = { .data = "12345", .sz = 5, .immutable = 1 };
STR $3 = { .data = "abc", .sz = 3, .immutable = 1 };
STR $F = { .data = "tests/string/test.easy", .sz = 22, .immutable = 1 };
int main_program()
{
    s = $0;
    const int $r1 = s.sz == $0.sz && memcmp(s.data, $0.data, s.sz) == 0;
    b = $r1;
    const int $r2 = $0.sz == s.sz && memcmp($0.data, s.data, $0.sz) == 0;
    b = $r2;
    const int $r3 = $0.sz == $1.sz && memcmp($0.data, $1.data, $0.sz) == 0;
    b = $r3;
    const STR $r4 = SUBSTR(s, 0, 6);
    const int $r5 = s.sz == $r4.sz && memcmp(s.data, $r4.data, s.sz) == 0;
    b = $r5;
    s = $0;
    const int $r6 = s.sz == $0.sz && memcmp(s.data, $0.data, s.sz) != 0;
    b = $r6;
    const int $r7 = $0.sz == s.sz && memcmp($0.data, s.data, $0.sz) != 0;
    b = $r7;
    const int $r8 = $0.sz == $1.sz && memcmp($0.data, $1.data, $0.sz) != 0;
    b = $r8;
    const STR $r9 = SUBSTR($2, 1, 2);
    $output("A", $r9);
    const STR $r10 = SUBSTR(s, 1, 2);
    $output("A", $r10);
    const STR $r12 = SUBSTR(s, 3, 2);
    const STR $r11 = $concat("AA", $3, $r12);
    $output("A", $r11);
}
