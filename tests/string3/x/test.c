#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR $0 = { .data = "abcXYZ", .sz = 6, .reference_count = 1 };
int main()
{
    {
        STR s = STR_new(0);
        STR v = STR_new(0);
        s = STR_copy(s, $0);
        STR_acquire(s);
        v = STR_copy(v, s);
        STR_acquire(v);
        STR_release(v);
        STR_release(s);
    }
}
