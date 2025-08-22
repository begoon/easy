
#include <stdio.h>
#include <string.h>
extern void exit(int);
extern void* malloc(size_t);
#define TRUE 1
#define FALSE 0
int FIX(float v) { return (int)v; }
float FLOAT(int v) { return (float)v; }
char* str(int v) { char* s = malloc(12); sprintf(s, "%d", v); return s; }
char* concat(char* a, char* b) { char* s = malloc(strlen(a) + strlen(b) + 1); strcpy(s, a); strcat(s, b); return s; }
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

////////////////////////////////////////
float abs(float x) {
  if (x < 0) {
    return (-x);
  } else {
    return x;
  }
  return 0;
}
int integersqrt(int a) {
  if (a < 0) {
      printf("%s", "a < 0 in FUNCTION integersqrt.");
      printf("\n");
      exit(0);
  }
  if (a == 0) {
      return 0;
  }
  if (a > 0) {
      float x, ra;
      float epsilon;
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
////////////////////////////////////////
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
        printf("%s", concat(concat(concat("Prime[", str(count)), "] = "), str(i)));
        printf("\n");
      } 
    }
  } else {
    printf("%s", concat(concat("Input value ", str(topnum)), " non-positive."));
    printf("\n");
  }
  exit(0);
}
