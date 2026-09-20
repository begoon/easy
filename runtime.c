#if defined(__linux__)
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <alloca.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void exit(int status);
void *calloc(size_t count, size_t size);
void free(void *ptr);

#define TRUE 1
#define FALSE 0

typedef void (*Manage)(const void *, int);

typedef struct Guard {
    struct Guard *previous;
    const void *value;
    Manage manage;
} Guard;

static Guard *guards;
static size_t live_strings;
static size_t live_arrays;

static inline void guard_init(Guard *guard, const void *value, Manage manage)
{
    *guard = (Guard){guards, value, manage};
    guards = guard;
}

static inline void guard_cleanup(Guard *guard)
{
    assert(guards == guard);
    guards = guard->previous;
    guard->manage(guard->value, -1);
}

#define OWN(value, manage, name) \
    Guard name __attribute__((cleanup(guard_cleanup))); \
    guard_init(&name, &(value), manage)

static void *allocate(size_t size)
{
    void *p = calloc(1, size);
    if (!p) { fprintf(stderr, "out of memory\n"); exit(1); }
    return p;
}

static inline void $sleep(double seconds)
{
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - (double)ts.tv_sec) * 1e9);
    if (ts.tv_nsec < 0)
    {
        ts.tv_nsec = 0;
    }
    nanosleep(&ts, NULL);
}

typedef struct {
    size_t refs;
    char bytes[];
} StringStorage;

typedef struct
{
    int sz;
    const char *data;
    StringStorage *owner;
} STR;

static inline void manage_string(const void *value, int change)
{
    STR s;
    memcpy(&s, value, sizeof(s));
    if (!s.owner) return;
    if (change > 0) ++s.owner->refs;
    else if (--s.owner->refs == 0) {
        --live_strings;
        free(s.owner);
    }
}

typedef struct
{
    const STR *filename;
    int line;
    int character;
} L;

STR make_string(const char *data, int sz)
{
    if (sz == 0) return (STR){0};
    assert(sz > 0);
    StringStorage *owner = allocate(sizeof(*owner) + (size_t)sz);
    owner->refs = 1;
    memcpy(owner->bytes, data, (size_t)sz);
    ++live_strings;
    return (STR){.sz = sz, .data = owner->bytes, .owner = owner};
}

// Dynamic arrays retain their shared backing allocation. The header records
// the element count independently of the variables used in the Easy bounds.
typedef struct {
    size_t refs, count, element_size;
    Manage element;
    max_align_t alignment;
    unsigned char data[];
} ArrayStorage;

static inline void *make_array(size_t count, size_t element_size, Manage element)
{
    if (count > (SIZE_MAX - sizeof(ArrayStorage)) / element_size) {
        fprintf(stderr, "array allocation too large\n"); exit(1);
    }
    ArrayStorage *owner = allocate(sizeof(*owner) + count * element_size);
    owner->refs = 1;
    owner->count = count;
    owner->element_size = element_size;
    owner->element = element;
    ++live_arrays;
    return owner->data;
}

static inline void manage_array(const void *value, int change)
{
    void *data;
    memcpy(&data, value, sizeof(data));
    if (!data) return;
    ArrayStorage *owner = (ArrayStorage *)((char *)data - offsetof(ArrayStorage, data));
    if (change > 0) ++owner->refs;
    else if (--owner->refs == 0) {
        if (owner->element)
            for (size_t i = 0; i < owner->count; ++i)
                owner->element(owner->data + i * owner->element_size, -1);
        --live_arrays;
        free(owner);
    }
}

int FIX(double v)
{
    return (int)v;
}
double FLOAT(int v) { return (double)v; }
double FLOOR(double v) { return (double)((int)v); }
int LENGTH(STR s) { return s.sz; }

STR CHARACTER(int c)
{
    char cbuf = (char)c;
    STR s = make_string(&cbuf, 1);
    return s;
}

STR SUBSTR(STR from, int start, int length)
{
    // start cannot be < 0
    if (start < 0)
    {
        char *buf = alloca(from.sz + 1);
        memcpy(buf, from.data, from.sz);
        buf[from.sz] = '\0';
        fprintf(stderr, "substr (%s): start < 0 (%d)\n", buf, start);
        exit(1);
    }
    // length cannot be < 0
    if (length < 0)
    {
        char *buf = alloca(from.sz + 1);
        memcpy(buf, from.data, from.sz);
        buf[from.sz] = '\0';
        fprintf(stderr, "substr (%s): length < 0 (%d)\n", buf, length);
        exit(1);
    }
    // start + length cannot be > from.sz
    if (start + length > from.sz)
    {
        char *buf = alloca(from.sz + 1);
        memcpy(buf, from.data, from.sz);
        buf[from.sz] = '\0';
        fprintf(stderr, "substr (%s): start + length > size (%d + %d = %d > %d)\n", buf, start, length, start + length, from.sz);
        exit(1);
    }
    if (length == 0) return (STR){0};
    STR v = make_string(from.data + start, length);
    return v;
}

// i - INTEGER (int)
// r - REAL (double)
// s - STRING (char *)
// b - BOOLEAN (int, 0 or 1)
// S - STRING (STR *)
// A - STRING (STR)
STR $concat(const char *fmt, ...)
{
    char buf[4096];

    va_list args;
    va_start(args, fmt);

    int sz = 0;
    for (; *fmt != '\0'; fmt += 1)
    {
        int n = 0;
        if (*fmt == 'i')
            n = snprintf(buf + sz, sizeof(buf) - sz, "%d", va_arg(args, int));
        else if (*fmt == 'r')
            n = snprintf(buf + sz, sizeof(buf) - sz, "%f", va_arg(args, double));
        else if (*fmt == 'b')
            n = snprintf(buf + sz, sizeof(buf) - sz, "%s", va_arg(args, int) ? "TRUE" : "FALSE");
        else if (*fmt == 's')
            n = snprintf(buf + sz, sizeof(buf) - sz, "%s", va_arg(args, const char *));
        else if (*fmt == 'S')
        {
            const STR *arg = va_arg(args, STR *);
            n = (int)sizeof(buf) - sz < arg->sz ? (int)sizeof(buf) - sz : arg->sz;
            if (n) memcpy(buf + sz, arg->data, n);
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = (int)sizeof(buf) - sz < arg.sz ? (int)sizeof(buf) - sz : arg.sz;
            if (n) memcpy(buf + sz, arg.data, n);
        }
        sz += n;
    }
    va_end(args);

    STR v = make_string(buf, sz);
    return v;
}

void $output(const char *fmt, ...)
{
    if (fmt[0] == '\0')
    {
        putchar('\n');
        return;
    }

    va_list args;
    va_start(args, fmt);

    int zero = 0;

    int n = 0;
    for (; *fmt != '\0'; fmt += 1)
    {
        if (*fmt == 'i')
            n = printf("%d", va_arg(args, int));
        else if (*fmt == 'r')
            n = printf("%f", va_arg(args, double));
        else if (*fmt == 'b')
            n = printf("%s", va_arg(args, int) ? "TRUE" : "FALSE");
        else if (*fmt == 's')
            n = printf("%s", va_arg(args, const char *));
        else if (*fmt == 'S')
        {
            const STR *arg = va_arg(args, STR *);
            n = arg->sz;

            zero = n == 1 && arg->data[0] == '\0' ? 1 : 0;

            if (!zero)
            {
                char *const buf = alloca(n + 1);
                if (n) memcpy(buf, arg->data, n);
                buf[n] = '\0';
                n = printf("%s", buf);
            }
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = arg.sz;

            zero = n == 1 && arg.data[0] == '\0' ? 1 : 0;

            if (!zero)
            {
                char *const buf = alloca(n + 1);
                if (n) memcpy(buf, arg.data, n);
                buf[n] = '\0';
                n = printf("%s", buf);
            }
        }
    }
    va_end(args);

    if (!zero && (n > 1 || fmt[-1] == 'i'))
        putchar('\n');
}

void runtime_pause(double seconds)
{
    $sleep(seconds);
}

void $index(int index, int lo, int hi, const STR *filename, int line, int character)
{
    if (index < lo || index > hi)
    {
        fprintf(stderr, "array index out of bounds: %d not in [%d..%d] (%s:%d:%d)\n", index, lo, hi, filename->data, line, character);
        exit(1);
    }
}

int main_program();

void $exit()
{
    while (guards) guard_cleanup(guards);
#ifdef EASY_CHECK_OWNERSHIP
    assert(live_strings == 0);
    assert(live_arrays == 0);
#endif
    exit(0);
}

int main()
{
    main_program();
    $exit();
}

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wreturn-type"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
