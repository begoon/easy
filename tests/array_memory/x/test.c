#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
typedef struct
{
    STR *data;
} Row;
typedef struct
{
    Row data[1 - 0 + 1];
} Matrix;
static void $manage0(const void *value, int change)
{
    typedef struct
    {
        struct
        {
            STR *data;
        } data[1 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        manage_array((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
static void $manage1(const void *value, int change)
{
    typedef struct
    {
        struct
        {
            Row *data;
        } rows;
    } T;
    manage_array((const char *)value + offsetof(T, rows), change);
}
static void $manage2(const void *value, int change)
{
    typedef struct
    {
        Row data[1 - 0 + 1];
    } T;
    for (size_t i = 0; i < sizeof(((T *)0)->data) / sizeof(((T *)0)->data[0]); ++i)
        manage_array((const char *)value + offsetof(T, data) + i * sizeof(((T *)0)->data[0]), change);
}
int lo = 0;
int hi = 0;
int n = 0;
int i = 0;
int same = 0;
STR $F = { .data = "tests/array_memory/test.easy", .sz = 28 };
STR $1 = { .data = "D", .sz = 1 };
Matrix returned()
{
    Matrix result = {0};
    OWN(result, $manage2, $r162);
    for (size_t $r163 = 0; $r163 < sizeof(result.data) / sizeof(result.data[0]); ++$r163)
    {
        const int $r164 = 0;
        const int $r165 = (1 + 1);
        result.data[$r163].data = make_array($r164, $r165, sizeof(STR), manage_string);
    }
    {
        $index(1, 0, 1, &$F, 9, 16);
        const size_t $r166 = $array_index(result.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 9, 19);
        const STR $r167 = CHARACTER(82);
        OWN($r167, manage_string, $r168);
        __auto_type $r169 = $r167;
        manage_string(&$r169, 1);
        manage_string(&result.data[(size_t)((int64_t)(1) - (0))].data[$r166], -1);
        result.data[(size_t)((int64_t)(1) - (0))].data[$r166] = $r169;
    }
    $manage2(&result, 1);
    return result;
}
int main_program()
{
    const int $r44 = (-1);
    lo = $r44;
    hi = 1;
    {
        struct
        {
            int *data;
        } values = {0};
        OWN(values, manage_array, $r45);
        const int $r46 = lo;
        values.data = make_array($r46, hi, sizeof(int), NULL);
        const int $r47 = (-1);
        const size_t $r48 = $array_index(values.data, $r47, &$F, 17, 16);
        values.data[$r48] = 11;
        const size_t $r49 = $array_index(values.data, 1, &$F, 18, 16);
        values.data[$r49] = 22;
        const int $r50 = (-100);
        lo = $r50;
        hi = 100;
        const int $r51 = (-1);
        const size_t $r52 = $array_index(values.data, $r51, &$F, 21, 19);
        const int $r53 = (-1);
        const size_t $r54 = $array_index(values.data, $r53, &$F, 21, 19);
        $output("i", values.data[$r54]);
        const size_t $r55 = $array_index(values.data, 1, &$F, 22, 19);
        const size_t $r56 = $array_index(values.data, 1, &$F, 22, 19);
        $output("i", values.data[$r56]);
        lo = 0;
        hi = 0;
        const int $r57 = (-1);
        const size_t $r58 = $array_index(values.data, $r57, &$F, 25, 19);
        const int $r59 = (-1);
        const size_t $r60 = $array_index(values.data, $r59, &$F, 25, 19);
        $output("i", values.data[$r60]);
        const size_t $r61 = $array_index(values.data, 1, &$F, 26, 19);
        const size_t $r62 = $array_index(values.data, 1, &$F, 26, 19);
        $output("i", values.data[$r62]);
    }
    n = 2;
    {
        i = 1;
        while (1)
        {
            if (!(i <= 100)) break;
            {
                struct
                {
                    struct
                    {
                        STR *data;
                    } data[1 - 0 + 1];
                } fixed = {0};
                OWN(fixed, $manage0, $r63);
                for (size_t $r64 = 0; $r64 < sizeof(fixed.data) / sizeof(fixed.data[0]); ++$r64)
                {
                    const int $r65 = 0;
                    fixed.data[$r64].data = make_array($r65, n, sizeof(STR), manage_string);
                }
                struct
                {
                    struct
                    {
                        STR *data;
                    } *data;
                } dynamic = {0};
                OWN(dynamic, manage_array, $r66);
                const int $r67 = 0;
                dynamic.data = make_array($r67, n, sizeof(struct
                {
                    STR *data;
                }), manage_array);
                for (size_t $r68 = 0; $r68 < array_storage(dynamic.data)->count; ++$r68)
                {
                    const int $r69 = 0;
                    dynamic.data[$r68].data = make_array($r69, n, sizeof(STR), manage_string);
                }
                struct
                {
                    struct
                    {
                        Row *data;
                    } rows;
                } record = {0};
                OWN(record, $manage1, $r70);
                const int $r71 = 0;
                record.rows.data = make_array($r71, n, sizeof(Row), manage_array);
                for (size_t $r72 = 0; $r72 < array_storage(record.rows.data)->count; ++$r72)
                {
                    const int $r73 = 0;
                    const int $r74 = (1 + 1);
                    record.rows.data[$r72].data = make_array($r73, $r74, sizeof(STR), manage_string);
                }
                Matrix a = {0};
                OWN(a, $manage2, $r75);
                for (size_t $r76 = 0; $r76 < sizeof(a.data) / sizeof(a.data[0]); ++$r76)
                {
                    const int $r77 = 0;
                    const int $r78 = (1 + 1);
                    a.data[$r76].data = make_array($r77, $r78, sizeof(STR), manage_string);
                }
                Matrix b = {0};
                OWN(b, $manage2, $r79);
                for (size_t $r80 = 0; $r80 < sizeof(b.data) / sizeof(b.data[0]); ++$r80)
                {
                    const int $r81 = 0;
                    const int $r82 = (1 + 1);
                    b.data[$r80].data = make_array($r81, $r82, sizeof(STR), manage_string);
                }
                {
                    $index(1, 0, 1, &$F, 36, 17);
                    const size_t $r83 = $array_index(fixed.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 36, 20);
                    const STR $r84 = CHARACTER(70);
                    OWN($r84, manage_string, $r85);
                    __auto_type $r86 = $r84;
                    manage_string(&$r86, 1);
                    manage_string(&fixed.data[(size_t)((int64_t)(1) - (0))].data[$r83], -1);
                    fixed.data[(size_t)((int64_t)(1) - (0))].data[$r83] = $r86;
                }
                {
                    const size_t $r87 = $array_index(dynamic.data, 2, &$F, 37, 19);
                    const size_t $r88 = $array_index(dynamic.data[$r87].data, 2, &$F, 37, 22);
                    const STR $r89 = CHARACTER(68);
                    OWN($r89, manage_string, $r90);
                    __auto_type $r91 = $r89;
                    manage_string(&$r91, 1);
                    manage_string(&dynamic.data[$r87].data[$r88], -1);
                    dynamic.data[$r87].data[$r88] = $r91;
                }
                {
                    const size_t $r92 = $array_index(record.rows.data, 2, &$F, 38, 23);
                    const size_t $r93 = $array_index(record.rows.data[$r92].data, 2, &$F, 38, 26);
                    const STR $r94 = CHARACTER(83);
                    OWN($r94, manage_string, $r95);
                    __auto_type $r96 = $r94;
                    manage_string(&$r96, 1);
                    manage_string(&record.rows.data[$r92].data[$r93], -1);
                    record.rows.data[$r92].data[$r93] = $r96;
                }
                {
                    const Matrix $r97 = returned();
                    OWN($r97, $manage2, $r98);
                    __auto_type $r99 = $r97;
                    $manage2(&$r99, 1);
                    $manage2(&a, -1);
                    a = $r99;
                }
                __auto_type $r100 = a;
                $manage2(&$r100, 1);
                $manage2(&b, -1);
                b = $r100;
                __auto_type $r101 = a;
                $manage2(&$r101, 1);
                $manage2(&a, -1);
                a = $r101;
                {
                    $index(1, 0, 1, &$F, 42, 13);
                    const size_t $r102 = $array_index(a.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 42, 16);
                    const STR $r103 = CHARACTER(88);
                    OWN($r103, manage_string, $r104);
                    __auto_type $r105 = $r103;
                    manage_string(&$r105, 1);
                    manage_string(&a.data[(size_t)((int64_t)(1) - (0))].data[$r102], -1);
                    a.data[(size_t)((int64_t)(1) - (0))].data[$r102] = $r105;
                }
                {
                    const int $r106 = (i == 100);
                    if ($r106)
                    {
                        {
                            $index(1, 0, 1, &$F, 44, 22);
                            const size_t $r108 = $array_index(fixed.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 44, 25);
                            $index(1, 0, 1, &$F, 44, 22);
                            const size_t $r109 = $array_index(fixed.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 44, 25);
                            const size_t $r110 = $array_index(dynamic.data, 2, &$F, 44, 39);
                            const size_t $r111 = $array_index(dynamic.data[$r110].data, 2, &$F, 44, 42);
                            const size_t $r112 = $array_index(dynamic.data, 2, &$F, 44, 39);
                            const size_t $r113 = $array_index(dynamic.data[$r112].data, 2, &$F, 44, 42);
                            const size_t $r114 = $array_index(record.rows.data, 2, &$F, 44, 60);
                            const size_t $r115 = $array_index(record.rows.data[$r114].data, 2, &$F, 44, 63);
                            const size_t $r116 = $array_index(record.rows.data, 2, &$F, 44, 60);
                            const size_t $r117 = $array_index(record.rows.data[$r116].data, 2, &$F, 44, 63);
                            $index(1, 0, 1, &$F, 44, 71);
                            const size_t $r118 = $array_index(b.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 44, 74);
                            $index(1, 0, 1, &$F, 44, 71);
                            const size_t $r119 = $array_index(b.data[(size_t)((int64_t)(1) - (0))].data, 2, &$F, 44, 74);
                            const STR $r107 = $concat("AAAA", fixed.data[(size_t)((int64_t)(1) - (0))].data[$r109], dynamic.data[$r112].data[$r113], record.rows.data[$r116].data[$r117], b.data[(size_t)((int64_t)(1) - (0))].data[$r119]);
                            OWN($r107, manage_string, $r120);
                            $output("A", $r107);
                        }
                        const size_t $r121 = $array_index(dynamic.data, 2, &$F, 45, 29);
                        const size_t $r122 = $array_index(dynamic.data[$r121].data, 2, &$F, 45, 32);
                        const int $r123 = dynamic.data[$r121].data[$r122].sz == $1.sz && (dynamic.data[$r121].data[$r122].sz ? memcmp(dynamic.data[$r121].data[$r122].data, $1.data, dynamic.data[$r121].data[$r122].sz) : 0) == 0;
                        same = $r123;
                        $output("b", same);
                    }
                }
            }
            i += 1;
        }
    }
    {
        struct
        {
            int *data;
        } low = {0};
        OWN(low, manage_array, $r124);
        const int $r126 = (-2147483647);
        const int $r125 = ($r126 - 1);
        const int $r127 = $r125;
        const int $r128 = (-2147483647);
        low.data = make_array($r127, $r128, sizeof(int), NULL);
        struct
        {
            int data[2147483647 - 2147483646 + 1];
        } high = {0};
        const int $r130 = (-2147483647);
        const int $r129 = ($r130 - 1);
        const size_t $r131 = $array_index(low.data, $r129, &$F, 54, 13);
        low.data[$r131] = 31;
        const int $r132 = (-2147483647);
        const size_t $r133 = $array_index(low.data, $r132, &$F, 55, 13);
        low.data[$r133] = 32;
        $index(2147483647, 2147483646, 2147483647, &$F, 56, 14);
        high.data[(size_t)((int64_t)(2147483647) - (2147483646))] = 33;
        const int $r135 = (-2147483647);
        const int $r134 = ($r135 - 1);
        const size_t $r136 = $array_index(low.data, $r134, &$F, 57, 13);
        low.data[$r136] = 31;
        while (1)
        {
            const int $r138 = (-2147483647);
            const int $r137 = ($r138 - 1);
            const size_t $r139 = $array_index(low.data, $r137, &$F, 57, 13);
            if (!(low.data[$r139] <= 31)) break;
            const int $r144 = (-2147483647);
            const int $r143 = ($r144 - 1);
            const size_t $r145 = $array_index(low.data, $r143, &$F, 58, 18);
            const int $r147 = (-2147483647);
            const int $r146 = ($r147 - 1);
            const size_t $r148 = $array_index(low.data, $r146, &$F, 58, 18);
            $output("i", low.data[$r148]);
            const int $r141 = (-2147483647);
            const int $r140 = ($r141 - 1);
            const size_t $r142 = $array_index(low.data, $r140, &$F, 57, 13);
            low.data[$r142] += 1;
        }
        const int $r150 = (-2147483647);
        const int $r149 = ($r150 - 1);
        const size_t $r151 = $array_index(low.data, $r149, &$F, 60, 13);
        low.data[$r151] = 31;
        const int $r153 = (-2147483647);
        const int $r152 = ($r153 - 1);
        const size_t $r154 = $array_index(low.data, $r152, &$F, 61, 16);
        const int $r156 = (-2147483647);
        const int $r155 = ($r156 - 1);
        const size_t $r157 = $array_index(low.data, $r155, &$F, 61, 16);
        $output("i", low.data[$r157]);
        const int $r158 = (-2147483647);
        const size_t $r159 = $array_index(low.data, $r158, &$F, 62, 16);
        const int $r160 = (-2147483647);
        const size_t $r161 = $array_index(low.data, $r160, &$F, 62, 16);
        $output("i", low.data[$r161]);
        $index(2147483647, 2147483646, 2147483647, &$F, 63, 17);
        $index(2147483647, 2147483646, 2147483647, &$F, 63, 17);
        $output("i", high.data[(size_t)((int64_t)(2147483647) - (2147483646))]);
    }
}
