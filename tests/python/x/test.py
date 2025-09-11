from dataclasses import make_dataclass

from runtime import runtime_print

r = make_dataclass("r_STRUCTURE", [("x", int), ("y", int)])(0, 0)
a = [[0 for _ in range(1+80+1)] for _ in range(1+25+1)]
i = 0
a = 0
s = ""
f = [[0 for _ in range(1+80+1)] for _ in range(1+25+1)]
a = 123
i = 1
while True:
    if i > 100:
        break
    if not (i <= 10):
        break
    x = 0
    x = (a + i)
    runtime_print(x)
    i += 3
s = 'abc'
runtime_print(s + '<->' + 'xyz')
f[10][20] = 99
exit(0)
