def runtime_print(*args, **kwargs) -> None:
    print(*args, end="", sep="")
    if args and len(str(args[-1])) > 1:
        print()


def CHARACTER(n: int) -> str:
    return chr(n)


def FLOAT(n: int) -> float:
    return float(n)


def FIX(n: float) -> int:
    return int(n)
