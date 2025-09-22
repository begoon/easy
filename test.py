import itertools
import os
import subprocess
import sys
import traceback
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

import easyc
import peg
from easyc import arg, flag, printer
from peg import PEGParser

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
    compiler = arg(sys.argv, "--compiler") or os.getenv("COMPILER")
    if not compiler:
        raise Exception("COMPILER not set")

    names = [f.strip() for f in filter.split(",")] if filter else []
    include = set([name for name in names if not name.startswith("-")])
    exclude = set([name[1:] for name in names if name.startswith("-")])

    def runnable(test: Path) -> bool:
        if filter and filter.startswith("@") and filter[1:] in test.name:
            return True
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

    expected_ast = x.with_suffix(".json")
    if expected_ast.exists():
        created_ast = test.with_suffix(".json")
        removals.append(created_ast)

        flags.append("-a")

    compiler = arg(sys.argv, "--compiler") or os.getenv("COMPILER")
    if not compiler:
        raise Exception("COMPILER not set")

    expected_peg_ast = x.with_suffix(".peg.json")
    if expected_peg_ast.exists():
        created_peg_ast = test.with_suffix(".peg.json")
        removals.append(created_peg_ast)

        if compiler == "python":
            peg_grammar = Path("easy.peg").read_text()
            peg_ast = PEGParser(peg_grammar, start="compilation").parse(program.read_text())
            created_peg_ast.write_text(printer(peg_ast) + "\n")
        elif compiler == "python-ext":
            run(["python", "peg.py", program])
        else:
            run(["bun", "peg.ts", program])

    if compiler == "python":
        easyc.run(["easyc.py", str(program), *flags])
    elif compiler == "python-ext":
        run(["python", "easyc.py", program, *flags])
    elif compiler == "ts":
        run(["bun", "easyc.ts", str(program), *flags])
    else:
        raise Exception(f"unknown compiler: {compiler}")

    expected_c = x.with_suffix(".c")
    if expected_c.exists():
        created_c = test.with_suffix(".c")
        removals.append(created_c)

        flags.extend(["-c", str(created_c)])

    expected_s = x.with_suffix(".s")
    if expected_s.exists():
        created_s = test.with_suffix(".s")
        removals.append(created_s)

        flags.extend(["-s", str(created_s)])

    if expected_tokens.exists():
        diff(expected_tokens, created_tokens)

    if expected_ast.exists():
        diff(expected_ast, created_ast)

    if expected_peg_ast.exists():
        diff(expected_peg_ast, created_peg_ast)

    if expected_c.exists():
        diff(expected_c, created_c)

    if expected_s.exists():
        diff(expected_s, created_s)

    skip_run = expected_c.exists() and os.getenv("SKIP_RUN")

    cc_flags = ["-Wall", "-Wextra", "-Werror", "-std=c23"]

    expected_output = x.with_suffix(".output")
    if expected_output.exists() and not skip_run:
        created_output = test.with_suffix(".output")
        removals.append(created_output)

        if expected_c.exists():
            exe = test.with_suffix(".exe")
            removals.append(exe)

            run(["clang", *cc_flags, test.with_suffix(".c"), "-I", ".", "-o", exe])

            cmd = [exe, ">" + str(test.with_suffix(".output"))]

            input_file = x.with_suffix(".input")
            if input_file.exists():
                cmd.append("<" + str(input_file))

            run(cmd)
            diff(expected_output, created_output)

            created_output.unlink()
    else:
        if expected_c.exists():
            obj = test.with_suffix(".o")
            removals.append(obj)

            run(["clang", *cc_flags, test.with_suffix(".c"), "-I", ".", "-c", "-o", obj])

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

            print()
            print("-" * 80)
            print(f"{expected_file}:{i}:{RED}{mismatch_column}{NC}")
            print(f"          {" " * (mismatch_column-1)}↓")
            print(f"expected: {WHITE}{expected}{NC}")
            print(f"created:  {RED}{created}{NC}")
            print(f"          {" " * (mismatch_column-1)}↑")
            print(f"{created_file}:{i}:{RED}{mismatch_column}{NC}")
            print("-" * 80)
            exit(1)


verbose = (v := os.getenv("VERBOSE", flag(sys.argv, "--verbose"))) and int(v) or 0

update = "--update" in sys.argv or "-U" in sys.argv or os.getenv("UPDATE")


if __name__ == "__main__":
    name = arg(sys.argv, "--filter")
    run_tests(name)
    if not verbose:
        print()
