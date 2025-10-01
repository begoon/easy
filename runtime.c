#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <alloca.h>
#include <stdlib.h>
#include <unistd.h>
#define TRUE 1
#define FALSE 0
typedef struct
{
    int sz;
    int immutable;
    char *data;
} STR;
int debug_memory = 0;
typedef struct
{
    const STR *filename;
    int line;
    int character;
} Location;

#define AUTOFREE __attribute__((cleanup(STR_free)))

struct
{
    void *ptr;
    size_t size;
} allocations[1024];

#define MAX_ALLOCATION (sizeof(allocations) / sizeof(allocations[0]))

void add_allocation(void *p, size_t size)
{
    for (size_t i = 0; i < MAX_ALLOCATION; i++)
    {
        if (allocations[i].ptr == NULL)
        {
            allocations[i].ptr = p;
            allocations[i].size = size;
            return;
        }
    }
    fprintf(stderr, "internal error: out of allocation slots\n");
    exit(1);
}

size_t remove_allocation(void *ptr)
{
    for (size_t i = 0; i < MAX_ALLOCATION; i++)
    {
        if (allocations[i].ptr == ptr)
        {
            allocations[i].ptr = NULL;
            size_t size = allocations[i].size;
            allocations[i].size = 0;
            return size;
        }
    }
    fprintf(stderr, "internal error: pointer %p not found in allocations\n", ptr);
    exit(1);
}

void print_allocations()
{
    size_t active = 0;
    for (size_t i = 0; i < MAX_ALLOCATION; i++)
    {
        if (allocations[i].ptr != NULL)
            active += 1;
    }

    if (active == 0)
        return;

    fprintf(stderr, "memory leak: %zu active allocations\n", active);

    for (size_t i = 0; i < MAX_ALLOCATION; i++)
    {
        if (allocations[i].ptr != NULL)
        {
            fprintf(stderr, "\t%p (%zu bytes)\n", allocations[i].ptr, allocations[i].size);
        }
    }
}

void *allocate(int size)
{
    void *ptr = malloc((size_t)size);
    if (ptr == NULL)
    {
        fprintf(stderr, "out of memory: malloc(%d) failed\n", size);
        exit(1);
    }
    add_allocation(ptr, size);
    memset(ptr, 0, size);
    if (debug_memory)
        fprintf(stderr, "malloc(%d, %p)\n", size, ptr);
    return ptr;
}

void deallocate(void *ptr)
{
    const size_t size = remove_allocation(ptr);
    if (debug_memory)
        fprintf(stderr, "free(%p)\n", ptr);
    memset(ptr, 0, size);
    free(ptr);
}

void exit_check_allocations()
{
    print_allocations();
}

void STR_free(const STR *v)
{
    if (v->immutable || v->data == NULL)
        return;
    if (debug_memory)
        fprintf(stderr, "STR_free: [%d:%s]\n", v->sz, v->data);
    deallocate(v->data);
}

void STR_copy(STR *to, STR from)
{
    if (debug_memory)
        fprintf(stderr, "STR_copy: from [%d:%s]\n", from.sz, from.data);

    if (to->sz >= from.sz)
    {
        memcpy(to->data, from.data, from.sz);
        to->sz = from.sz;
        return;
    }
    STR_free(to);

    *to = (STR){.sz = from.sz, .data = allocate(from.sz)};
    memcpy(to->data, from.data, from.sz);
}

int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
double FLOOR(double v) { return (double)((int)v); }
int LENGTH(STR s) { return s.sz; }

STR CHARACTER(int c)
{
    STR v = {.sz = 1, .data = allocate(1)};
    v.data[0] = (char)c;
    return v;
}

STR SUBSTR(STR from, int start, int length)
{
    // if (start < 0)
    // {
    //     char *buf = alloca(from.sz + 1);
    //     memcpy(buf, from.data, from.sz);
    //     buf[from.sz] = '\0';
    //     fprintf(stderr, "substr (%s): start < 0 (%d)\n", buf, start);
    //     exit(1);
    // }
    // if (length < 0)
    // {
    //     char *buf = alloca(from.sz + 1);
    //     memcpy(buf, from.data, from.sz);
    //     buf[from.sz] = '\0';
    //     fprintf(stderr, "substr (%s): length < 0 (%d)\n", buf, length);
    //     exit(1);
    // }
    // if (start + length - 1 > from.sz)
    // {
    //     char *buf = alloca(from.sz + 1);
    //     memcpy(buf, from.data, from.sz);
    //     buf[from.sz] = '\0';
    //     fprintf(stderr, "substr (%s): start + length > size (%d + %d > %d)\n", buf, start, length, from.sz);
    //     exit(1);
    // }
    // length = from.sz - start;
    // if (length < 0)
    // {
    //     char *buf = alloca(from.sz + 1);
    //     memcpy(buf, from.data, from.sz);
    //     buf[from.sz] = '\0';
    //     fprintf(stderr, "substr (%s): size - start < 0 (%d - %d < 0)\n", buf, from.sz, start);
    //     exit(1);
    // }
    STR v = {.sz = length, .data = allocate(length + 1)};
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
    STR v = {.sz = sz, .data = allocate(sz)};
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
            char *buf = alloca(n + 1);
            memcpy(buf, arg->data, n);
            buf[n] = '\0';
            n = printf("%s", buf);
        }
        else if (*fmt == 'A')
        {
            const STR arg = va_arg(args, STR);
            n = arg.sz;
            char *buf = alloca(n + 1);
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
    debug_memory = getenv("DEBUG_MEMORY") != NULL;
    memset(allocations, 0, sizeof(allocations));
    main_program();
    exit_check_allocations();
    return 0;
}

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wreturn-type"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
