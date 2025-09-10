import json
import sys
from pathlib import Path

from peg.parser import PEGParser

grammar = (Path(__file__).parent / "easy.peg").read_text

parser = PEGParser(grammar, start="compilation")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: peg_cli.py <input.easy>")
        sys.exit(1)

    code = Path(sys.argv[1]).read_text()
    v = parser.parse(code)

    print(json.dumps(v, indent=2))
