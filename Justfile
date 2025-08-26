test:
    python easy.py

run:
    python easy.py tests/sieve/test.easy && cc -o sieve sieve.c && ./sieve
