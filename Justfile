default: test-unit test-compiler

test-unit:
    uv run python easy.py

test-compiler:
    uv run python test.py

life:
    python easy.py life.easy && clang life.c -o life && ./life

clean:
    git clean -Xf
