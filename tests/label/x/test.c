#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
int i = 0;
STR $0 = { .data = "if_label", .sz = 8, .immutable = 1 };
STR $1 = { .data = "for_label", .sz = 9, .immutable = 1 };
STR $2 = { .data = "select_label", .sz = 12, .immutable = 1 };
STR $3 = { .data = "repeat_label_NO", .sz = 15, .immutable = 1 };
STR $4 = { .data = "repeat_label", .sz = 12, .immutable = 1 };
STR $5 = { .data = "repent_label_NO", .sz = 15, .immutable = 1 };
STR $6 = { .data = "repent_label", .sz = 12, .immutable = 1 };
STR $7 = { .data = "block", .sz = 5, .immutable = 1 };
STR $8 = { .data = "label block_NO", .sz = 14, .immutable = 1 };
STR $9 = { .data = ".", .sz = 1, .immutable = 1 };
STR $F = { .data = "tests/label/test.easy", .sz = 21, .immutable = 1 };
int main_program()
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
    const int $r1 = (i == 0);
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
