#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int data[299 - 0 + 1];
} Tape;
STR program = {0};
STR $0 = { .data = "" };
STR $1 = { .data = ">" };
STR $2 = { .data = "<" };
STR $3 = { .data = "+" };
STR $F = { .data = "tests/bf/test.easy" };
STR $5 = { .data = "-" };
STR $6 = { .data = "." };
STR $7 = { .data = "[" };
STR $8 = { .data = "]" };
STR $9 = { .data = "+++++++++++[>+++++>+++<<-]>++++++++++++++ .---- .++++++++++++++++++ .++++++ .>." };
STR bf(STR program)
{
    Tape tape = {0};
    STR out = {0};
    int pc = 0;
    int ptr = 0;
    STR ch = {0};
    int depth = 0;
    $index(0, 0, 299, &$F, 16, 14);
    tape.data[(0) - (0)] = 0;
    ptr = 0;
    out = $0;
    pc = 0;
    while (1)
    {
        const int $r3 = LENGTH(program);
        const int $r2 = ($r3 - 1);
        if (!(pc <= $r2)) break;
        const STR $r4 = SUBSTR(program, pc, 1);
        ch = $r4;
        if (strcmp(ch.data, $1.data) == 0)
        {
            const int $r5 = (ptr + 1);
            ptr = $r5;
        }
        if (strcmp(ch.data, $2.data) == 0)
        {
            const int $r6 = (ptr - 1);
            ptr = $r6;
        }
        if (strcmp(ch.data, $3.data) == 0)
        {
            $index(ptr, 0, 299, &$F, 26, 33);
            const int $r7 = (tape.data[(ptr) - (0)] + 1);
            tape.data[(ptr) - (0)] = $r7;
        }
        if (strcmp(ch.data, $5.data) == 0)
        {
            $index(ptr, 0, 299, &$F, 27, 33);
            const int $r8 = (tape.data[(ptr) - (0)] - 1);
            tape.data[(ptr) - (0)] = $r8;
        }
        if (strcmp(ch.data, $6.data) == 0)
        {
            const STR $r10 = CHARACTER(tape.data[(ptr) - (0)]);
            const STR $r9 = $concat("AA", out, $r10);
            out = $r9;
        }
        if (strcmp(ch.data, $7.data) == 0)
        {
            const int $r11 = (tape.data[(ptr) - (0)] == 0);
            if ($r11)
            {
                depth = 1;
                while (1)
                {
                    const int $r12 = (depth > 0);
                    if (!($r12)) break;
                    const int $r13 = (pc + 1);
                    pc = $r13;
                    const STR $r14 = SUBSTR(program, pc, 1);
                    ch = $r14;
                    if (strcmp(ch.data, $7.data) == 0)
                    {
                        const int $r15 = (depth + 1);
                        depth = $r15;
                    }
                    if (strcmp(ch.data, $8.data) == 0)
                    {
                        const int $r16 = (depth - 1);
                        depth = $r16;
                    }
                    depth += 0;
                }
            }
        }
        if (strcmp(ch.data, $8.data) == 0)
        {
            const int $r17 = (tape.data[(ptr) - (0)] != 0);
            if ($r17)
            {
                depth = 1;
                while (1)
                {
                    const int $r18 = (depth > 0);
                    if (!($r18)) break;
                    const int $r19 = (pc - 1);
                    pc = $r19;
                    const STR $r20 = SUBSTR(program, pc, 1);
                    ch = $r20;
                    if (strcmp(ch.data, $8.data) == 0)
                    {
                        const int $r21 = (depth + 1);
                        depth = $r21;
                    }
                    if (strcmp(ch.data, $7.data) == 0)
                    {
                        const int $r22 = (depth - 1);
                        depth = $r22;
                    }
                    depth += 0;
                }
            }
        }
        pc += 1;
    }
    return out;
}
int main()
{
    program = $9;
    const STR $r1 = bf(program);
    $output("A", $r1);
}
