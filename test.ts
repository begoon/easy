import * as fs from "node:fs";
import * as os from "node:os";
import * as path from "node:path";

import child_process from "node:child_process";
import process from "node:process";
import { promisify } from "node:util";

const exec = promisify(child_process.exec);

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
    const { VERBOSE } = process.env;
    const i = flag(process.argv, "--verbose");
    const n = VERBOSE ?? (i !== null ? "1" : undefined);
    const value = n ? Number(n) : 0;
    return Number.isFinite(value) ? value : 0;
})();

const update: boolean = process.argv.includes("--update") || process.argv.includes("-U") || Boolean(process.env.UPDATE);

async function run(cmd: string | string[], opts: { env?: Record<string, string> } = {}) {
    const command = Array.isArray(cmd) ? cmd.join(" ") : String(cmd);
    if (verbose) console.log("[RUN]", command);
    try {
        await exec(command, {
            env: { ...process.env, ...(opts.env || {}) },
        });
    } catch (e: any) {
        const code = typeof e?.code === "number" ? e.code : 1;
        console.error(`ERROR: [${command}] -> ${code}`);
        if (e?.stdout) process.stdout.write(e.stdout);
        if (e?.stderr) process.stderr.write(e.stderr);
        process.exit(1);
    }
}

function listTests(filter: string | null): string[] {
    const names = filter
        ? filter
              .split(",")
              .map((s) => s.trim())
              .filter(Boolean)
        : [];
    const include = new Set(names.filter((n) => !n.startsWith("-")));
    const exclude = new Set(names.filter((n) => n.startsWith("-")).map((n) => n.slice(1)));
    const contain = Boolean(filter && filter.startsWith("@"));
    const substring = contain ? filter!.slice(1) : "";

    const entires = fs.readdirSync(TESTS_FOLDER, { withFileTypes: true });
    return entires
        .filter((dir) => dir.isDirectory())
        .map((dir) => dir.name)
        .filter((name) => {
            if (contain && name.includes(substring)) return true;
            if (include.size && !include.has(name)) return false;
            if (exclude.size && exclude.has(name)) return false;
            return true;
        })
        .map((name) => path.join(TESTS_FOLDER, name));
}

async function runTests(filter: string | null) {
    const testDirs = listTests(filter);
    const maxWorkers = filter ? 1 : os.cpus()?.length || 1;

    const queue = [...testDirs];
    const failed: string[] = [];

    async function worker() {
        while (queue.length) {
            const dir = queue.shift()!;
            try {
                await runTest(dir);
            } catch (e) {
                const error = e as Error;
                failed.push(dir);
                console.error(`[${path.basename(dir)}] failed: ${error?.message || error}`);
                console.error(error?.stack || "");
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

function exists(file: string): boolean {
    try {
        return fs.statSync(file).isFile();
    } catch {
        return false;
    }
}

function diff(expectedFile: string, createdFile: string) {
    const expectedLines = fs.readFileSync(expectedFile, "utf-8").split(/\r?\n/);
    const createdLines = fs.readFileSync(createdFile, "utf-8").split(/\r?\n/);

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

async function runTest(test_folder: string) {
    const test_stem = path.join(test_folder, "test");
    if (verbose) console.log("[TEST]", path.basename(test_folder));
    else process.stdout.write(path.basename(test_folder) + " ");

    const program = test_stem + ".easy";
    const x = path.join(test_folder, "x", "test");

    const removals: string[] = [];
    const flags: string[] = [];

    const expected_tokens = x + ".tokens";
    let created_tokens = test_stem + ".tokens";
    if (exists(expected_tokens)) {
        removals.push(created_tokens);
        flags.push("-t");
    }

    const expected_ast = x + ".json";
    const created_ast = test_stem + ".json";
    if (exists(expected_ast)) {
        removals.push(created_ast);
        flags.push("-a");
    }

    const expected_peg_ast = x + ".peg.json";
    const created_peg_ast = test_stem + ".peg.json";
    if (exists(expected_peg_ast)) {
        removals.push(created_peg_ast);
        await run(["bun", "peg.ts", program]);
    }

    await run(["bun", "easyc.ts", program, ...flags]);

    const expected_c = x + ".c";
    const created_c = test_stem + ".c";
    if (exists(expected_c)) {
        removals.push(created_c);
        flags.push("-c", created_c);
    }

    const expected_s = x + ".s";
    const created_s = test_stem + ".s";
    if (exists(expected_s)) {
        removals.push(created_s);
        flags.push("-s", created_s);
    }

    if (exists(expected_tokens)) diff(expected_tokens, created_tokens);
    if (exists(expected_ast)) diff(expected_ast, created_ast);
    if (exists(expected_peg_ast)) diff(expected_peg_ast, created_peg_ast);
    if (exists(expected_c)) diff(expected_c, created_c);
    if (exists(expected_s)) diff(expected_s, created_s);

    const skip_run = exists(expected_c) && Boolean(process.env.SKIP_RUN);

    const cc_flags = ["-Wall", "-Wextra", "-Werror", "-std=c23", "-g", "-fsanitize=address"];

    const expected_output = x + ".output";
    if (exists(expected_output) && !skip_run) {
        const created_output = test_stem + ".output";
        removals.push(created_output);

        if (exists(expected_c)) {
            const exe = test_stem + ".exe";
            removals.push(exe);

            await run(["clang", ...cc_flags, created_c, "-I", ".", "-o", exe]);

            const cmd: string[] = [exe, ">" + JSON.stringify(created_output)];
            const input_file = x + ".input";
            if (exists(input_file)) cmd.push("<" + JSON.stringify(input_file));

            await run(cmd);
            diff(expected_output, created_output);

            if (exists(created_output)) fs.unlinkSync(created_output);
        }
    } else {
        if (exists(expected_c)) {
            const obj = test_stem + ".o";
            removals.push(obj);
            await run(["clang", ...cc_flags, created_c, "-I", ".", "-c", "-o", obj]);
        }
    }

    for (const f of removals) {
        if (exists(f)) {
            if (verbose > 1) console.log(`[DEL] ${f}`);
            fs.unlinkSync(f);
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
