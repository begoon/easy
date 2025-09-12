from runtime import FIX, runtime_print

row = [0 for _ in range(1+32+1)]
N = 0
n = 0
i = 0


def step():
    i = 0
    next = [0 for _ in range(1+32+1)]
    i = 1
    while True:
        if i > N:
            break
        next[i] = 0
        i += 1
    i = 1
    while True:
        if i > N:
            break
        left = 0
        c = 0
        right = 0
        s = 0
        j = 0
        j = (i - 1)
        if (j < 1):
            j = N
        left = row[j]
        c = row[i]
        j = (i + 1)
        if (j > N):
            j = 1
        right = row[j]
        s = ((left + c) + right)
        if (s == 2):
            next[i] = 1
        elif ((s == 0) or (s == 3)):
            next[i] = 0
        else:
            if (c == 1):
                next[i] = 1
            else:
                next[i] = right
        i += 1
    i = 1
    while True:
        if i > N:
            break
        row[i] = next[i]
        i += 1


def print():
    i = 0
    i = 1
    while True:
        if i > N:
            break
        if (row[i] == 1):
            runtime_print('X')
        else:
            runtime_print('.')
        i += 1
    runtime_print('  ')


N = 32
i = 1
while True:
    if i > N:
        break
    row[i] = 0
    i += 1
i = FIX((N / 2))
row[i] = 1
print()
n = 1
while True:
    if n > 10:
        break
    step()
    print()
    n += 1
