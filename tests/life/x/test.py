from runtime import CHARACTER, runtime_print

w = 0
h = 0
field = [[False for _ in range(0+80+1)] for _ in range(0+25+1)]
x = 0
y = 0
i = 0


def valid(x, y) -> int:
    return (not x < 0 or x >= w or y < 0 or y >= h)


def neighbours(x, y) -> int:
    n = 0
    xx = 0
    yy = 0
    n = 0
    xx = x - 1
    while True:
        if xx > x + 1:
            break
        yy = y - 1
        while True:
            if yy > y + 1:
                break
            if xx != x or yy != y:
                if valid(xx, yy):
                    if field[yy][xx]:
                        n = n + 1
            yy += 1
        xx += 1
    return n


def print():
    x = 0
    y = 0
    runtime_print('** [ EASY LIFE ]', ' ')
    x = 0
    while True:
        if x > w + 1 - 17:
            break
        runtime_print('*')
        x += 1
    runtime_print(' ' + CHARACTER(13))
    y = 0
    while True:
        if y > h - 1:
            break
        runtime_print('*')
        x = 0
        while True:
            if x > w - 1:
                break
            if field[y][x]:
                runtime_print('x')
            else:
                runtime_print(' ')
            x += 1
        runtime_print('*' + CHARACTER(13))
        y += 1
    x = 0
    while True:
        if x > w + 1:
            break
        runtime_print('*')
        x += 1
    runtime_print(' ' + CHARACTER(13))


def glider(x, y):
    field[y][x] = True
    field[y][(x + 1)] = True
    field[y][(x + 2)] = True
    field[(y + 1)][x] = True
    field[(y + 2)][(x + 1)] = True


def evolution():
    x = 0
    y = 0
    n = 0
    next = [[False for _ in range(0+80+1)] for _ in range(0+25+1)]
    y = 0
    while True:
        if y > h - 1:
            break
        x = 0
        while True:
            if x > w - 1:
                break
            alive = False
            alive = field[y][x]
            n = neighbours(x, y)
            if alive:
                if n < 2 or n > 3:
                    alive = False
            else:
                if n == 3:
                    alive = True
            next[y][x] = alive
            x += 1
        y += 1
    y = 0
    while True:
        if y > h - 1:
            break
        x = 0
        while True:
            if x > w - 1:
                break
            field[y][x] = next[y][x]
            x += 1
        y += 1


w = 80
h = 25
y = 0
while True:
    if y > h - 1:
        break
    x = 0
    while True:
        if x > w - 1:
            break
        field[y][x] = False
        x += 1
    y += 1
glider(30, 15)
glider(40, 10)
glider(50, 20)
i = 1
while True:
    if i > 12:
        break
    print()
    runtime_print('GENERATION: ' + str(i))
    evolution()
    if i % 10 == 0:
        glider(40, 10)
        glider(30, 15)
    i += 1
exit(0)
