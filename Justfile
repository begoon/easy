test:
    python easy.py

run:
    python easy.py compile seive.easy >seive.c && cc -o seive seive.c && ./seive