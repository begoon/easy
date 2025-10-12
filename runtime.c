#if defined(__linux__)
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <alloca.h>
#include <time.h>

void exit(int status);
void *malloc(size_t size);
void free(void *ptr);

#define TRUE 1
#define FALSE 0

#define AUTOFREE_ARRAY __attribute__((cleanup(free_array)))
#define AUTOFREE_STRUCT __attribute__((cleanup(free_struct)))

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

typedef struct
{
    int sz;
    int immutable;
    char *data;
} STR;

typedef struct
{
    const STR *filename;
    int line;
    int character;
} L;

void STR_copy(STR *to, const STR *from)
{
    if (to->data && !to->immutable)
    {
        free(to->data);
    }
    to->sz = from->sz;
    to->immutable = from->immutable;
    if (from->sz > 0)
    {
        void *ptr = malloc(from->sz);
        if (!ptr)
        {
            fprintf(stderr, "STR_copy(): out of memory\n");
            exit(1);
        }
        memcpy(ptr, from->data, from->sz);
        to->data = ptr;
    }
    else
    {
        to->data = NULL;
    }
}

typedef struct STR_REFERENCE
{
    STR *str;
    struct STR_REFERENCE *next;
} STR_REFERENCE;

typedef struct STRINGS_OWNER
{
    STR_REFERENCE *strings;
} STRINGS_OWNER;

void STR_register(void *owner_, STR *str)
{
    STRINGS_OWNER *owner = (STRINGS_OWNER *)owner_;

    STR_REFERENCE *ref = malloc(sizeof(STR_REFERENCE));
    if (!ref)
    {
        fprintf(stderr, "STR_register(): out of memory\n");
        exit(1);
    }
    ref->str = str;
    ref->next = owner->strings;
    owner->strings = ref;
}

void STR_release_strings(STRINGS_OWNER *owner)
{
    printf("STR_release_strings: %p\n", owner);
    STR_REFERENCE *ref = owner->strings;
    while (ref)
    {
        STR_REFERENCE *next = ref->next;
        if (!ref->str->immutable && ref->str->data)
        {
            printf("freeing string: %p\n", ref->str);
            free(ref->str->data);
            ref->str->data = NULL;
        }
        free(ref);
        ref = next;
    }
    owner->strings = NULL;
}

typedef struct
{
    void *data;
} ARRAY;

void free_array(void *ptr)
{
    printf("free_array: %p\n", ptr);
    ARRAY *array = (ARRAY *)ptr;
    void *data = array->data;
    if (data)
    {
        free(data);
        array->data = NULL;
    }
}

void free_struct(void *ptr)
{
    printf("free_struct: %p\n", ptr);
    STRINGS_OWNER *owner = (STRINGS_OWNER *)ptr;
    STR_release_strings(owner);
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
    void *ptr = malloc(1);
    if (!ptr)
    {
        fprintf(stderr, "CHARACTER(): out of memory\n");
        exit(1);
    }
    STR v = {.sz = 1, .data = ptr};
    v.data[0] = (char)c;
    {
        char *buf = alloca(2);
        buf[0] = (char)c;
        buf[1] = '\0';
        printf("CHARACTER: %d -> '%s'\n", c, buf);
    }
    return v;
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
    void *ptr = malloc(length);
    if (!ptr)
    {
        fprintf(stderr, "SUBSTR(): out of memory\n");
        exit(1);
    }
    STR v = {.sz = length, .data = ptr};
    memcpy(v.data, from.data + start, length);
    {
        char *buf = alloca(length + 1);
        memcpy(buf, v.data, length);
        buf[length] = '\0';
        printf("SUBSTR: %d,%d -> '%s'\n", start, length, buf);
    }
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
            n = arg->sz;
            memcpy(buf + sz, arg->data, n);
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = arg.sz;
            memcpy(buf + sz, arg.data, n);
        }
        sz += n;
    }
    va_end(args);
    void *ptr = malloc(sz);
    if (!ptr)
    {
        fprintf(stderr, "$concat(): out of memory\n");
        exit(1);
    }
    STR v = {.sz = sz, .data = ptr};
    memcpy(v.data, buf, sz);
    return v;
}

void $output(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

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
            char *const buf = alloca(n + 1);
            memcpy(buf, arg->data, n);
            buf[n] = '\0';
            n = printf("%s", buf);
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = arg.sz;
            char *const buf = alloca(n + 1);
            memcpy(buf, arg.data, n);
            buf[n] = '\0';
            n = printf("%s", buf);
        }
    }
    va_end(args);

    if (n > 1 || fmt[-1] == 'i')
        putchar('\n');
}

void rt_pause(double seconds)
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

int main()
{
    main_program();
    return 0;
}

void $exit()
{
    exit(0);
}

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wreturn-type"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
