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

typedef struct string_list
{
    struct string_list *next;
    STR *str;
} string_list;

static string_list *strings = NULL;

void print_string(STR s, const char *prefix)
{
    char *buf = alloca(s.sz + 1);
    memcpy(buf, s.data, s.sz);
    buf[s.sz] = '\0';
    printf("%s: '%s'\n", prefix, buf);
}

STR make_string(const char *data, int sz)
{
    void *ptr = malloc(sz);
    if (!ptr)
    {
        fprintf(stderr, "make_string|data: out of memory\n");
        exit(1);
    }
    STR *str = malloc(sizeof(STR));
    if (!str)
    {
        fprintf(stderr, "make_string|str: out of memory\n");
        exit(1);
    }
    *str = (STR){.sz = sz, .data = ptr, .immutable = 0};
    memcpy(str->data, data, sz);

    string_list *reference = malloc(sizeof(string_list));
    if (!reference)
    {
        fprintf(stderr, "make_string|list: out of memory\n");
        exit(1);
    }
    reference->str = str;
    reference->next = strings;
    strings = reference;
    return *str;
}

void free_strings()
{
    string_list *list = strings;
    while (list)
    {
        string_list *next = list->next;
        if (list->str->data && !list->str->immutable)
        {
            memset(list->str->data, 0, list->str->sz);
            free(list->str->data);
            list->str->data = NULL;

            memset(list->str, 0, sizeof(STR));
            free(list->str);
            list->str = NULL;
        }
        free(list);
        list = next;
    }
    strings = NULL;
}

typedef struct
{
    void *data;
} ARRAY;

void free_array(void *ptr)
{
    ARRAY *array = (ARRAY *)ptr;
    if (array->data)
    {
        free(array->data);
        array->data = NULL;
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
            memcpy(buf + sz, arg->data, n);
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = (int)sizeof(buf) - sz < arg.sz ? (int)sizeof(buf) - sz : arg.sz;
            memcpy(buf + sz, arg.data, n);
        }
        sz += n;
    }
    va_end(args);

    STR v = make_string(buf, sz);
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
    free_strings();
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
