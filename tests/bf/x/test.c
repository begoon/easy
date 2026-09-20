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
STR $F = { .data = "tests/bf/test.easy", .sz = 18 };
STR $1 = { .data = "", .sz = 0 };
STR $2 = { .data = ">", .sz = 1 };
STR $3 = { .data = "<", .sz = 1 };
STR $4 = { .data = "+", .sz = 1 };
STR $5 = { .data = "-", .sz = 1 };
STR $6 = { .data = ".", .sz = 1 };
STR $7 = { .data = "[", .sz = 1 };
STR $8 = { .data = "]", .sz = 1 };
STR $9 = { .data = "+++++++++++[>+++++>+++<<-]>++++++++++++++ .---- .++++++++++++++++++ .++++++ .>.", .sz = 79 };
STR bf(STR program)
{
    manage_string(&program, 1);
    OWN(program, manage_string, $r5);
    Tape tape = {0};
    STR out = {0};
    OWN(out, manage_string, $r6);
    int pc = 0;
    int ptr = 0;
    STR ch = {0};
    OWN(ch, manage_string, $r7);
    int depth = 0;
    $index(0, 0, 299, &$F, 16, 14);
    tape.data[(0) - (0)] = 0;
    ptr = 0;
    __auto_type $r8 = $1;
    manage_string(&$r8, 1);
    manage_string(&out, -1);
    out = $r8;
    {
        pc = 0;
        while (1)
        {
            const int $r10 = LENGTH(program);
            const int $r9 = ($r10 - 1);
            if (!(pc <= $r9)) break;
            {
                const STR $r11 = SUBSTR(program, pc, 1);
                OWN($r11, manage_string, $r12);
                __auto_type $r13 = $r11;
                manage_string(&$r13, 1);
                manage_string(&ch, -1);
                ch = $r13;
            }
            const int $r14 = ch.sz == $2.sz && (ch.sz ? memcmp(ch.data, $2.data, ch.sz) : 0) == 0;
            if ($r14)
            {
                const int $r15 = (ptr + 1);
                ptr = $r15;
            }
            const int $r16 = ch.sz == $3.sz && (ch.sz ? memcmp(ch.data, $3.data, ch.sz) : 0) == 0;
            if ($r16)
            {
                const int $r17 = (ptr - 1);
                ptr = $r17;
            }
            const int $r18 = ch.sz == $4.sz && (ch.sz ? memcmp(ch.data, $4.data, ch.sz) : 0) == 0;
            if ($r18)
            {
                $index(ptr, 0, 299, &$F, 26, 33);
                $index(ptr, 0, 299, &$F, 26, 46);
                const int $r19 = (tape.data[(ptr) - (0)] + 1);
                tape.data[(ptr) - (0)] = $r19;
            }
            const int $r20 = ch.sz == $5.sz && (ch.sz ? memcmp(ch.data, $5.data, ch.sz) : 0) == 0;
            if ($r20)
            {
                $index(ptr, 0, 299, &$F, 27, 33);
                $index(ptr, 0, 299, &$F, 27, 46);
                const int $r21 = (tape.data[(ptr) - (0)] - 1);
                tape.data[(ptr) - (0)] = $r21;
            }
            {
                const int $r22 = ch.sz == $6.sz && (ch.sz ? memcmp(ch.data, $6.data, ch.sz) : 0) == 0;
                if ($r22)
                {
                    {
                        $index(ptr, 0, 299, &$F, 28, 57);
                        const STR $r24 = CHARACTER(tape.data[(ptr) - (0)]);
                        OWN($r24, manage_string, $r25);
                        const STR $r23 = $concat("AA", out, $r24);
                        OWN($r23, manage_string, $r26);
                        __auto_type $r27 = $r23;
                        manage_string(&$r27, 1);
                        manage_string(&out, -1);
                        out = $r27;
                    }
                }
            }
            {
                const int $r28 = ch.sz == $7.sz && (ch.sz ? memcmp(ch.data, $7.data, ch.sz) : 0) == 0;
                if ($r28)
                {
                    {
                        $index(ptr, 0, 299, &$F, 30, 17);
                        const int $r29 = (tape.data[(ptr) - (0)] == 0);
                        if ($r29)
                        {
                            {
                                depth = 1;
                                while (1)
                                {
                                    const int $r30 = (depth > 0);
                                    if (!($r30)) break;
                                    const int $r31 = (pc + 1);
                                    pc = $r31;
                                    {
                                        const STR $r32 = SUBSTR(program, pc, 1);
                                        OWN($r32, manage_string, $r33);
                                        __auto_type $r34 = $r32;
                                        manage_string(&$r34, 1);
                                        manage_string(&ch, -1);
                                        ch = $r34;
                                    }
                                    const int $r35 = ch.sz == $7.sz && (ch.sz ? memcmp(ch.data, $7.data, ch.sz) : 0) == 0;
                                    if ($r35)
                                    {
                                        const int $r36 = (depth + 1);
                                        depth = $r36;
                                    }
                                    const int $r37 = ch.sz == $8.sz && (ch.sz ? memcmp(ch.data, $8.data, ch.sz) : 0) == 0;
                                    if ($r37)
                                    {
                                        const int $r38 = (depth - 1);
                                        depth = $r38;
                                    }
                                    depth += 0;
                                }
                            }
                        }
                    }
                }
            }
            {
                const int $r39 = ch.sz == $8.sz && (ch.sz ? memcmp(ch.data, $8.data, ch.sz) : 0) == 0;
                if ($r39)
                {
                    {
                        $index(ptr, 0, 299, &$F, 40, 17);
                        const int $r40 = (tape.data[(ptr) - (0)] != 0);
                        if ($r40)
                        {
                            {
                                depth = 1;
                                while (1)
                                {
                                    const int $r41 = (depth > 0);
                                    if (!($r41)) break;
                                    const int $r42 = (pc - 1);
                                    pc = $r42;
                                    {
                                        const STR $r43 = SUBSTR(program, pc, 1);
                                        OWN($r43, manage_string, $r44);
                                        __auto_type $r45 = $r43;
                                        manage_string(&$r45, 1);
                                        manage_string(&ch, -1);
                                        ch = $r45;
                                    }
                                    const int $r46 = ch.sz == $8.sz && (ch.sz ? memcmp(ch.data, $8.data, ch.sz) : 0) == 0;
                                    if ($r46)
                                    {
                                        const int $r47 = (depth + 1);
                                        depth = $r47;
                                    }
                                    const int $r48 = ch.sz == $7.sz && (ch.sz ? memcmp(ch.data, $7.data, ch.sz) : 0) == 0;
                                    if ($r48)
                                    {
                                        const int $r49 = (depth - 1);
                                        depth = $r49;
                                    }
                                    depth += 0;
                                }
                            }
                        }
                    }
                }
            }
            pc += 1;
        }
    }
    manage_string(&out, 1);
    return out;
}
int main_program()
{
    OWN(program, manage_string, $r1);
    __auto_type $r2 = $9;
    manage_string(&$r2, 1);
    manage_string(&program, -1);
    program = $r2;
    {
        const STR $r3 = bf(program);
        OWN($r3, manage_string, $r4);
        $output("A", $r3);
    }
}
