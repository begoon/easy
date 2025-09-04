import json

from peg.peg_parser import PEGParser

g = open("peg/easy.peg").read()

p = PEGParser(g, start="assignment_statement")

code = """
SET p.x.z := p.y;
"""
print(json.dumps(p.parse(code), indent=2))
