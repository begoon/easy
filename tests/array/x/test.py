from dataclasses import make_dataclass

from runtime import runtime_print

b = [make_dataclass("b_STRUCTURE", [("a", list), ("x", str)])([make_dataclass("a_STRUCTURE", [("x", int), ("y", float), ("f", bool)])(0, 0.0, False) for _ in range(0+80+1)], "") for _ in range(0+25+1)]
b[1].a[2] = b[2].a[1]
runtime_print('...')
exit(0)
