import itertools
import os
import subprocess
import sys
import traceback
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

from easy import arg

TESTS_FOLDER = Path("tests")


def run(cmd, *args, **kwargs) -> None:
    cmd = " ".join([str(v) for v in cmd]) if isinstance(cmd, (list, tuple)) else str(cmd)
    if verbose:
        print("[RUN]", cmd)
    code: subprocess.CompletedProcess = subprocess.run(cmd, *args, shell=True, **kwargs)
    if code.returncode != 0:
        print(f"ERROR: [{cmd}] -> {code.returncode}")
        exit(1)


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
        failed = []
        for future in as_completed(futures):
            task = futures[future]
            try:
                future.result()
            except Exception as e:
                failed.append(task)
                print(f"[{task.name}] failed: {e}")
                print(traceback.format_exc())
        if failed:
            print("failed tests:")
            for t in failed:
                print(f" - {t.name}")


def process(test: Path) -> None:
    test /= "test"
    if verbose:
        print("[TEST]", test.parent.name)
    else:
        print(test.parent.name, end=" ", flush=True)

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

    expected_yaml = x.with_suffix(".yaml")
    if expected_yaml.exists():
        created_yaml = test.with_suffix(".yaml")
        removals.append(created_yaml)

        flags.append("-y")

    expected_peg_ast = x.with_suffix(".json")
    if expected_peg_ast.exists():
        created_peg_ast = test.with_suffix(".json")
        removals.append(created_peg_ast)

        flags.append("-j")

    expected_c = x.with_suffix(".c")
    if expected_c.exists():
        created_c = test.with_suffix(".c")
        removals.append(created_c)

        flags.extend(["-c", str(created_c)])

    expected_py = x.with_suffix(".py")
    if expected_py.exists():
        created_py = test.with_suffix(".py")
        removals.append(created_py)

        flags.extend(["-p", str(created_py)])

    run(["python", "easy.py", program, *flags])

    if expected_tokens.exists():
        diff(expected_tokens, created_tokens)

    if expected_ast.exists():
        diff(expected_ast, created_ast)

    if expected_yaml.exists():
        diff(expected_yaml, created_yaml)

    if expected_peg_ast.exists():
        diff(expected_peg_ast, created_peg_ast)

    if expected_c.exists():
        diff(expected_c, created_c)

    if expected_py.exists():
        diff(expected_py, created_py)

    expected_output = x.with_suffix(".output")
    if expected_output.exists():
        created_output = test.with_suffix(".output")
        removals.append(created_output)

        if expected_c.exists():
            exe = test.with_suffix(".exe")
            removals.append(exe)

            cc_flags = ["-Wall", "-Wextra", "-Werror"]
            run(["clang", *cc_flags, test.with_suffix(".c"), "-I", ".", "-o", exe])

            cmd = [exe, ">" + str(test.with_suffix(".output"))]

            input_file = x.with_suffix(".input")
            if input_file.exists():
                cmd.append("<" + str(input_file))

            run(cmd)
            diff(expected_output, created_output)

            created_output.unlink()

        if expected_py.exists():
            cmd = ["python", created_py, ">" + str(test.with_suffix(".output"))]
            input_file = x.with_suffix(".input")
            if input_file.exists():
                cmd.append("<" + str(input_file))

            run(cmd, env={**os.environ, "PYTHONPATH": "."})
            diff(expected_output, created_output)

    for removal in removals:
        if removal.exists():
            if verbose > 1:
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
            mismatch_column = 1
            for c1, c2 in itertools.zip_longest(expected or "", created or ""):
                if c1 != c2:
                    break
                mismatch_column += 1

            print(f"{expected_file}:{i}:{RED}{mismatch_column}{NC}")
            print(f"          {" " * (mismatch_column-1)}↓")
            print(f"expected: {WHITE}{expected}{NC}")
            print(f"created:  {RED}{created}{NC}")
            print(f"          {" " * (mismatch_column-1)}↑")
            print(f"{created_file}:{i}:{RED}{mismatch_column}{NC}")
            exit(1)


verbose = (v := os.getenv("VERBOSE", arg(sys.argv, "--verbose"))) and int(v) or 0

update = "--update" in sys.argv or "-U" in sys.argv or os.getenv("UPDATE")


def flag(argv: list[str], name: str) -> int | None:
    return argv.index(name) if name in argv else None


def arg(argv: list[str], name: str) -> str | None:
    i = flag(argv, name)
    return argv[i + 1] if i is not None and i + 1 < len(argv) else None


if __name__ == "__main__":
    name = arg(sys.argv, "--filter")
    run_tests(name)
    if not verbose:
        print()
