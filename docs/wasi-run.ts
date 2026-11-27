// Runs a compiled Easy program (a WASI command module) to completion.
// Shared by the playground's run worker and test-wasm.ts, so the browser
// and the test suite execute programs through the same WASI shim.

import { ConsoleStdout, File, OpenFile, WASI } from "@bjorn3/browser_wasi_shim";

export type Stream = "stdout" | "stderr";

export class OutputLimitError extends Error {}

// Only what wasi-libc needs for standard streams, clocks, sleep and exit.
// No preopened directories: a program has no filesystem.
const PERMITTED_IMPORTS = [
    "args_get", "args_sizes_get", "environ_get", "environ_sizes_get",
    "clock_time_get", "clock_res_get", "poll_oneoff", "random_get",
    "proc_exit", "sched_yield",
    "fd_read", "fd_write", "fd_seek", "fd_close", "fd_fdstat_get", "fd_filestat_get",
    "fd_prestat_get", "fd_prestat_dir_name",
];

// Returns the exit status. Throws WebAssembly.RuntimeError on a trap
// (e.g. stack overflow) and OutputLimitError past `outputLimit` bytes.
export async function runWasi(
    wasm: BufferSource,
    stdin: string,
    onOutput: (stream: Stream, bytes: Uint8Array) => void,
    outputLimit = Infinity,
): Promise<number> {
    let remaining = outputLimit;
    const sink = (stream: Stream) =>
        new ConsoleStdout((bytes) => {
            const allowed = Math.max(0, Math.min(bytes.length, remaining));
            if (allowed > 0) onOutput(stream, bytes.subarray(0, allowed));
            remaining -= bytes.length;
            if (remaining < 0) throw new OutputLimitError(`output exceeded ${outputLimit} bytes`);
        });
    const wasi = new WASI(["program"], [], [
        new OpenFile(new File(new TextEncoder().encode(stdin), { readonly: true })),
        sink("stdout"),
        sink("stderr"),
    ]);
    const imports = Object.fromEntries(PERMITTED_IMPORTS.map((name) => [name, wasi.wasiImport[name]]));
    const { instance } = await WebAssembly.instantiate(wasm, { wasi_snapshot_preview1: imports });
    return wasi.start(instance as Parameters<WASI["start"]>[0]);
}
