#include <stdio.h>
#include <string.h>
#include <stdarg.h>
extern void exit(int);
extern void *malloc(size_t);
#define TRUE 1
#define FALSE 0
int FIX(double v) { return (int)v; }
double FLOAT(int v) { return (double)v; }
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
double abs(double x) {
  if (x < 0) {
    return (-x);
  } else {
    return x;
  }
  return 0;
}
int integersqrt(int a) {
  if (a < 0) {
      printf("%s\n", "a < 0 in FUNCTION integersqrt.");
      exit(0);
  }
  if (a == 0) {
      return 0;
  }
  if (a > 0) {
      double x, ra;
      double epsilon;
      int sqrt;
      ra = FLOAT(a);
      epsilon = (1e-07 * ra);
      for (x = (ra / 2.0); (abs((ra - (x * x))) > epsilon); x += (((ra / x) - x) / 2.0)) { 
        ; 
      }
      for (sqrt = (FIX(x) - 1); (((sqrt + 1) * (sqrt + 1)) <= a); sqrt += 1) { 
        ; 
      }
      return sqrt;
  }
  return 0;
}
int main() {
  int topnum;
  scanf("%d", &topnum);
  if (topnum > 0) {
    int sieve[topnum + 1];
    int i, limit, count;
    for (i = 1; i <= topnum; i += 1) { 
      sieve [i] = TRUE; 
    }
    limit = (integersqrt(topnum) + 1);
    for (i = 2; i <= limit; i += 1) { 
      if (sieve[i]) {
        int j;
        for (j = (2 * i); j <= topnum; j += i) { 
          sieve [j] = FALSE; 
        }
      } 
    }
    count = 0;
    for (i = 1; i <= topnum; i += 1) { 
      if (sieve[i]) {
        count = (count + 1);
        printf("%s\n", concat(4, "Prime[", str(count), "] = ", str(i)));
      } 
    }
  } else {
    printf("%s\n", concat(3, "Input value ", str(topnum), " non-positive."));
  }
  exit(0);
}
