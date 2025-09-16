default: test-compiler

test-unit:
    uv run pytest

test-compiler:
    uv run python test.py

one NAME:
    python3 easy.py tests/{{NAME}}/test.easy && cc tests/{{NAME}}/test.c -o tests/{{NAME}}/test.exe -I . && ./tests/{{NAME}}/test.exe

life:
    python easy.py life.easy && clang life.c -o life && ./life

peg-cli:
    python -m peg.cli tests/array/test.easy

peg:
    python -m peg.run
    
clean:
    git clean -Xf
