#include "preamble.c"
int i = {0};
int main()
{
    if_label: 
    if (TRUE)
    {
        output(1, "if_label");
    }
    for_label: 
    for (i = 0; i <= 0; i += 1)
    {
        output(1, "for_label");
    }
    i = 0;
    select_label: 
    if (i == 0)
    {
        output(1, "select_label");
    }
    goto repeat_label;
    output(1, "repeat_label_NO");
    repeat_label: 
    output(1, "repeat_label");
    goto repent_label;
    output(1, "repent_label_NO");
    repent_label: 
    output(1, "repent_label");
    {
        output(1, "block");
    }
    goto block_label;
    {
        output(1, "label block_NO");
    }
    block_label:
    output(1, ".");
    exit(0);
}
