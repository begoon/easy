default: test-unit test-compiler

test-unit:
    uv run pytest -vvv

test-compiler:
    uv run python test.py

life:
    python easy.py life.easy && clang life.c -o life && ./life

clean:
    git clean -Xf
