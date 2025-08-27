default: test-unit test-compiler

test-unit:
    uv run python easy.py

test-compiler:
    uv run python test.py

run:
    python easy.py tests/sieve/test.easy && cc -o sieve sieve.c && ./sieve

clean:
    git clean -Xf
