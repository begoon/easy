// Runs every golden tests/<name>/x/test.c through the playground's
// in-browser toolchain: YoWASP Clang with the same flags (docs/toolchain.ts)
// and the same WASI shim (docs/wasi-run.ts), then diffs stdout against
// x/test.output the way test.ts does. Catches native-vs-wasm drift.
//
//   bun test-wasm.ts [name...]
//
// Tests in SLOW only run when named. Wasm object files store even
// zero-initialised globals as literal zeros (wasm-ld drops them at link
// time), so string_alloc's 160 MB static array makes a 125 MB .o that
// takes in-Wasm clang about two minutes to write.

import * as fs from "node:fs";
import * as path from "node:path";
import { commands } from "@yowasp/clang";
import { CLANG_ARGS, CLANG_VERSION, C_FILE, WASM_FILE } from "./docs/toolchain.ts";
import { runWasi } from "./docs/wasi-run.ts";

const root = path.dirname(new URL(import.meta.url).pathname);
const pkg = JSON.parse(fs.readFileSync(path.join(root, "package.json"), "utf8"));
if (pkg.devDependencies?.["@yowasp/clang"] !== CLANG_VERSION) {
    console.error(
        `docs/toolchain.ts pins clang ${CLANG_VERSION} but package.json has ${pkg.devDependencies?.["@yowasp/clang"]}`,
    );
    process.exit(1);
}

const runtime = fs.readFileSync(path.join(root, "runtime.c"), "utf8");
const only = process.argv.slice(2);
const SLOW = new Set(["string_alloc"]);
const names = fs
    .readdirSync(path.join(root, "tests"))
    .filter((name) => fs.existsSync(path.join(root, "tests", name, "x", "test.output")))
    .filter((name) => (only.length === 0 ? !SLOW.has(name) : only.includes(name)))
    .sort();

const lines = (text: string) => {
    const result = text.split(/\r?\n/);
    if (result.at(-1) === "") result.pop();
    return result;
};

// Prefetch quietly; commands.clang() would log progress via `clang -###`.
await commands.clang(null as unknown as string[], {}, { fetchProgress: () => {} });

let failed = 0;
for (const name of names) {
    const x = path.join(root, "tests", name, "x");
    const started = performance.now();
    let stderr = "";
    let verdict: string;
    try {
        const files = await commands.clang(
            CLANG_ARGS,
            { [C_FILE]: fs.readFileSync(path.join(x, "test.c"), "utf8"), "runtime.c": runtime },
            {
                decodeASCII: false,
                stderr: (bytes) => bytes && (stderr += new TextDecoder().decode(bytes)),
            },
        );
        const compiled = performance.now();
        const input = path.join(x, "test.input");
        const stdin = fs.existsSync(input) ? fs.readFileSync(input, "utf8") : "";
        const chunks: Uint8Array[] = [];
        const status = await runWasi(files![WASM_FILE] as Uint8Array<ArrayBuffer>, stdin, (stream, bytes) => {
            if (stream === "stdout") chunks.push(bytes.slice());
            else process.stderr.write(bytes);
        });
        const got = lines(new TextDecoder().decode(Buffer.concat(chunks)));
        const expected = lines(fs.readFileSync(path.join(x, "test.output"), "utf8"));
        const same = got.length === expected.length && got.every((line, i) => line === expected[i]);
        const timing = `compile ${(compiled - started).toFixed(0)} ms, run ${(performance.now() - compiled).toFixed(0)} ms`;
        verdict = same && status === 0 ? `ok (${timing})` : `FAIL exit ${status}, output ${same ? "matches" : "differs"} (${timing})`;
        if (!same) {
            const i = got.findIndex((line, i) => line !== expected[i]);
            const at = i === -1 ? Math.min(got.length, expected.length) : i;
            verdict += `\n    line ${at + 1}: expected ${JSON.stringify(expected[at])}, got ${JSON.stringify(got[at])}`;
        }
    } catch (e) {
        verdict = `FAIL ${stderr.trim() || e}`;
    }
    if (verdict.startsWith("FAIL")) failed++;
    console.log(`${name}: ${verdict}`);
}

console.log(`${names.length - failed}/${names.length} passed`);
process.exit(failed ? 1 : 0);
