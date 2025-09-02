import itertools
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

TESTS_FOLDER = Path("tests")


def run(cmd, *args, **kwargs) -> subprocess.CompletedProcess:
    if verbose:
        print("[RUN]", " ".join([str(v) for v in cmd]) if isinstance(cmd, (list, tuple)) else str(cmd))
    code: subprocess.CompletedProcess = subprocess.run(cmd, *args, **kwargs)
    if code.returncode != 0:
        v = " ".join([str(v) for v in cmd]) if isinstance(cmd, (list, tuple)) else str(cmd)
        print("ERROR:", v)
        exit(1)
    return code


def run_tests(filter: str | None) -> None:
    names = [f.strip() for f in filter.split(",")] if filter else []
    include = set([name for name in names if not name.startswith("-")])
    exclude = set([name[1:] for name in names if name.startswith("-")])

    def runnable(test: Path) -> bool:
        if include and test.name not in include:
            return False
        if exclude and test.name in exclude:
            return False
        return test.is_dir()

    tests = [t for t in TESTS_FOLDER.iterdir() if runnable(t)]

    max_workers = filter and 1 or (os.cpu_count() or 1)
    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        futures = {pool.submit(process, t): t for t in tests}
        for fut in as_completed(futures):
            t = futures[fut]
            try:
                fut.result()
            except Exception as e:
                print(f"[{t.name}] failed: {e}")


def process(test: Path) -> None:
    test /= "test"
    print(test.parent.name)

    program = test.with_suffix(".easy")

    flags = []

    x = test.parent / "x" / "test"

    removals: list[Path] = []

    expected_tokens = x.with_suffix(".tokens")
    if expected_tokens.exists():
        created_tokens = test.with_suffix(".tokens")
        removals.append(created_tokens)

        flags.append("-t")

    expected_ast = x.with_suffix(".ast")
    if expected_ast.exists():
        created_ast = test.with_suffix(".ast")
        removals.append(created_ast)

        flags.append("-a")

    expected_c = x.with_suffix(".c")
    if expected_c.exists():
        created_c = test.with_suffix(".c")
        removals.append(created_c)

        flags.extend(["-o", str(created_c)])

    run(["python", "easy.py", program, *flags])

    if expected_tokens.exists():
        diff(expected_tokens, created_tokens)

    if expected_ast.exists():
        diff(expected_ast, created_ast)

    if expected_c.exists():
        diff(expected_c, created_c)

    expected_output = x.with_suffix(".output")
    if expected_output.exists():
        created_output = test.with_suffix(".output")
        removals.append(created_output)

        exe = test.with_suffix(".exe")
        removals.append(exe)

        cc_flags = ["-Wall", "-Wextra", "-Werror"]
        run(["clang", *cc_flags, test.with_suffix(".c"), "-I", ".", "-o", exe])

        cmd = [exe, ">" + str(test.with_suffix(".output"))]

        input_file = x.with_suffix(".input")
        if input_file.exists():
            cmd.insert(1, "<" + str(input_file))

        executor = " ".join(map(str, cmd))

        run(executor, shell=True)

        diff(expected_output, created_output)

    for removal in removals:
        if removal.exists():
            if verbose:
                print(f"[DEL] {removal}")
            removal.unlink()


WHITE = "\033[97m"
RED = "\033[91m"
NC = "\033[0m"


def diff(expected_file: Path, created_file: Path) -> None:
    expected_lines = expected_file.read_text().splitlines()
    created_lines = created_file.read_text().splitlines()

    if expected_lines == created_lines:
        return

    if update:
        print(f"copy {created_file} -> {expected_file}")
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
