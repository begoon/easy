import json
from pathlib import Path

from peg.peg_parser import PEGParser

grammar = (Path(__file__).parent / "easy.peg").read_text

p = PEGParser(grammar, start="compilation")

code = """
PROGRAM Test:
    SET a := 2.;
  EXIT;
END PROGRAM Test;
"""

print(json.dumps(p.parse(code), indent=2))
