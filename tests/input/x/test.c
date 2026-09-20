#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
double f = 0.0;
STR s = {0};
STR $0 = { .data = "i = [", .sz = 5 };
STR $1 = { .data = "]", .sz = 1 };
STR $2 = { .data = "f = [", .sz = 5 };
STR $3 = { .data = "s = [", .sz = 5 };
STR $F = { .data = "tests/input/test.easy", .sz = 21 };
int main_program()
{
    OWN(s, manage_string, $r1);
    scanf("%d", &i);
    scanf("%lf", &f);
    {
        char buf[4096];
        scanf("%4095s", buf);
        manage_string(&s, -1);
        s = make_string(buf, strlen(buf));
    }
    {
        const STR $r2 = $concat("AiA", $0, i, $1);
        OWN($r2, manage_string, $r3);
        $output("A", $r2);
    }
    {
        const STR $r4 = $concat("ArA", $2, f, $1);
        OWN($r4, manage_string, $r5);
        $output("A", $r4);
    }
    {
        const STR $r6 = $concat("AAA", $3, s, $1);
        OWN($r6, manage_string, $r7);
        $output("A", $r6);
    }
}
