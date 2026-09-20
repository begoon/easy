#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "123", .sz = 3 };
STR $1 = { .data = "!", .sz = 1 };
STR $2 = { .data = "a", .sz = 1 };
STR $3 = { .data = "b", .sz = 1 };
STR $4 = { .data = "c", .sz = 1 };
STR $F = { .data = "tests/character/test.easy", .sz = 25 };
int main_program()
{
    {
        const STR $r2 = CHARACTER(48);
        OWN($r2, manage_string, $r3);
        const STR $r4 = CHARACTER(50);
        OWN($r4, manage_string, $r5);
        const STR $r1 = $concat("AA", $r2, $r4);
        OWN($r1, manage_string, $r6);
        $output("A", $r1);
    }
    {
        const STR $r7 = CHARACTER(49);
        OWN($r7, manage_string, $r8);
        $output("A", $r7);
    }
    {
        const STR $r10 = CHARACTER(13);
        OWN($r10, manage_string, $r11);
        const STR $r12 = CHARACTER(10);
        OWN($r12, manage_string, $r13);
        const STR $r9 = $concat("AA", $r10, $r12);
        OWN($r9, manage_string, $r14);
        $output("A", $r9);
    }
    {
        const STR $r15 = $concat("AA", $0, $1);
        OWN($r15, manage_string, $r16);
        $output("A", $r15);
    }
    {
        const STR $r17 = $concat("AAA", $2, $3, $4);
        OWN($r17, manage_string, $r18);
        $output("A", $r17);
    }
    $exit();
}
