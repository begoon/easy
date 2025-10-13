#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
double f = 0.0;
STR s = {0};
STR $0 = { .data = "i = [", .sz = 5, .immutable = 1 };
STR $1 = { .data = "]", .sz = 1, .immutable = 1 };
STR $2 = { .data = "f = [", .sz = 5, .immutable = 1 };
STR $3 = { .data = "s = [", .sz = 5, .immutable = 1 };
STR $F = { .data = "tests/input/test.easy", .sz = 21, .immutable = 1 };
int main_program()
{
    scanf("%d", &i);
    scanf("%lf", &f);
    {
        char buf[4096];
        scanf("%4095s", buf);
        s = make_string(buf, strlen(buf));
    }
    const STR $r1 = $concat("AiA", $0, i, $1);
    $output("A", $r1);
    const STR $r2 = $concat("ArA", $2, f, $1);
    $output("A", $r2);
    const STR $r3 = $concat("AAA", $3, s, $1);
    $output("A", $r3);
}
