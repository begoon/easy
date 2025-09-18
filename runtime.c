#include <stdio.h>
#include <string.h>
#include <stdarg.h>
extern void exit(int);
extern void *malloc(size_t);
extern void usleep(int);
#define TRUE 1
#define FALSE 0
typedef struct
{
    char _;
    char data[1024];
} STR;
int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
int LENGTH(STR s) { return strlen(s.data); }
STR CHARACTER(int c)
{
    STR v = {0};
    v.data[0] = (char)c;
    v.data[1] = '\0';
    return v;
}
STR SUBSTR(STR str, int start, int length)
{
    STR v = {0};
    strncpy(v.data, str.data + start, length);
    v.data[length] = '\0';
    return v;
}
STR from_cstring(const char *s)
{
    STR v = {0};
    strncpy(v.data, s, sizeof(v.data) - 1);
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
    va_list args;
    va_start(args, fmt);

    STR r = {0};

    int sz = 0;
    for (; *fmt != '\0'; fmt += 1)
    {
        int n = 0;
        if (*fmt == 'i')
            n = sprintf(r.data + sz, "%d", va_arg(args, int));
        else if (*fmt == 'r')
            n = sprintf(r.data + sz, "%f", va_arg(args, double));
        else if (*fmt == 'b')
            n = sprintf(r.data + sz, "%s", va_arg(args, int) ? "TRUE" : "FALSE");
        else if (*fmt == 's')
            n = sprintf(r.data + sz, "%s", va_arg(args, const char *));
        else if (*fmt == 'S')
            n = sprintf(r.data + sz, "%s", (va_arg(args, STR *))->data);
        else if (*fmt == 'A')
            n = sprintf(r.data + sz, "%s", va_arg(args, STR).data);
        sz += n;
    }
    va_end(args);
    return r;
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
            n = printf("%s", va_arg(args, STR *)->data);
        else if (*fmt == 'A')
            n = printf("%s", va_arg(args, STR).data);
    }
    va_end(args);

    if (n > 1 || fmt[-1] == 'i')
        putchar('\n');
}
void $pause(double seconds)
{
    usleep(seconds * 1e6);
}
void *$ref(void *ptr, int index, int lo, int hi, int element_size, const char *location)
{
    if (index < lo || index > hi)
    {
        fprintf(stderr, "array index out of bounds: %d not in [%d..%d] (%s)\n", index, lo, hi, location);
        exit(1);
    }
    index -= lo;
    return (void *)((unsigned char *)ptr + index * element_size);
}
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wreturn-type"