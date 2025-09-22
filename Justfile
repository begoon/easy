default: test-compiler

ci:
    just COMPILER=python
    just COMPILER=python-ext
    just COMPILER=ts

export COMPILER := "python"

quick:
    SKIP_RUN=1 just test-compiler

update:
    UPDATE=1 just test-compiler

test-unit:
    uv run pytest

test-compiler: test-compiler-python test-compiler-ts

test-compiler-python:
    uv run python test.py --compiler {{ COMPILER }}

test-compiler-ts:
    bun run test.ts --compiler {{ COMPILER }}

one NAME:
    python3 easy.py tests/{{NAME}}/test.easy && cc -std=c23 tests/{{NAME}}/test.c -o tests/{{NAME}}/test.exe -I . && ./tests/{{NAME}}/test.exe

life:
    python easy.py life.easy && clang life.c -o life && ./life

peg-cli:
    python -m peg.cli tests/array/test.easy

peg:
    python -m peg.run
    
clean:
    git clean -Xf
