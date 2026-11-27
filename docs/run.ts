// Main-thread side of "run": C -> .wasm in the compile worker, then the
// .wasm in a fresh run worker. Both honour an AbortSignal (the stop
// button) by terminating the worker involved.

import type { CompileMessage, CompileRequest } from "./compile-worker.ts";
import type { RunMessage, RunRequest } from "./run-worker.ts";
import type { Stream } from "./wasi-run.ts";

export class CompileFailure extends Error {}

let compiler: Worker | undefined;
let cached: { c: string; wasm: Uint8Array<ArrayBuffer> } | undefined;

function abortError(signal: AbortSignal): unknown {
    return signal.reason ?? new DOMException("aborted", "AbortError");
}

export function compileToWasm(
    c: string,
    onStatus: (text: string) => void,
    signal: AbortSignal,
): Promise<Uint8Array<ArrayBuffer>> {
    if (cached?.c === c) return Promise.resolve(cached.wasm);
    signal.throwIfAborted();
    // The worker keeps the loaded toolchain between runs; it is only
    // recreated after stop or a crash.
    const worker = (compiler ??= new Worker("compile-worker.js", { type: "module" }));
    return new Promise((resolve, reject) => {
        function finish(reset: boolean) {
            signal.removeEventListener("abort", abort);
            worker.onmessage = null;
            worker.onerror = null;
            if (reset) {
                worker.terminate();
                compiler = undefined;
            }
        }
        function abort() {
            finish(true);
            reject(abortError(signal));
        }
        signal.addEventListener("abort", abort, { once: true });
        worker.onerror = (e) => {
            finish(true);
            reject(new Error(`C compiler worker failed: ${e.message || "could not start"}`));
        };
        worker.onmessage = ({ data }: MessageEvent<CompileMessage>) => {
            if (data.type === "status") return onStatus(data.text);
            finish(false);
            if (data.type === "error") return reject(new CompileFailure(data.text));
            cached = { c, wasm: data.wasm };
            resolve(data.wasm);
        };
        worker.postMessage({ c } satisfies CompileRequest);
    });
}

export function runWasm(
    wasm: Uint8Array<ArrayBuffer>,
    stdin: string,
    onOutput: (stream: Stream, text: string) => void,
    signal: AbortSignal,
): Promise<number> {
    signal.throwIfAborted();
    const worker = new Worker("run-worker.js", { type: "module" });
    return new Promise((resolve, reject) => {
        function finish() {
            signal.removeEventListener("abort", abort);
            worker.terminate();
        }
        function abort() {
            finish();
            reject(abortError(signal));
        }
        signal.addEventListener("abort", abort, { once: true });
        worker.onerror = (e) => {
            finish();
            reject(new Error(`run worker failed: ${e.message || "could not start"}`));
        };
        worker.onmessage = ({ data }: MessageEvent<RunMessage>) => {
            if (data.type === "output") {
                for (const [stream, text] of data.chunks) onOutput(stream, text);
                return;
            }
            finish();
            if (data.type === "exit") resolve(data.status);
            else reject(new Error(data.text));
        };
        // Structured clone copies the bytes, so the cached module survives.
        worker.postMessage({ wasm, stdin } satisfies RunRequest);
    });
}
