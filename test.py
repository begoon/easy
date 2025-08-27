import os
import subprocess
import sys
from pathlib import Path

TESTS_FOLDER = Path("tests")


def run(cmd, *args, **kwargs) -> subprocess.CompletedProcess:
    code: subprocess.CompletedProcess = subprocess.run(cmd, *args, **kwargs)
    if code.returncode != 0:
        v = " ".join([str(v) for v in cmd]) if isinstance(cmd, (list, tuple)) else str(cmd)
        print("ERROR:", v)
        exit(1)
    return code


def run_tests(name: str | None) -> None:
    for test in TESTS_FOLDER.iterdir():
        if test.is_dir() and (name is None or name in test.name):
            process(test)


def process(test: Path) -> None:
    test /= "test"
    print(test.parent.name)

    program = test.with_suffix(".easy")

    flags = []

    expected_tokens = test.with_suffix(".expected.tokens")
    if expected_tokens.exists():
        flags.append("-t")

    expected_ast = test.with_suffix(".expected.ast")
    if expected_ast.exists():
        flags.append("-a")

    expected_s = test.with_suffix(".expected.s")
    if expected_s.exists():
        flags.append("-s")

    if verbose:
        print(">", ["python", "easy.py", program, *flags])

    flags.extend(["-o", str(test.with_suffix(".c"))])

    if run(["python", "easy.py", program, *flags]).returncode != 0:
        exit(1)

    if "-t" in flags:
        if run(["diff", "-u", expected_tokens, test.with_suffix(".tokens")]).returncode != 0:
            exit(1)
    if "-a" in flags:
        if run(["diff", "-u", expected_ast, test.with_suffix(".ast")]).returncode != 0:
            exit(1)

    if "-s" in flags:
        if run(["diff", "-u", expected_s, test.with_suffix(".s")]).returncode != 0:
            exit(1)

    expected_c = test.with_suffix(".expected.c")
    c = test.with_suffix(".c")
    if expected_c.exists() and run(["diff", "-u", expected_c, c]).returncode != 0:
        exit(1)

    expected_output = test.with_suffix(".expected.output")
    if expected_output.exists():

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


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    name = arg(sys.argv, "--filter")
    run_tests(name)
