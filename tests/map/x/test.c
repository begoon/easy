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
        const int $r25 = (node_number < 1);
        if ($r25)
        {
            $output("A", $0);
            $exit();
        }
        else
        {
            struct
            {
                struct
                {
                    int data[49 - 1 + 1];
                } *data;
            } adjacency = {0};
            OWN(adjacency, manage_array, $r26);
            const int $r27 = 1;
            adjacency.data = make_array($r27, node_number, sizeof(struct
            {
                int data[49 - 1 + 1];
            }), NULL);
            struct
            {
                Node *data;
            } nodes = {0};
            OWN(nodes, manage_array, $r28);
            const int $r29 = 1;
            nodes.data = make_array($r29, node_number, sizeof(Node), $manage0);
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
                        const size_t $r30 = $array_index(nodes.data, i, &$F, 29, 19);
                        scanf("%d", &nodes.data[$r30].id);
                        const size_t $r31 = $array_index(nodes.data, i, &$F, 29, 32);
                        scanf("%d", &nodes.data[$r31].neighbour_number);
                        const size_t $r32 = $array_index(nodes.data, i, &$F, 29, 59);
                        {
                            char buf[4096];
                            scanf("%4095s", buf);
                            manage_string(&nodes.data[$r32].name, -1);
                            nodes.data[$r32].name = make_string(buf, strlen(buf));
                        }
                        j = 1;
                        while (1)
                        {
                            const size_t $r33 = $array_index(nodes.data, i, &$F, 31, 27);
                            if (!(j <= nodes.data[$r33].neighbour_number)) break;
                            const size_t $r34 = $array_index(nodes.data, i, &$F, 32, 21);
                            $index(j, 1, 10, &$F, 32, 35);
                            scanf("%d", &nodes.data[$r34].neighbours.data[(size_t)((int64_t)(j) - (1))]);
                            const size_t $r35 = $array_index(nodes.data, i, &$F, 33, 29);
                            const size_t $r36 = $array_index(adjacency.data, nodes.data[$r35].id, &$F, 33, 23);
                            const size_t $r37 = $array_index(nodes.data, i, &$F, 33, 42);
                            $index(j, 1, 10, &$F, 33, 56);
                            $index(nodes.data[$r37].neighbours.data[(size_t)((int64_t)(j) - (1))], 1, 49, &$F, 33, 36);
                            adjacency.data[$r36].data[(size_t)((int64_t)(nodes.data[$r37].neighbours.data[(size_t)((int64_t)(j) - (1))]) - (1))] = TRUE;
                            j += 1;
                        }
                        {
                            const STR $r38 = CHARACTER(13);
                            OWN($r38, manage_string, $r39);
                            $output("A", $r38);
                        }
                        i += 1;
                    }
                }
                scanf("%d", &i);
                const int $r40 = (i != 0);
                if ($r40)
                {
                    $output("A", $3);
                    $exit();
                }
                $output("A", $4);
                $output("");
                {
                    const STR $r41 = CHARACTER(0);
                    OWN($r41, manage_string, $r42);
                    $output("AA", $5, $r41);
                }
                {
                    i = 1;
                    while (1)
                    {
                        if (!(i <= node_number)) break;
                        int j = 0;
                        const int $r43 = (i % 10);
                        j = $r43;
                        {
                            const STR $r44 = CHARACTER(0);
                            OWN($r44, manage_string, $r45);
                            $output("iA", j, $r44);
                        }
                        i += 1;
                    }
                }
                $output("");
                {
                    const STR $r46 = CHARACTER(0);
                    OWN($r46, manage_string, $r47);
                    $output("AA", $5, $r46);
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
                        const int $r48 = (i < 10);
                        if ($r48)
                        {
                            $output("A", $7);
                        }
                        {
                            const STR $r49 = CHARACTER(0);
                            OWN($r49, manage_string, $r50);
                            $output("iAA", i, $8, $r49);
                        }
                        j = 1;
                        while (1)
                        {
                            if (!(j <= node_number)) break;
                            const size_t $r52 = $array_index(adjacency.data, i, &$F, 61, 22);
                            $index(j, 1, 49, &$F, 61, 25);
                            const int $r51 = (adjacency.data[$r52].data[(size_t)((int64_t)(j) - (1))] == TRUE);
                            if ($r51)
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
                            const STR $r53 = CHARACTER(10);
                            OWN($r53, manage_string, $r54);
                            $output("A", $r53);
                        }
                        i += 1;
                    }
                }
            }
            {
                struct
                {
                    int *data;
                } colors = {0};
                OWN(colors, manage_array, $r55);
                const int $r56 = 1;
                colors.data = make_array($r56, node_number, sizeof(int), NULL);
                struct
                {
                    int *data;
                } available = {0};
                OWN(available, manage_array, $r57);
                const int $r58 = 1;
                available.data = make_array($r58, node_number, sizeof(int), NULL);
                int max_color = 0;
                int i = 0;
                i = 1;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    const size_t $r59 = $array_index(colors.data, i, &$F, 73, 45);
                    const int $r60 = (-1);
                    colors.data[$r59] = $r60;
                    i += 1;
                }
                const size_t $r61 = $array_index(colors.data, 1, &$F, 74, 16);
                colors.data[$r61] = 1;
                max_color = 1;
                i = 1;
                while (1)
                {
                    if (!(i <= node_number)) break;
                    const size_t $r62 = $array_index(available.data, i, &$F, 77, 48);
                    available.data[$r62] = TRUE;
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
                        const size_t $r64 = $array_index(adjacency.data, i, &$F, 84, 22);
                        $index(j, 1, 49, &$F, 84, 25);
                        const int $r63 = (adjacency.data[$r64].data[(size_t)((int64_t)(j) - (1))] == TRUE);
                        if ($r63)
                        {
                            const size_t $r66 = $array_index(colors.data, j, &$F, 85, 21);
                            const int $r67 = (-1);
                            const int $r65 = (colors.data[$r66] != $r67);
                            if ($r65)
                            {
                                const size_t $r68 = $array_index(colors.data, j, &$F, 86, 34);
                                const size_t $r69 = $array_index(available.data, colors.data[$r68], &$F, 86, 27);
                                available.data[$r69] = FALSE;
                            }
                        }
                        j += 1;
                    }
                    found = FALSE;
                    j = 1;
                    while (1)
                    {
                        const int $r70 = (!found);
                        if (!($r70 && j <= node_number)) break;
                        const size_t $r72 = $array_index(available.data, j, &$F, 93, 24);
                        const int $r71 = (available.data[$r72] == TRUE);
                        if ($r71)
                        {
                            const size_t $r73 = $array_index(colors.data, i, &$F, 94, 24);
                            colors.data[$r73] = j;
                            const int $r74 = (j > max_color);
                            if ($r74)
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
                        const size_t $r75 = $array_index(available.data, j, &$F, 100, 50);
                        available.data[$r75] = TRUE;
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
                                const STR $r76 = CHARACTER(0);
                                OWN($r76, manage_string, $r77);
                                $output("AiAA", $11, color, $12, $r76);
                            }
                            i = 1;
                            while (1)
                            {
                                if (!(i <= node_number)) break;
                                const size_t $r79 = $array_index(colors.data, i, &$F, 110, 21);
                                const int $r78 = (colors.data[$r79] == color);
                                if ($r78)
                                {
                                    const size_t $r80 = $array_index(nodes.data, i, &$F, 111, 26);
                                    const size_t $r81 = $array_index(nodes.data, i, &$F, 111, 26);
                                    $output("iA", nodes.data[$r81].id, $7);
                                }
                                i += 1;
                            }
                            {
                                const STR $r82 = CHARACTER(10);
                                OWN($r82, manage_string, $r83);
                                $output("A", $r82);
                            }
                            color += 1;
                        }
                    }
                }
            }
        }
    }
}
