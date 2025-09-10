#include "runtime.c"
int i = {0};
int main()
{
    if_label:
    if (TRUE)
    {
        output("A", from_cstring("if_label"));
    }
    for_label:
    for (i = 0; i <= 0; i += 1)
    {
        output("A", from_cstring("for_label"));
    }
    i = 0;
    select_label:
    if (i == 0)
    {
        output("A", from_cstring("select_label"));
    }
    goto repeat_label;
    output("A", from_cstring("repeat_label_NO"));
    repeat_label:
    if (TRUE)
    {
        output("A", from_cstring("repeat_label"));
    }
    goto repent_label;
    output("A", from_cstring("repent_label_NO"));
    repent_label:
    if (TRUE)
    {
        output("A", from_cstring("repent_label"));
    }
    {
        output("A", from_cstring("block"));
    }
    goto block_label;
    {
        output("A", from_cstring("label block_NO"));
    }
    block_label:
    output("A", from_cstring("."));
    exit(0);
}
