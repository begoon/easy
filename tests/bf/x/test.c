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
STR $F = { .data = "tests/bf/test.easy", .sz = 18, .immutable = 1 };
STR $1 = { .data = "", .sz = 0, .immutable = 1 };
STR $2 = { .data = ">", .sz = 1, .immutable = 1 };
STR $3 = { .data = "<", .sz = 1, .immutable = 1 };
STR $4 = { .data = "+", .sz = 1, .immutable = 1 };
STR $5 = { .data = "-", .sz = 1, .immutable = 1 };
STR $6 = { .data = ".", .sz = 1, .immutable = 1 };
STR $7 = { .data = "[", .sz = 1, .immutable = 1 };
STR $8 = { .data = "]", .sz = 1, .immutable = 1 };
STR $9 = { .data = "+++++++++++[>+++++>+++<<-]>++++++++++++++ .---- .++++++++++++++++++ .++++++ .>.", .sz = 79, .immutable = 1 };
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
    out = $1;
    pc = 0;
    while (1)
    {
        const int $r3 = LENGTH(program);
        const int $r2 = ($r3 - 1);
        if (!(pc <= $r2)) break;
        const STR $r4 = SUBSTR(program, pc, 1);
        ch = $r4;
        const int $r5 = ch.sz == $2.sz && memcmp(ch.data, $2.data, ch.sz) == 0;
        if ($r5)
        {
            const int $r6 = (ptr + 1);
            ptr = $r6;
        }
        const int $r7 = ch.sz == $3.sz && memcmp(ch.data, $3.data, ch.sz) == 0;
        if ($r7)
        {
            const int $r8 = (ptr - 1);
            ptr = $r8;
        }
        const int $r9 = ch.sz == $4.sz && memcmp(ch.data, $4.data, ch.sz) == 0;
        if ($r9)
        {
            $index(ptr, 0, 299, &$F, 26, 33);
            $index(ptr, 0, 299, &$F, 26, 46);
            const int $r10 = (tape.data[(ptr) - (0)] + 1);
            tape.data[(ptr) - (0)] = $r10;
        }
        const int $r11 = ch.sz == $5.sz && memcmp(ch.data, $5.data, ch.sz) == 0;
        if ($r11)
        {
            $index(ptr, 0, 299, &$F, 27, 33);
            $index(ptr, 0, 299, &$F, 27, 46);
            const int $r12 = (tape.data[(ptr) - (0)] - 1);
            tape.data[(ptr) - (0)] = $r12;
        }
        const int $r13 = ch.sz == $6.sz && memcmp(ch.data, $6.data, ch.sz) == 0;
        if ($r13)
        {
            $index(ptr, 0, 299, &$F, 28, 57);
            const STR $r15 = CHARACTER(tape.data[(ptr) - (0)]);
            const STR $r14 = $concat("AA", out, $r15);
            out = $r14;
        }
        const int $r16 = ch.sz == $7.sz && memcmp(ch.data, $7.data, ch.sz) == 0;
        if ($r16)
        {
            $index(ptr, 0, 299, &$F, 30, 17);
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
                    const int $r21 = ch.sz == $7.sz && memcmp(ch.data, $7.data, ch.sz) == 0;
                    if ($r21)
                    {
                        const int $r22 = (depth + 1);
                        depth = $r22;
                    }
                    const int $r23 = ch.sz == $8.sz && memcmp(ch.data, $8.data, ch.sz) == 0;
                    if ($r23)
                    {
                        const int $r24 = (depth - 1);
                        depth = $r24;
                    }
                    depth += 0;
                }
            }
        }
        const int $r25 = ch.sz == $8.sz && memcmp(ch.data, $8.data, ch.sz) == 0;
        if ($r25)
        {
            $index(ptr, 0, 299, &$F, 40, 17);
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
                    const int $r30 = ch.sz == $8.sz && memcmp(ch.data, $8.data, ch.sz) == 0;
                    if ($r30)
                    {
                        const int $r31 = (depth + 1);
                        depth = $r31;
                    }
                    const int $r32 = ch.sz == $7.sz && memcmp(ch.data, $7.data, ch.sz) == 0;
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
int main_program()
{
    program = $9;
    const STR $r1 = bf(program);
    $output("A", $r1);
}
