#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
int b = 0;
STR $0 = { .data = "abcXYZ", .sz = 6 };
STR $1 = { .data = "abcXYZ-*", .sz = 8 };
STR $2 = { .data = "12345", .sz = 5 };
STR $3 = { .data = "abc", .sz = 3 };
STR $F = { .data = "tests/string/test.easy", .sz = 22 };
int main_program()
{
    OWN(s, manage_string, $r1);
    __auto_type $r2 = $0;
    manage_string(&$r2, 1);
    manage_string(&s, -1);
    s = $r2;
    const int $r3 = s.sz == $0.sz && (s.sz ? memcmp(s.data, $0.data, s.sz) : 0) == 0;
    b = $r3;
    const int $r4 = $0.sz == s.sz && ($0.sz ? memcmp($0.data, s.data, $0.sz) : 0) == 0;
    b = $r4;
    const int $r5 = $0.sz == $1.sz && ($0.sz ? memcmp($0.data, $1.data, $0.sz) : 0) == 0;
    b = $r5;
    {
        const STR $r6 = SUBSTR(s, 0, 6);
        OWN($r6, manage_string, $r7);
        const int $r8 = s.sz == $r6.sz && (s.sz ? memcmp(s.data, $r6.data, s.sz) : 0) == 0;
        b = $r8;
    }
    __auto_type $r9 = $0;
    manage_string(&$r9, 1);
    manage_string(&s, -1);
    s = $r9;
    const int $r10 = s.sz == $0.sz && (s.sz ? memcmp(s.data, $0.data, s.sz) : 0) != 0;
    b = $r10;
    const int $r11 = $0.sz == s.sz && ($0.sz ? memcmp($0.data, s.data, $0.sz) : 0) != 0;
    b = $r11;
    const int $r12 = $0.sz == $1.sz && ($0.sz ? memcmp($0.data, $1.data, $0.sz) : 0) != 0;
    b = $r12;
    {
        const STR $r13 = SUBSTR($2, 1, 2);
        OWN($r13, manage_string, $r14);
        $output("A", $r13);
    }
    {
        const STR $r15 = SUBSTR(s, 1, 2);
        OWN($r15, manage_string, $r16);
        $output("A", $r15);
    }
    {
        const STR $r18 = SUBSTR(s, 3, 2);
        OWN($r18, manage_string, $r19);
        const STR $r17 = $concat("AA", $3, $r18);
        OWN($r17, manage_string, $r20);
        $output("A", $r17);
    }
}
