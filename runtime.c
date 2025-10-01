#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <alloca.h>
#include <unistd.h>

void exit(int status);
void *malloc(size_t size);
int usleep(useconds_t usec);

#define TRUE 1
#define FALSE 0

typedef struct
{
    int sz;
    int immutable;
    char data[4096];
} STR;

typedef struct
{
    const STR *filename;
    int line;
    int character;
} Location;

int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
double FLOOR(double v) { return (double)((int)v); }
int LENGTH(STR s) { return s.sz; }

STR CHARACTER(int c)
{
    STR v = {.sz = 1, .data = {c, 0}};
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
    STR v = {.sz = length};
    memcpy(v.data, from.data + start, length);
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
    STR v = {.sz = sz};
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
    usleep(seconds * 1e6);
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

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wreturn-type"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
