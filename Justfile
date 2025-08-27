default: test-unit test-compiler

test-unit:
    python easy.py

test-compiler:
    python test.py

run:
    python easy.py tests/sieve/test.easy && cc -o sieve sieve.c && ./sieve

clean:
    git clean -Xf
