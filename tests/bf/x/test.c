#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int data[0 + 299 + 1];
} Tape;
STR program = {0};
STR $0 = { .data = "" };
STR $1 = { .data = ">" };
STR $2 = { .data = "<" };
STR $3 = { .data = "+" };
STR $4 = { .data = "-" };
STR $5 = { .data = "." };
STR $6 = { .data = "[" };
STR $7 = { .data = "]" };
STR $8 = { .data = "+++++++++++[>+++++>+++<<-]>++++++++++++++ .---- .++++++++++++++++++ .++++++ .>." };
STR bf(STR program)
{
    Tape tape = {0};
    STR out = {0};
    int pc = 0;
    int ptr = 0;
    STR ch = {0};
    int depth = 0;
    tape.data[0] = 0;
    ptr = 0;
    out = $0;
    for (pc = 0; pc <= (LENGTH(program) - 1); pc += 1)
    {
        ch = SUBSTR(program, pc, 1);
        if (strcmp(ch.data, $1.data) == 0)
        {
            ptr = (ptr + 1);
        }
        if (strcmp(ch.data, $2.data) == 0)
        {
            ptr = (ptr - 1);
        }
        if (strcmp(ch.data, $3.data) == 0)
        {
            tape.data[ptr] = (tape.data[ptr] + 1);
        }
        if (strcmp(ch.data, $4.data) == 0)
        {
            tape.data[ptr] = (tape.data[ptr] - 1);
        }
        if (strcmp(ch.data, $5.data) == 0)
        {
            out = concat("AA", out, CHARACTER(tape.data[ptr]));
        }
        if (strcmp(ch.data, $6.data) == 0)
        {
            if (tape.data[ptr] == 0)
            {
                for (depth = 1; (depth > 0); depth += 0)
                {
                    pc = (pc + 1);
                    ch = SUBSTR(program, pc, 1);
                    if (strcmp(ch.data, $6.data) == 0)
                    {
                        depth = (depth + 1);
                    }
                    if (strcmp(ch.data, $7.data) == 0)
                    {
                        depth = (depth - 1);
                    }
                }
            }
        }
        if (strcmp(ch.data, $7.data) == 0)
        {
            if (tape.data[ptr] != 0)
            {
                for (depth = 1; (depth > 0); depth += 0)
                {
                    pc = (pc - 1);
                    ch = SUBSTR(program, pc, 1);
                    if (strcmp(ch.data, $7.data) == 0)
                    {
                        depth = (depth + 1);
                    }
                    if (strcmp(ch.data, $6.data) == 0)
                    {
                        depth = (depth - 1);
                    }
                }
            }
        }
    }
    return out;
}
int main()
{
    program = $8;
    output("A", bf(program));
}
