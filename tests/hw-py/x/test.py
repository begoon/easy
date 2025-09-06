import preamble # noqa:  F401

a = 0
a = 100
print('abc ' + str(a))
a = 0
while True:
    if a > 10:
        break
    print(str(a))
    a += 1
exit(0)
