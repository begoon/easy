#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int data[1295 - 0 + 1];
} Universe;
typedef struct
{
    int in_place;
    int by_value;
} Comparison;
Universe universe = {0};
Universe candidates = {0};
int candidate_index = 0;
int in_place = 0;
int by_value = 0;
int tries = 0;
STR $F = { .data = "tests/mastermind/test.easy", .sz = 26, .immutable = 1 };
STR $1 = { .data = "  ", .sz = 2, .immutable = 1 };
STR $2 = { .data = "maybe ", .sz = 6, .immutable = 1 };
STR $3 = { .data = " ? ", .sz = 3, .immutable = 1 };
STR $4 = { .data = "#", .sz = 1, .immutable = 1 };
STR $5 = { .data = ", enter feedback, <in-place> and <by-value>: (e.g. '1 2' for 1 in place and 2 by value):", .sz = 88, .immutable = 1 };
STR $6 = { .data = " ", .sz = 1, .immutable = 1 };
STR $7 = { .data = "guesses in ", .sz = 11, .immutable = 1 };
STR $8 = { .data = " tries", .sz = 6, .immutable = 1 };
STR $9 = { .data = " candidates eliminated, ", .sz = 24, .immutable = 1 };
STR $10 = { .data = " remaining", .sz = 10, .immutable = 1 };
STR $11 = { .data = "no candidates remaining, it is impossible!", .sz = 42, .immutable = 1 };
Universe create_universe()
{
    Universe universe = {0};
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    i = 0;
    while (1)
    {
        if (!(i <= 5)) break;
        j = 0;
        while (1)
        {
            if (!(j <= 5)) break;
            k = 0;
            while (1)
            {
                if (!(k <= 5)) break;
                l = 0;
                while (1)
                {
                    if (!(l <= 5)) break;
                    int v = 0;
                    int offset = 0;
                    const int $r32 = (i * 1000);
                    const int $r33 = (j * 100);
                    const int $r31 = ($r32 + $r33);
                    const int $r34 = (k * 10);
                    const int $r30 = ($r31 + $r34);
                    const int $r29 = ($r30 + l);
                    const int $r28 = ($r29 + 1111);
                    v = $r28;
                    const int $r40 = (i * 6);
                    const int $r39 = ($r40 * 6);
                    const int $r41 = (j * 6);
                    const int $r38 = ($r39 + $r41);
                    const int $r37 = ($r38 + k);
                    const int $r36 = ($r37 * 6);
                    const int $r35 = ($r36 + l);
                    offset = $r35;
                    $index(offset, 0, 1295, &$F, 27, 24);
                    universe.data[(offset) - (0)] = v;
                    l += 1;
                }
                k += 1;
            }
            j += 1;
        }
        i += 1;
    }
    return universe;
}
Comparison compare(int probe, int code)
{
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;
    int p4 = 0;
    int c1 = 0;
    int c2 = 0;
    int c3 = 0;
    int c4 = 0;
    Comparison comparison = {0};
    const int $r43 = (probe / 1000);
    const int $r42 = ($r43 % 10);
    p1 = $r42;
    const int $r45 = (probe / 100);
    const int $r44 = ($r45 % 10);
    p2 = $r44;
    const int $r47 = (probe / 10);
    const int $r46 = ($r47 % 10);
    p3 = $r46;
    const int $r48 = (probe % 10);
    p4 = $r48;
    const int $r50 = (code / 1000);
    const int $r49 = ($r50 % 10);
    c1 = $r49;
    const int $r52 = (code / 100);
    const int $r51 = ($r52 % 10);
    c2 = $r51;
    const int $r54 = (code / 10);
    const int $r53 = ($r54 % 10);
    c3 = $r53;
    const int $r55 = (code % 10);
    c4 = $r55;
    comparison.in_place = 0;
    comparison.by_value = 0;
    const int $r56 = (p1 == c1);
    if ($r56)
    {
        const int $r57 = (comparison.in_place + 1);
        comparison.in_place = $r57;
        const int $r58 = (-1);
        c1 = $r58;
    }
    const int $r59 = (p2 == c2);
    if ($r59)
    {
        const int $r60 = (comparison.in_place + 1);
        comparison.in_place = $r60;
        const int $r61 = (-1);
        c2 = $r61;
    }
    const int $r62 = (p3 == c3);
    if ($r62)
    {
        const int $r63 = (comparison.in_place + 1);
        comparison.in_place = $r63;
        const int $r64 = (-1);
        c3 = $r64;
    }
    const int $r65 = (p4 == c4);
    if ($r65)
    {
        const int $r66 = (comparison.in_place + 1);
        comparison.in_place = $r66;
        const int $r67 = (-1);
        c4 = $r67;
    }
    const int $r68 = (p1 == c2);
    if ($r68)
    {
        const int $r69 = (comparison.by_value + 1);
        comparison.by_value = $r69;
        const int $r70 = (-1);
        c2 = $r70;
    }
    else
    {
        const int $r71 = (p1 == c3);
        if ($r71)
        {
            const int $r72 = (comparison.by_value + 1);
            comparison.by_value = $r72;
            const int $r73 = (-1);
            c3 = $r73;
        }
        else
        {
            const int $r74 = (p1 == c4);
            if ($r74)
            {
                const int $r75 = (comparison.by_value + 1);
                comparison.by_value = $r75;
                const int $r76 = (-1);
                c4 = $r76;
            }
        }
    }
    const int $r77 = (p2 == c1);
    if ($r77)
    {
        const int $r78 = (comparison.by_value + 1);
        comparison.by_value = $r78;
        const int $r79 = (-1);
        c1 = $r79;
    }
    else
    {
        const int $r80 = (p2 == c3);
        if ($r80)
        {
            const int $r81 = (comparison.by_value + 1);
            comparison.by_value = $r81;
            const int $r82 = (-1);
            c3 = $r82;
        }
        else
        {
            const int $r83 = (p2 == c4);
            if ($r83)
            {
                const int $r84 = (comparison.by_value + 1);
                comparison.by_value = $r84;
                const int $r85 = (-1);
                c4 = $r85;
            }
        }
    }
    const int $r86 = (p3 == c1);
    if ($r86)
    {
        const int $r87 = (comparison.by_value + 1);
        comparison.by_value = $r87;
        const int $r88 = (-1);
        c1 = $r88;
    }
    else
    {
        const int $r89 = (p3 == c2);
        if ($r89)
        {
            const int $r90 = (comparison.by_value + 1);
            comparison.by_value = $r90;
            const int $r91 = (-1);
            c2 = $r91;
        }
        else
        {
            const int $r92 = (p3 == c4);
            if ($r92)
            {
                const int $r93 = (comparison.by_value + 1);
                comparison.by_value = $r93;
                const int $r94 = (-1);
                c4 = $r94;
            }
        }
    }
    const int $r95 = (p4 == c1);
    if ($r95)
    {
        const int $r96 = (comparison.by_value + 1);
        comparison.by_value = $r96;
        const int $r97 = (-1);
        c1 = $r97;
    }
    else
    {
        const int $r98 = (p4 == c2);
        if ($r98)
        {
            const int $r99 = (comparison.by_value + 1);
            comparison.by_value = $r99;
            const int $r100 = (-1);
            c2 = $r100;
        }
        else
        {
            const int $r101 = (p4 == c3);
            if ($r101)
            {
                const int $r102 = (comparison.by_value + 1);
                comparison.by_value = $r102;
                const int $r103 = (-1);
                c3 = $r103;
            }
        }
    }
    return comparison;
}
int main_program()
{
    const Universe $r1 = create_universe();
    universe = $r1;
    const Universe $r2 = create_universe();
    candidates = $r2;
    tries = 1;
    while (1)
    {
        if (!(TRUE)) break;
        int eliminated_candidates = 0;
        int remaining_candidates = 0;
        int probe_index = 0;
        int best_probe = 0;
        int min_rank = 0;
        probe_index = 0;
        while (1)
        {
            if (!(probe_index <= 1295)) break;
            int probe = 0;
            int probe_in_candidates = 0;
            struct
            {
                int data[44 - 0 + 1];
            } buckets = {0};
            int rank = 0;
            int worst_evaluation = 0;
            $index(probe_index, 0, 1295, &$F, 141, 27);
            probe = universe.data[(probe_index) - (0)];
            probe_in_candidates = 0;
            const int $r3 = (-1);
            worst_evaluation = $r3;
            candidate_index = 0;
            while (1)
            {
                if (!(candidate_index <= 1295)) break;
                int candidate = 0;
                $index(candidate_index, 0, 1295, &$F, 148, 35);
                candidate = candidates.data[(candidate_index) - (0)];
                const int $r4 = (probe == candidate);
                if ($r4)
                {
                    probe_in_candidates = 1;
                }
                const int $r5 = (candidate != 0);
                if ($r5)
                {
                    Comparison cmp = {0};
                    int bucket_index = 0;
                    const Comparison $r6 = compare(probe, candidate);
                    cmp = $r6;
                    const int $r8 = (cmp.in_place * 10);
                    const int $r7 = ($r8 + cmp.by_value);
                    bucket_index = $r7;
                    $index(bucket_index, 0, 44, &$F, 159, 21);
                    $index(bucket_index, 0, 44, &$F, 159, 46);
                    const int $r9 = (buckets.data[(bucket_index) - (0)] + 1);
                    buckets.data[(bucket_index) - (0)] = $r9;
                    $index(bucket_index, 0, 44, &$F, 161, 20);
                    const int $r10 = (buckets.data[(bucket_index) - (0)] > worst_evaluation);
                    if ($r10)
                    {
                        $index(bucket_index, 0, 44, &$F, 161, 90);
                        worst_evaluation = buckets.data[(bucket_index) - (0)];
                    }
                }
                candidate_index += 1;
            }
            const int $r14 = (1 - probe_in_candidates);
            const int $r13 = ($r14 * 10000);
            const int $r12 = (probe + $r13);
            const int $r15 = (worst_evaluation * 100000);
            const int $r11 = ($r12 + $r15);
            rank = $r11;
            const int $r17 = (min_rank == 0);
            const int $r18 = (rank < min_rank);
            const int $r16 = ($r17 || $r18);
            if ($r16)
            {
                min_rank = rank;
                best_probe = probe;
            }
            probe_index += 1;
        }
        $output("A", $1);
        $output("AiA", $2, best_probe, $3);
        $output("AiAA", $4, tries, $5, $6);
        scanf("%d", &in_place);
        scanf("%d", &by_value);
        const int $r19 = (in_place == 4);
        if ($r19)
        {
            $output("A", $1);
            $output("A", $1);
            $output("AiA", $7, tries, $8);
            exit(0);
        }
        eliminated_candidates = 0;
        remaining_candidates = 0;
        $output("A", $1);
        candidate_index = 0;
        while (1)
        {
            if (!(candidate_index <= 1295)) break;
            int candidate = 0;
            Comparison comparison = {0};
            $index(candidate_index, 0, 1295, &$F, 195, 33);
            candidate = candidates.data[(candidate_index) - (0)];
            const int $r20 = (candidate != 0);
            if ($r20)
            {
                const Comparison $r21 = compare(best_probe, candidate);
                comparison = $r21;
                const int $r23 = (comparison.in_place != in_place);
                const int $r24 = (comparison.by_value != by_value);
                const int $r22 = ($r23 || $r24);
                if ($r22)
                {
                    $index(candidate_index, 0, 1295, &$F, 199, 24);
                    candidates.data[(candidate_index) - (0)] = 0;
                    const int $r25 = (eliminated_candidates + 1);
                    eliminated_candidates = $r25;
                }
                else
                {
                    const int $r26 = (remaining_candidates + 1);
                    remaining_candidates = $r26;
                    $output("iA", candidate, $6);
                }
            }
            candidate_index += 1;
        }
        $output("A", $1);
        $output("iAiA", eliminated_candidates, $9, remaining_candidates, $10);
        const int $r27 = (remaining_candidates == 0);
        if ($r27)
        {
            $output("A", $11);
            exit(0);
        }
        tries += 1;
    }
}
