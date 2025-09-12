from runtime import runtime_print

n = 0


def FizzBuzz(n):
    i = 0
    i = 1
    while True:
        if i > n:
            break
        if (((i % 3) == 0) and ((i % 5) == 0)):
            runtime_print('FizzBuzz')
        elif ((i % 3) == 0):
            runtime_print('Fizz')
        elif ((i % 5) == 0):
            runtime_print('Buzz')
        else:
            runtime_print(i)
        i += 1


FizzBuzz(20)
