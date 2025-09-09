from runtime import runtime_print

a = 0
a = 100
runtime_print("abc " + str(a))
a = 0
while True:
    if a > 10:
        break
    runtime_print(str(a))
    a += 1
exit(0)
