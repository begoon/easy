import itertools
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

    x = test.parent / "x" / "test"

    expected_tokens = x.with_suffix(".tokens")
    if expected_tokens.exists():
        flags.append("-t")

    expected_ast = x.with_suffix(".ast")
    if expected_ast.exists():
        flags.append("-a")

    expected_s = x.with_suffix(".s")
    if expected_s.exists():
        flags.append("-s")

    if verbose:
        print(">", ["python", "easy.py", program, *flags])

    flags.extend(["-o", str(test.with_suffix(".c"))])

    run(["python", "easy.py", program, *flags])

    if "-t" in flags:
        diff(expected_tokens, test.with_suffix(".tokens"))
    if "-a" in flags:
        diff(expected_ast, test.with_suffix(".ast"))

    if "-s" in flags:
        diff(expected_s, test.with_suffix(".s"))

    expected_c = x.with_suffix(".c")
    if expected_c.exists():
        diff(expected_c, test.with_suffix(".c"))

    expected_output = x.with_suffix(".output")
    if expected_output.exists():
        exe = test.with_suffix(".exe")
        cc_flags = ["-Wall", "-Wextra", "-Werror"]
        run(["clang", *cc_flags, test.with_suffix(".c"), "-o", exe])

        cmd = [exe, ">" + str(test.with_suffix(".output"))]
        input_file = x.with_suffix(".input")
        if input_file.exists():
            cmd.insert(1, "<" + str(input_file))

        executor = " ".join(map(str, cmd))
        if verbose:
            print(executor)

        run(executor, shell=True)

        diff(expected_output, test.with_suffix(".output"))


WHITE = "\033[97m"
RED = "\033[91m"
NC = "\033[0m"


def diff(expected_file: Path, created_file: Path) -> None:
    expected_lines = expected_file.read_text().splitlines()
    created_lines = created_file.read_text().splitlines()

    if expected_lines == created_lines:
        return

    if update:
        print(f"update {created_file}")
        expected_file.write_text("\n".join(created_lines) + "\n")
        return

    for i, (expected, created) in enumerate(itertools.zip_longest(expected_lines, created_lines), 1):
        if expected != created:
            print(f"{expected_file}:{i}:")
            print(f"  expected: {WHITE}{expected}{NC}")
            print(f"  created:  {RED}{created}{NC}")
            print(f"{created_file}:{i}:")
            exit(1)


verbose = "-v" in sys.argv or os.getenv("DEBUG")
update = "-u" in sys.argv


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    name = arg(sys.argv, "--filter")
    run_tests(name)
