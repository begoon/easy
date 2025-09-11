from runtime import FIX, FLOAT, runtime_print

topnum = 0


def abs(x):
    if (x < 0):
        return (-x)
    else:
        return x
    return 0


def integersqrt(a):
    if (a < 0):
        runtime_print('a < 0 in FUNCTION integersqrt.')
        exit(0)
    elif (a == 0):
        return 0
    elif (a > 0):
        x = 0.0
        ra = 0.0
        epsilon = 0.0
        sqrt = 0
        ra = FLOAT(a)
        epsilon = (1e-07 * ra)
        x = (ra / 2.0)
        while True:
            if not (abs((ra - (x * x))) > epsilon):
                break
            pass
            x += (((ra / x) - x) / 2.0)
        sqrt = (FIX(x) - 1)
        while True:
            if not (((sqrt + 1) * (sqrt + 1)) <= a):
                break
            pass
            sqrt += 1
        return sqrt
    return 0


topnum = int(input())
if (topnum > 0):
    sieve = [False for _ in range(1+topnum+1)]
    i = 0
    limit = 0
    count = 0
    topnum = (topnum + 1)
    i = 1
    while True:
        if i > topnum:
            break
        sieve[i] = True
        i += 1
    limit = (integersqrt(topnum) + 1)
    i = 2
    while True:
        if i > limit:
            break
        if sieve[i]:
            j = 0
            j = (2 * i)
            while True:
                if j > topnum:
                    break
                sieve[j] = False
                j += i
        i += 1
    count = 0
    i = 1
    while True:
        if i > topnum:
            break
        if sieve[i]:
            count = (count + 1)
            runtime_print('Prime[' + str(count) + '] = ' + str(i))
        i += 1
else:
    runtime_print('Input value ' + str(topnum) + ' non-positive.')
exit(0)
