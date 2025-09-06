default: test-unit test-compiler

test-unit:
    uv run pytest

test-compiler:
    uv run python test.py

life:
    python easy.py life.easy && clang life.c -o life && ./life

peg-cli:
    python -m peg.peg_cli tests/array/test.easy

peg:
    python -m peg.peg_run
    
hw-c:
    python easy.py hw.easy && clang hw.c -o hw && ./hw

hw-py:
    python easy.py hw.easy -c hw.py && python hw.py

hw: hw-c hw-py

clean:
    git clean -Xf
