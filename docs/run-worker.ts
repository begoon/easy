// Module worker: executes one compiled program, then is terminated.
// Stop works by terminating the worker, so a program can loop forever
// without freezing the page. The program only sees the WASI imports in
// wasi-run.ts: stdin from the request, stdout/stderr, clocks and exit.

import { OutputLimitError, runWasi, type Stream } from "./wasi-run.ts";

export type RunRequest = { wasm: Uint8Array<ArrayBuffer>; stdin: string };
export type RunMessage =
    | { type: "output"; chunks: [Stream, string][] }
    | { type: "exit"; status: number }
    | { type: "error"; text: string };

export const OUTPUT_LIMIT = 4 * 1024 * 1024;

// The program runs synchronously, so timers never fire while it does.
// Output is batched here and flushed every ~50 ms of wall time instead.
const decoders = { stdout: new TextDecoder(), stderr: new TextDecoder() };
let chunks: [Stream, string][] = [];
let lastFlush = 0;

function flush() {
    for (const stream of ["stdout", "stderr"] as const) {
        const tail = decoders[stream].decode();
        if (tail) chunks.push([stream, tail]);
    }
    if (chunks.length) self.postMessage({ type: "output", chunks } satisfies RunMessage);
    chunks = [];
    lastFlush = performance.now();
}

function onOutput(stream: Stream, bytes: Uint8Array) {
    const text = decoders[stream].decode(bytes, { stream: true });
    if (!text) return;
    const last = chunks[chunks.length - 1];
    if (last && last[0] === stream) last[1] += text;
    else chunks.push([stream, text]);
    if (performance.now() - lastFlush > 50) flush();
}

self.onmessage = async ({ data }: MessageEvent<RunRequest>) => {
    let message: RunMessage;
    try {
        const status = await runWasi(data.wasm, data.stdin, onOutput, OUTPUT_LIMIT);
        message = { type: "exit", status };
    } catch (e) {
        const text =
            e instanceof OutputLimitError
                ? `program stopped: output exceeded ${OUTPUT_LIMIT / 1024 / 1024} MiB`
                : e instanceof WebAssembly.RuntimeError
                  ? `program crashed: ${e.message}`
                  : String(e);
        message = { type: "error", text };
    }
    flush();
    self.postMessage(message);
};
