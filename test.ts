#!/usr/bin/env bun
/**
 * TS rewrite of the Python test harness.
 * Concurrency via Promises (no threads). Assumes Bun/Node + clang available.
 */

import { exec as _exec } from "node:child_process";
import * as fs from "node:fs";
import * as os from "node:os";
import * as path from "node:path";
import process from "node:process";
import { promisify } from "node:util";

const exec = promisify(_exec);

const TESTS_FOLDER = "tests";

const WHITE = "\x1b[97m";
const RED = "\x1b[91m";
const NC = "\x1b[0m";

function flag(argv: string[], name: string): number | null {
    const i = argv.indexOf(name);
    return i >= 0 ? i : null;
}
function arg(argv: string[], name: string): string | null {
    const i = flag(argv, name);
    return i !== null && i + 1 < argv.length ? argv[i + 1] : null;
}

const verbose: number = (() => {
    const envV = process.env.VERBOSE;
    const cliIdx = flag(process.argv, "--verbose");
    const v = envV ?? (cliIdx !== null ? "1" : undefined);
    const asNum = v ? Number(v) : 0;
    return Number.isFinite(asNum) ? asNum : 0;
})();

const update: boolean = process.argv.includes("--update") || process.argv.includes("-U") || Boolean(process.env.UPDATE);

/** run a command string or argv vector using the shell (so redirections work). */
async function run(cmd: string | string[], opts: { cwd?: string; env?: NodeJS.ProcessEnv } = {}): Promise<void> {
    const command = Array.isArray(cmd) ? cmd.join(" ") : String(cmd);
    if (verbose) console.log("[RUN]", command);
    try {
        await exec(command, {
            shell: true,
            cwd: opts.cwd,
            env: { ...process.env, ...(opts.env || {}) },
            maxBuffer: 10 * 1024 * 1024, // 10MB
        });
    } catch (e: any) {
        const code = typeof e?.code === "number" ? e.code : 1;
        console.error(`ERROR: [${command}] -> ${code}`);
        if (e?.stdout) process.stdout.write(e.stdout);
        if (e?.stderr) process.stderr.write(e.stderr);
        process.exit(1);
    }
}

function listTestDirs(filterSpec: string | null): string[] {
    const names = filterSpec
        ? filterSpec
              .split(",")
              .map((s) => s.trim())
              .filter(Boolean)
        : [];
    const include = new Set(names.filter((n) => !n.startsWith("-")));
    const exclude = new Set(names.filter((n) => n.startsWith("-")).map((n) => n.slice(1)));
    const substringMode = !!(filterSpec && filterSpec.startsWith("@"));
    const substring = substringMode ? filterSpec!.slice(1) : "";

    const ents = fs.readdirSync(TESTS_FOLDER, { withFileTypes: true });
    return ents
        .filter((d) => d.isDirectory())
        .map((d) => d.name)
        .filter((name) => {
            if (substringMode && name.includes(substring)) return true;
            if (include.size && !include.has(name)) return false;
            if (exclude.size && exclude.has(name)) return false;
            return true;
        })
        .map((name) => path.join(TESTS_FOLDER, name));
}

async function runTests(filterSpec: string | null) {
    const compiler = arg(process.argv, "--compiler") ?? process.env.COMPILER;
    if (!compiler) throw new Error("COMPILER not set");

    const testDirs = listTestDirs(filterSpec);
    const maxWorkers = filterSpec ? 1 : os.cpus()?.length || 1;

    const queue = [...testDirs];
    const failed: string[] = [];

    async function worker() {
        while (queue.length) {
            const dir = queue.shift()!;
            try {
                await processOne(dir, compiler);
            } catch (e: any) {
                failed.push(dir);
                console.error(`[${path.basename(dir)}] failed: ${e?.message || e}`);
                console.error(e?.stack || "");
            }
        }
    }

    const workers = Array.from({ length: Math.max(1, maxWorkers) }, () => worker());
    await Promise.all(workers);

    if (failed.length) {
        console.log("failed tests:");
        for (const t of failed) console.log(` - ${path.basename(t)}`);
        process.exit(1);
    }
}

function p(file: string): boolean {
    try {
        return fs.statSync(file).isFile();
    } catch {
        return false;
    }
}

function diff(expectedFile: string, createdFile: string) {
    const expectedLines = fs.readFileSync(expectedFile, "utf-8").split(/\r?\n/);
    const createdLines = fs.readFileSync(createdFile, "utf-8").split(/\r?\n/);

    // normalize trailing newline behavior (Python test writes newline at end)
    if (expectedLines.length && expectedLines[expectedLines.length - 1] === "") expectedLines.pop();
    if (createdLines.length && createdLines[createdLines.length - 1] === "") createdLines.pop();

    const equal =
        expectedLines.length === createdLines.length && expectedLines.every((line, i) => line === createdLines[i]);

    if (equal) return;

    if (update) {
        console.log(`copy ${createdFile} -> ${expectedFile}`);
        fs.writeFileSync(expectedFile, fs.readFileSync(createdFile));
        return;
    }

    const maxLen = Math.max(expectedLines.length, createdLines.length);
    for (let i = 0; i < maxLen; i++) {
        const expected = expectedLines[i];
        const created = createdLines[i];
        if (expected !== created) {
            let mismatch_column = 1;
            const maxCol = Math.max(expected?.length ?? 0, created?.length ?? 0);
            for (let c = 0; c < maxCol; c++) {
                const c1 = expected?.[c] ?? undefined;
                const c2 = created?.[c] ?? undefined;
                if (c1 !== c2) break;
                mismatch_column += 1;
            }

            console.log();
            console.log("-".repeat(80));
            const lineNo = i + 1;
            console.log(`${expectedFile}:${lineNo}:${RED}${mismatch_column}${NC}`);
            console.log(`          ${" ".repeat(Math.max(0, mismatch_column - 1))}↓`);
            console.log(`expected: ${WHITE}${expected ?? ""}${NC}`);
            console.log(`created:  ${RED}${created ?? ""}${NC}`);
            console.log(`          ${" ".repeat(Math.max(0, mismatch_column - 1))}↑`);
            console.log(`${createdFile}:${lineNo}:${RED}${mismatch_column}${NC}`);
            console.log("-".repeat(80));
            process.exit(1);
        }
    }
}

async function processOne(testDir: string, compiler: string) {
    const testStem = path.join(testDir, "test");
    if (verbose) console.log("[TEST]", path.basename(testDir));
    else process.stdout.write(path.basename(testDir) + " ");

    const program = testStem + ".easy";
    const xStem = path.join(testDir, "x", "test");

    const removals: string[] = [];
    const flags: string[] = [];

    const expected_tokens = xStem + ".tokens";
    let created_tokens = testStem + ".tokens";
    if (p(expected_tokens)) {
        removals.push(created_tokens);
        flags.push("-t");
    }

    const expected_ast = xStem + ".json";
    const created_ast = testStem + ".json";
    if (p(expected_ast)) {
        removals.push(created_ast);
        flags.push("-a");
    }

    const expected_peg_ast = xStem + ".peg.json";
    const created_peg_ast = testStem + ".peg.json";
    if (p(expected_peg_ast)) {
        removals.push(created_peg_ast);
        if (compiler === "python" || compiler === "python-ext") await run(["python", "peg.py", program]);
        else await run(["bun", "peg.ts", program]);
    }

    if (compiler === "python" || compiler === "python-ext") {
        await run(["python", "easy.py", program, ...flags]);
    } else if (compiler === "ts") {
        await run(["bun", "easyc.ts", program, ...flags]);
    } else {
        throw new Error(`unknown compiler: ${compiler}`);
    }

    const expected_c = xStem + ".c";
    const created_c = testStem + ".c";
    if (p(expected_c)) {
        removals.push(created_c);
        // flags for -c/-s in Python script were not used further; mimic behavior:
        flags.push("-c", created_c);
    }

    const expected_s = xStem + ".s";
    const created_s = testStem + ".s";
    if (p(expected_s)) {
        removals.push(created_s);
        flags.push("-s", created_s);
    }

    if (p(expected_tokens)) diff(expected_tokens, created_tokens);
    if (p(expected_ast)) diff(expected_ast, created_ast);
    if (p(expected_peg_ast)) diff(expected_peg_ast, created_peg_ast);
    if (p(expected_c)) diff(expected_c, created_c);
    if (p(expected_s)) diff(expected_s, created_s);

    const skip_run = p(expected_c) && Boolean(process.env.SKIP_RUN);

    const cc_flags = ["-Wall", "-Wextra", "-Werror", "-std=c23"];

    const expected_output = xStem + ".output";
    if (p(expected_output) && !skip_run) {
        const created_output = testStem + ".output";
        removals.push(created_output);

        if (p(expected_c)) {
            const exe = testStem + ".exe";
            removals.push(exe);

            await run(["clang", ...cc_flags, created_c, "-I", ".", "-o", exe]);

            // Build shell command with redirections (we use exec with shell=true)
            const cmd: string[] = [exe, ">" + JSON.stringify(created_output)];
            const input_file = xStem + ".input";
            if (p(input_file)) cmd.push("<" + JSON.stringify(input_file));

            await run(cmd);
            diff(expected_output, created_output);

            // delete output after successful diff (like Python)
            if (p(created_output)) fs.unlinkSync(created_output);
        }
    } else {
        if (p(expected_c)) {
            const obj = testStem + ".o";
            removals.push(obj);
            await run(["clang", ...cc_flags, created_c, "-I", ".", "-c", "-o", obj]);
        }
    }

    // cleanup
    for (const f of removals) {
        if (p(f)) {
            if (verbose > 1) console.log(`[DEL] ${f}`);
            try {
                fs.unlinkSync(f);
            } catch {
                /* ignore */
            }
        }
    }
}

async function main() {
    const name = arg(process.argv, "--filter");
    await runTests(name);
    if (!verbose) process.stdout.write("\n");
}

main().catch((e) => {
    console.error(e?.stack || String(e));
    process.exit(2);
});
