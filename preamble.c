#include <stdio.h>
#include <string.h>
#include <stdarg.h>
extern void exit(int);
extern void *malloc(size_t);
#define TRUE 1
#define FALSE 0
int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
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
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
