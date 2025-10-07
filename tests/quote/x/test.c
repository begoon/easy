#include "runtime.c"
typedef int INTEGER;
typedef double REAL;
typedef int BOOLEAN;
typedef STR STRING;
STR s = {0};
STR $0 = { .data = "\"", .sz = 2, .immutable = 1 };
STR $1 = { .data = "<1>", .sz = 3, .immutable = 1 };
STR $2 = { .data = "", .sz = 0, .immutable = 1 };
STR $3 = { .data = "<2>", .sz = 3, .immutable = 1 };
STR $4 = { .data = "<3>", .sz = 3, .immutable = 1 };
STR $5 = { .data = "<4>", .sz = 3, .immutable = 1 };
STR $6 = { .data = "...", .sz = 3, .immutable = 1 };
STR $7 = { .data = "we said \"ok\"", .sz = 14, .immutable = 1 };
STR $8 = { .data = "str = [", .sz = 7, .immutable = 1 };
STR $9 = { .data = "abc = [", .sz = 7, .immutable = 1 };
STR $F = { .data = "tests/quote/test.easy", .sz = 21, .immutable = 1 };
int main_program()
{
    s = $0;
    $output("A", s);
    $output("A", $1);
    $output("A", $2);
    $output("A", $3);
    $output("A", $0);
    $output("A", $4);
    $output("A", s);
    $output("A", $5);
    $output("A", $6);
    s = $0;
    s = $7;
    s = $8;
    s = $9;
    $exit();
}
