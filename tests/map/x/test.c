#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    int id;
    int neighbour_number;
    STR name;
    struct
    {
        int data[10 - 1 + 1];
    } neighbours;
} Node;
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        int id;
        int neighbour_number;
        STR name;
        struct
        {
            int data[10 - 1 + 1];
        } neighbours;
    } T;
    manage_string((const char *)value + offsetof(T, name), change);
}
int node_number = 0;
STR $0 = { .data = "ERROR: number of nodes must be 1 or greater", .sz = 43 };
STR $1 = { .data = "NUMBER OF NODES ", .sz = 16 };
STR $F = { .data = "tests/map/test.easy", .sz = 19 };
STR $3 = { .data = "ERROR: expected 0 at the end of input", .sz = 37 };
STR $4 = { .data = "ADJACENCY MATRIX:", .sz = 17 };
STR $5 = { .data = "    ", .sz = 4 };
STR $6 = { .data = "-", .sz = 1 };
STR $7 = { .data = " ", .sz = 1 };
STR $8 = { .data = "| ", .sz = 2 };
STR $9 = { .data = "*", .sz = 1 };
STR $10 = { .data = "NUMBER OF COLORS NEEDED: ", .sz = 25 };
STR $11 = { .data = "COLOR ", .sz = 6 };
STR $12 = { .data = ", NODES: ", .sz = 9 };
int main_program()
{
    scanf("%d", &node_number);
    {
        const int $r1 = (node_number < 1);
        if ($r1)
        {
            $output("A", $0);
            $exit();
        }
        else
        {
            void *$r2 = make_array((node_number - 1 + 1), sizeof(struct
            {
                int data[49 - 1 + 1];
            }), NULL);
            struct
            {
                struct
                {
                    int data[49 - 1 + 1];
                } *data;
            } adjacency = { .data = $r2 };
            OWN(adjacency, manage_array, $r3);
            void *$r4 = make_array((node_number - 1 + 1), sizeof(Node), $manage0);
            struct
            {
                Node *data;
            } nodes = { .data = $r4 };
            OWN(nodes, manage_array, $r5);
            $output("Ai", $1, node_number);
            $output("");
            {
                int i = 0;
                {
                    i = 1;
                    while (1)
                    {
                        if (!(i <= node_number)) break;
                        int j = 0;
                        $index(i, 1, node_number, &$F, 29, 19);
                        scanf("%d", &nodes.data[(i) - (1)].id);
                        $index(i, 1, node_number, &$F, 29, 32);
                        scanf("%d", &nodes.data[(i) - (1)].neighbour_number);
                        $index(i, 1, node_number, &$F, 29, 59);
                        {
                            char buf[4096];
                            scanf("%4095s", buf);
                            manage_string(&nodes.data[(i) - (1)].name, -1);
                            nodes.data[(i) - (1)].name = make_string(buf, strlen(buf));
                        }
                        j = 1;
                        while (1)
                        {
                            $index(i, 1, node_number, &$F, 31, 27);
                            if (!(j <= nodes.data[(i) - (1)].neighbour_number)) break;
                            $index(i, 1, node_number, &$F, 32, 21);
                            $index(j, 1, 10, &$F, 32, 35);
                            scanf("%d", &nodes.data[(i) - (1)].neighbours.data[(j) - (1)]);
                            $index(i, 1, node_number, &$F, 33, 29);
                            $index(nodes.data[(i) - (1)].id, 1, node_number, &$F, 33, 23);
                            $index(i, 1, node_number, &$F, 33, 42);
                            $index(j, 1, 10, &$F, 33, 56);
                            $index(nodes.data[(i) - (1)].neighbours.data[(j) - (1)], 1, 49, &$F, 33, 36);
                            adjacency.data[(nodes.data[(i) - (1)].id) - (1)].data[(nodes.data[(i) - (1)].neighbours.data[(j) - (1)]) - (1)] = TRUE;
                            j += 1;
                        }
                        {
                            const STR $r6 = CHARACTER(13);
                            OWN($r6, manage_string, $r7);
                            $output("A", $r6);
                        }
                        i += 1;
                    }
                }
                scanf("%d", &i);
                const int $r8 = (i != 0);
                if ($r8)
                {
                    $output("A", $3);
                    $exit();
                }
                $output("A", $4);
                $output("");
                {
                    const STR $r9 = CHARACTER(0);
                    OWN($r9, manage_string, $r10);
                    $output("AA", $5, $r9);
                }
                {
                    i = 1;
                    while (1)
                    {
                        if (!(i <= node_number)) break;
                        int j = 0;
                        const int $r11 = (i % 10);
                        j = $r11;
                        {
                            const STR $r12 = CHARACTER(0);
                            OWN($r12, manage_string, $r13);
                            $output("iA", j, $r12);
                        }
                        i += 1;
                    }
                }
                $output("");
                {
                    const STR $r14 = CHARACTER(0);
                    OWN($r14, manage_string, $r15);
                    $output("AA", $5, $r14);
                }
                i = 1;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    $output("A", $6);
                    i += 1;
                }
                $output("");
                {
                    i = 1;
                    while (1)
                    {
                        if (!(i <= node_number)) break;
                        int j = 0;
                        const int $r16 = (i < 10);
                        if ($r16)
                        {
                            $output("A", $7);
                        }
                        {
                            const STR $r17 = CHARACTER(0);
                            OWN($r17, manage_string, $r18);
                            $output("iAA", i, $8, $r17);
                        }
                        j = 1;
                        while (1)
                        {
                            if (!(j <= node_number)) break;
                            $index(i, 1, node_number, &$F, 61, 22);
                            $index(j, 1, 49, &$F, 61, 25);
                            const int $r19 = (adjacency.data[(i) - (1)].data[(j) - (1)] == TRUE);
                            if ($r19)
                            {
                                $output("A", $9);
                            }
                            else
                            {
                                $output("A", $7);
                            }
                            j += 1;
                        }
                        {
                            const STR $r20 = CHARACTER(10);
                            OWN($r20, manage_string, $r21);
                            $output("A", $r20);
                        }
                        i += 1;
                    }
                }
            }
            {
                void *$r22 = make_array((node_number - 1 + 1), sizeof(int), NULL);
                struct
                {
                    int *data;
                } colors = { .data = $r22 };
                OWN(colors, manage_array, $r23);
                void *$r24 = make_array((node_number - 1 + 1), sizeof(int), NULL);
                struct
                {
                    int *data;
                } available = { .data = $r24 };
                OWN(available, manage_array, $r25);
                int max_color = 0;
                int i = 0;
                i = 1;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    $index(i, 1, node_number, &$F, 73, 45);
                    const int $r26 = (-1);
                    colors.data[(i) - (1)] = $r26;
                    i += 1;
                }
                $index(1, 1, node_number, &$F, 74, 16);
                colors.data[(1) - (1)] = 1;
                max_color = 1;
                i = 1;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    $index(i, 1, node_number, &$F, 77, 48);
                    available.data[(i) - (1)] = TRUE;
                    i += 1;
                }
                i = 2;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    int j = 0;
                    int found = 0;
                    j = 1;
                    while (1)
                    {
                        if (!(j <= node_number)) break;
                        $index(i, 1, node_number, &$F, 84, 22);
                        $index(j, 1, 49, &$F, 84, 25);
                        const int $r27 = (adjacency.data[(i) - (1)].data[(j) - (1)] == TRUE);
                        if ($r27)
                        {
                            $index(j, 1, node_number, &$F, 85, 21);
                            const int $r29 = (-1);
                            const int $r28 = (colors.data[(j) - (1)] != $r29);
                            if ($r28)
                            {
                                $index(j, 1, node_number, &$F, 86, 34);
                                $index(colors.data[(j) - (1)], 1, node_number, &$F, 86, 27);
                                available.data[(colors.data[(j) - (1)]) - (1)] = FALSE;
                            }
                        }
                        j += 1;
                    }
                    found = FALSE;
                    j = 1;
                    while (1)
                    {
                        const int $r30 = (!found);
                        if (!($r30 && j <= node_number)) break;
                        $index(j, 1, node_number, &$F, 93, 24);
                        const int $r31 = (available.data[(j) - (1)] == TRUE);
                        if ($r31)
                        {
                            $index(i, 1, node_number, &$F, 94, 24);
                            colors.data[(i) - (1)] = j;
                            const int $r32 = (j > max_color);
                            if ($r32)
                            {
                                max_color = j;
                            }
                            found = TRUE;
                        }
                        j += 1;
                    }
                    j = 1;
                    while (1)
                    {
                        if (!(j <= node_number)) break;
                        $index(j, 1, node_number, &$F, 100, 50);
                        available.data[(j) - (1)] = TRUE;
                        j += 1;
                    }
                    i += 1;
                }
                $output("");
                $output("Ai", $10, max_color);
                $output("");
                {
                    int color = 0;
                    int i = 0;
                    {
                        color = 1;
                        while (1)
                        {
                            if (!(color <= max_color)) break;
                            {
                                const STR $r33 = CHARACTER(0);
                                OWN($r33, manage_string, $r34);
                                $output("AiAA", $11, color, $12, $r33);
                            }
                            i = 1;
                            while (1)
                            {
                                if (!(i <= node_number)) break;
                                $index(i, 1, node_number, &$F, 110, 21);
                                const int $r35 = (colors.data[(i) - (1)] == color);
                                if ($r35)
                                {
                                    $index(i, 1, node_number, &$F, 111, 26);
                                    $index(i, 1, node_number, &$F, 111, 26);
                                    $output("iA", nodes.data[(i) - (1)].id, $7);
                                }
                                i += 1;
                            }
                            {
                                const STR $r36 = CHARACTER(10);
                                OWN($r36, manage_string, $r37);
                                $output("A", $r36);
                            }
                            color += 1;
                        }
                    }
                }
            }
        }
    }
}
