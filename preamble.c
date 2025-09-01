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
    char data[256];
} STR;
int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
int LENGTH(STR s) { return strlen(s.data); }
char *CHARACTER(int c)
{
    char *v = malloc(2);
    v[0] = (char)c;
    v[1] = '\0';
    return v;
}
char *SUBSTR(const char *str, int start, int length)
{
    char *sub = malloc(length + 1);
    if (!sub)
        return NULL;
    strncpy(sub, str + start, length);
    sub[length] = '\0';
    return sub;
}
char *str(int v)
{
    char *s = malloc(12);
    sprintf(s, "%d", v);
    return s;
}
char *concat(int count, ...)
{
    va_list args;
    va_start(args, count);

    size_t total_len = 0;
    for (int i = 0; i < count; i++)
    {
        char *s = va_arg(args, char *);
        total_len += strlen(s);
    }
    va_end(args);

    char *result = malloc(total_len + 1);
    if (!result)
        return NULL;

    result[0] = '\0';

    va_start(args, count);
    for (int i = 0; i < count; i++)
    {
        strcat(result, va_arg(args, char *));
    }
    va_end(args);

    return result;
}
void output(int count, ...)
{
    va_list args;
    va_start(args, count);

    if (count < 1)
        return;

    for (int i = 0; i < count - 1; i++)
    {
        printf("%s", va_arg(args, char *));
    }

    char *s = va_arg(args, char *);
    printf("%s", s);
    if (strlen(s) > 1)
        printf("%s", "\n");

    va_end(args);
}
void pause(double seconds)
{
    usleep(seconds * 1e6);
}
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-variable"
