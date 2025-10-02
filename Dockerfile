FROM oven/bun:1-debian AS bun
FROM silkeh/clang:latest

COPY --from=bun /usr/local/bin/bun /usr/local/bin/bun

COPY easy.peg ./easy.peg
COPY peg.ts ./peg.ts

COPY easyc.ts ./easyc.ts
COPY test.ts ./test.ts
COPY runtime.c ./runtime.c
COPY tests/ ./tests/

CMD ["bun", "test.ts"]
