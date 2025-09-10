from pathlib import Path

from peg.parser import PEGParser
from yamler import yamlizer

grammar = (Path(__file__).parent / "easy.peg").read_text()

p = PEGParser(grammar, start="compilation")

code = """
PROGRAM Test:
  OUTPUT CHARACTER (48);
END PROGRAM Test;
"""

print(yamlizer(p.parse(code)))
