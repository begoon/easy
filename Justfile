default: test-unit test-compiler

test-unit:
    uv run pytest -vvv

test-compiler:
    uv run python test.py

peg-cli:
    python -m peg.peg_cli tests/array/test.easy

life:
    python easy.py life.easy && clang life.c -o life && ./life

oneoff:
    python -m peg.peg_oneoff
    
clean:
    git clean -Xf
