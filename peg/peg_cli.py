import json
import sys
from pathlib import Path

from peg.peg_parser import PEGParser

grammar = open("peg/easy.peg").read()

parser = PEGParser(grammar, start="compilation")

code = Path(sys.argv[1]).read_text()
r = parser.parse(code)

print(json.dumps(r, indent=2))
