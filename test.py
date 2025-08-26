import os
import sys
from pathlib import Path
from subprocess import run

TESTS_FOLDER = Path("tests")


def run_tests() -> None:
    for test in TESTS_FOLDER.iterdir():
        if test.is_dir():
            process(test)


def process(test: Path) -> None:
    test /= "test"
    print(test.parent.name)

    program = test.with_suffix(".easy")

    if run(["python", "easy.py", program, "-r"]).returncode != 0:
        exit(1)

    expected_c = test.with_suffix(".expected.c")
    c = test.with_suffix(".c")
    if run(["diff", "-u", expected_c, c]).returncode != 0:
        exit(1)

    expected_output = test.with_suffix(".expected.output")
    if expected_output.exists():
        if run(["python", "easy.py", program]).returncode != 0:
            exit(1)

        exe = test.with_suffix(".exe")
        if run(["cc", c, "-o", exe]).returncode != 0:
            exit(1)

        cmd = [exe, ">" + str(test.with_suffix(".output"))]
        if test.with_suffix(".input").exists():
            cmd.insert(1, "<" + str(test.with_suffix(".input")))

        executor = " ".join(map(str, cmd))
        if verbose:
            print(executor)

        if run(executor, shell=True).returncode != 0:
            exit(1)

        if run(["diff", "-u", expected_output, test.with_suffix(".output")]).returncode != 0:
            exit(1)


verbose = "-v" in sys.argv or os.getenv("DEBUG")

if __name__ == "__main__":
    run_tests()
