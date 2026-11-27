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

# Golden tests through the playground's in-WebAssembly clang + WASI shim.
test-wasm *NAMES:
    bun test-wasm.ts {{ NAMES }}

run NAME:
    bun easyc.ts tests/{{ NAME }}/test.easy \
    && cc -std=c23 tests/{{ NAME }}/test.c -o tests/{{ NAME }}/test.exe -I . -g -fsanitize=address \
    && ./tests/{{ NAME }}/test.exe

life:
    bun easyc.ts life.easy && clang -std=c23 life.c -o life && ./life

clean:
    git clean -Xf

test-docker:
    docker build --platform linux/amd64 -t easy . && docker run --platform linux/amd64 --rm easy

build-playground:
    bun run build:playground

playground: build-playground
    python3 -m http.server -d docs

build:
    bun run build

# Rebuild video/easy-playground.mp4: narration, a live recording of the
# playground, then the mix (see video/README.md). Not committed.
video: build-playground
    cd video && bun install --frozen-lockfile --silent
    mkdir -p video/.cache
    test -f video/.cache/kokoro-v1.0.onnx || curl -fsSL -o video/.cache/kokoro-v1.0.onnx https://github.com/thewh1teagle/kokoro-onnx/releases/download/model-files-v1.0/kokoro-v1.0.onnx
    test -f video/.cache/voices-v1.0.bin || curl -fsSL -o video/.cache/voices-v1.0.bin https://github.com/thewh1teagle/kokoro-onnx/releases/download/model-files-v1.0/voices-v1.0.bin
    cd video && uv run --python 3.12 --with kokoro-onnx --with soundfile python narrate.py
    cd video && bun capture.ts
    cd video && uv run --python 3.12 python build.py

# Build, bump patch version, and publish to npm.
# --access=public is required for scoped packages (@begoon/easyc) since
# npm defaults scoped names to private.
publish: quick build
    npm version patch --no-git-tag-version
    npm publish --access=public
