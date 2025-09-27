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
        const int $r5 = strcmp(ch.data, $1.data) == 0;
        if ($r5)
        {
            const int $r6 = (ptr + 1);
            ptr = $r6;
        }
        const int $r7 = strcmp(ch.data, $2.data) == 0;
        if ($r7)
        {
            const int $r8 = (ptr - 1);
            ptr = $r8;
        }
        const int $r9 = strcmp(ch.data, $3.data) == 0;
        if ($r9)
        {
            $index(ptr, 0, 299, &$F, 26, 33);
            const int $r10 = (tape.data[(ptr) - (0)] + 1);
            tape.data[(ptr) - (0)] = $r10;
        }
        const int $r11 = strcmp(ch.data, $5.data) == 0;
        if ($r11)
        {
            $index(ptr, 0, 299, &$F, 27, 33);
            const int $r12 = (tape.data[(ptr) - (0)] - 1);
            tape.data[(ptr) - (0)] = $r12;
        }
        const int $r13 = strcmp(ch.data, $6.data) == 0;
        if ($r13)
        {
            const STR $r15 = CHARACTER(tape.data[(ptr) - (0)]);
            const STR $r14 = $concat("AA", out, $r15);
            out = $r14;
        }
        const int $r16 = strcmp(ch.data, $7.data) == 0;
        if ($r16)
        {
            const int $r17 = (tape.data[(ptr) - (0)] == 0);
            if ($r17)
            {
                depth = 1;
                while (1)
                {
                    const int $r18 = (depth > 0);
                    if (!($r18)) break;
                    const int $r19 = (pc + 1);
                    pc = $r19;
                    const STR $r20 = SUBSTR(program, pc, 1);
                    ch = $r20;
                    const int $r21 = strcmp(ch.data, $7.data) == 0;
                    if ($r21)
                    {
                        const int $r22 = (depth + 1);
                        depth = $r22;
                    }
                    const int $r23 = strcmp(ch.data, $8.data) == 0;
                    if ($r23)
                    {
                        const int $r24 = (depth - 1);
                        depth = $r24;
                    }
                    depth += 0;
                }
            }
        }
        const int $r25 = strcmp(ch.data, $8.data) == 0;
        if ($r25)
        {
            const int $r26 = (tape.data[(ptr) - (0)] != 0);
            if ($r26)
            {
                depth = 1;
                while (1)
                {
                    const int $r27 = (depth > 0);
                    if (!($r27)) break;
                    const int $r28 = (pc - 1);
                    pc = $r28;
                    const STR $r29 = SUBSTR(program, pc, 1);
                    ch = $r29;
                    const int $r30 = strcmp(ch.data, $8.data) == 0;
                    if ($r30)
                    {
                        const int $r31 = (depth + 1);
                        depth = $r31;
                    }
                    const int $r32 = strcmp(ch.data, $7.data) == 0;
                    if ($r32)
                    {
                        const int $r33 = (depth - 1);
                        depth = $r33;
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
