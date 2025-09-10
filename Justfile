default: test-compiler

test-unit:
    uv run pytest

test-compiler:
    uv run python test.py

life:
    python easy.py life.easy && clang life.c -o life && ./life

peg-cli:
    python -m peg.cli tests/array/test.easy

peg:
    python -m peg.run
    
clean:
    git clean -Xf
