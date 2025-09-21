#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
STR $0 = { .data = "if_label" };
STR $1 = { .data = "for_label" };
STR $2 = { .data = "select_label" };
STR $3 = { .data = "repeat_label_NO" };
STR $4 = { .data = "repeat_label" };
STR $5 = { .data = "repent_label_NO" };
STR $6 = { .data = "repent_label" };
STR $7 = { .data = "block" };
STR $8 = { .data = "label block_NO" };
STR $9 = { .data = "." };
int main()
{
    if_label:
    if (TRUE)
    {
        $output("A", $0);
    }
    for_label:
    i = 0;
    while (1)
    {
        if (!(i <= 0)) break;
        $output("A", $1);
        i += 1;
    }
    i = 0;
    select_label:
    auto $r1 = (i == 0);
    if ($r1)
    {
        $output("A", $2);
    }
    goto repeat_label;
    $output("A", $3);
    repeat_label:
    if (TRUE)
    {
        $output("A", $4);
    }
    goto repent_label;
    $output("A", $5);
    repent_label:
    if (TRUE)
    {
        $output("A", $6);
    }
    {
        $output("A", $7);
    }
    goto block_label;
    {
        $output("A", $8);
    }
    block_label:
    $output("A", $9);
    exit(0);
}
