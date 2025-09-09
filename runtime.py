def runtime_print(*args, **kwargs) -> None:
    print(*args, end="")
    if len(args) > 0 and len(args[-1]) > 1:
        print()
