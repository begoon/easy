default: test-compiler

x:
    just test-docker

ci:
    just

quick:
    SKIP_RUN=1 just test-compiler

update:
    UPDATE=1 just test-compiler

test-compiler:
    bun run test.ts

one NAME:
    bun easyc.ts tests/{{NAME}}/test.easy \
    && cc -std=c23 tests/{{NAME}}/test.c -o tests/{{NAME}}/test.exe -I . -g -fsanitize=address \
    && ./tests/{{NAME}}/test.exe

life:
    bun easyc.ts life.easy && clang -std=c23 life.c -o life && ./life

clean:
    git clean -Xf

test-docker:
    docker build --platform linux/amd64 -t easy . && docker run --platform linux/amd64 --rm easy
