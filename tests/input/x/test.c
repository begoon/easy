#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
double f = 0.0;
STR s = {0};
STR $0 = { .data = "i = [" };
STR $1 = { .data = "]" };
STR $2 = { .data = "f = [" };
STR $3 = { .data = "s = [" };
int main()
{
    scanf("%d", &i);
    scanf("%lf", &f);
    scanf("%s", s.data);
    const STR $r1 = $concat("AiA", $0, i, $1);
    $output("A", $r1);
    const STR $r2 = $concat("ArA", $2, f, $1);
    $output("A", $r2);
    const STR $r3 = $concat("AAA", $3, s, $1);
    $output("A", $r3);
    exit(0);
}
