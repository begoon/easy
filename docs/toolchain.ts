// The in-browser C toolchain behind the playground's "run" button.
//
// Clang/LLD come from the YoWASP build of LLVM, loaded on the first run
// straight from jsDelivr (about 100 MB, then served from the browser
// cache). CLANG_VERSION must match the `@yowasp/clang` devDependency in
// package.json, which `bun test-wasm.ts` uses to run the same toolchain
// outside the browser; that script fails if the two disagree.

export const CLANG_VERSION = "21.1.4-3";
export const CLANG_URL = `https://cdn.jsdelivr.net/npm/@yowasp/clang@${CLANG_VERSION}/gen/bundle.js`;

export const C_FILE = "program.c";
export const WASM_FILE = "program.wasm";

export const CLANG_ARGS = [
    "-std=c23",
    // wasi-libc hides POSIX declarations such as nanosleep() under strict
    // -std=c23; glibc and macOS expose them anyway.
    "-D_DEFAULT_SOURCE",
    "-O2",
    // wasm-ld's default 64 KiB stack is far below the 8 MiB native one.
    // --stack-first puts it below static data, so an overflow traps
    // instead of silently corrupting globals.
    "-Wl,-z,stack-size=8388608",
    "-Wl,--stack-first",
    // 32-bit Wasm can address 4 GiB; memory still grows on demand.
    "-Wl,--max-memory=4294967296",
    C_FILE,
    "-o",
    WASM_FILE,
];
