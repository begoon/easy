// Module worker: compiles generated C (plus runtime.c) to a WASI .wasm
// with YoWASP Clang. The toolchain is imported from jsDelivr on the first
// request and stays loaded for the lifetime of the worker.

import runtimeC from "../runtime.c" with { type: "text" };
import { CLANG_ARGS, CLANG_URL, WASM_FILE, C_FILE } from "./toolchain.ts";

export type CompileRequest = { c: string };
export type CompileMessage =
    | { type: "status"; text: string }
    | { type: "done"; wasm: Uint8Array<ArrayBuffer> }
    | { type: "error"; text: string };

type Clang = typeof import("@yowasp/clang");

const post = (message: CompileMessage, transfer: Transferable[] = []) => self.postMessage(message, { transfer });

let clang: Promise<Clang> | undefined;

const MB = 1024 * 1024;

async function loadClang(): Promise<Clang> {
    post({ type: "status", text: "loading C compiler…" });
    // A runtime-built URL keeps the bundler from trying to resolve it.
    const module = (await import(String(new URL(CLANG_URL)))) as Clang;
    // Fetch the ~100 MB of resources up front: commands.clang() would
    // fetch them too, but through an internal `clang -###` call that
    // drops our fetchProgress callback.
    let lastPercent = -1;
    await module.commands.clang(null as unknown as string[], {}, {
        fetchProgress: ({ doneLength, totalLength }) => {
            const percent = Math.floor((100 * doneLength) / totalLength);
            if (percent === lastPercent) return;
            lastPercent = percent;
            const done = (doneLength / MB).toFixed(0);
            const total = (totalLength / MB).toFixed(0);
            post({ type: "status", text: `downloading C compiler… ${percent}% (${done} / ${total} MB, once per browser)` });
        },
    });
    return module;
}

self.onmessage = async ({ data }: MessageEvent<CompileRequest>) => {
    let stderr = "";
    const decoder = new TextDecoder();
    try {
        const { commands } = await (clang ??= loadClang().catch((e) => {
            clang = undefined;
            throw e;
        }));
        post({ type: "status", text: "compiling C…" });
        const files = await commands.clang(CLANG_ARGS, { [C_FILE]: data.c, "runtime.c": runtimeC }, {
            decodeASCII: false,
            stderr: (bytes) => {
                stderr += decoder.decode(bytes ?? undefined, { stream: !!bytes });
            },
        });
        const wasm = files?.[WASM_FILE];
        if (!(wasm instanceof Uint8Array)) throw new Error("clang produced no " + WASM_FILE);
        post({ type: "done", wasm: wasm as Uint8Array<ArrayBuffer> }, [wasm.buffer]);
    } catch (e) {
        const text = stderr.trim() || (e instanceof Error ? e.message : String(e));
        post({ type: "error", text });
    }
};
