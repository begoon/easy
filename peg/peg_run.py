import json

from peg.peg_parser import PEGParser

grammar = open("peg/easy.peg").read()

p = PEGParser(grammar, start="compilation")

code = """
PROGRAM Test:
    SET a := 2.;
  EXIT;
END PROGRAM Test;
"""

print(json.dumps(p.parse(code), indent=2))
