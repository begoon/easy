#include "preamble.c"
int i = {0};
int main()
{
    if_label:
    if (1)
    {
        output("s", "if_label");
    }
    for_label:
    for (i = 0; i <= 0; i += 1)
    {
        output("s", "for_label");
    }
    i = 0;
    select_label:
    if (i == 0)
    {
        output("s", "select_label");
    }
    goto repeat_label;
    output("s", "repeat_label_NO");
    repeat_label:
    if (1)
    {
        output("s", "repeat_label");
    }
    goto repent_label;
    output("s", "repent_label_NO");
    repent_label:
    if (1)
    {
        output("s", "repent_label");
    }
    {
        output("s", "block");
    }
    goto block_label;
    {
        output("s", "label block_NO");
    }
    block_label:
    output("s", ".");
    exit(0);
}
