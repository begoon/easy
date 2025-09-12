def runtime_print(*args, **kwargs) -> None:
    if not args:
        return
    print(*args, end="", sep="")
    last = args[-1]
    if len(str(last)) > 1 or isinstance(last, int):
        print()


def CHARACTER(n: int) -> str:
    return chr(n)


def FLOAT(n: int) -> float:
    return float(n)


def FIX(n: float) -> int:
    return int(n)
