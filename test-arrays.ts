// Run with: bun test-arrays.ts
import * as fs from "node:fs";
import * as os from "node:os";
import * as path from "node:path";
import { spawnSync } from "node:child_process";
import { compileToC } from "./easyc.ts";

const directory = fs.mkdtempSync(path.join(os.tmpdir(), "easy-arrays-"));
type Case = { name: string; source?: string; c?: string; output?: string; error?: string };
const source = fs.readFileSync("tests/array_memory/test.easy", "utf8");
const cases: Case[] = [
    { name: "nested-and-bounds", source, output: fs.readFileSync("tests/array_memory/x/test.output", "utf8") },
    ...["fixed", "dynamic", "struct"].map(kind => ({
        name: `partial-initialization-${kind}`,
        source: `PROGRAM Partial:
            DECLARE calls INTEGER;
            FUNCTION bound() INTEGER:
                SET calls := calls + 1;
                IF calls = 2 THEN EXIT; FI;
                RETURN 1;
            END FUNCTION bound;
            BEGIN
                DECLARE value ${kind === "struct"
                    ? "STRUCTURE FIELD first IS ARRAY [0:bound()] OF STRING, FIELD second IS ARRAY [0:bound()] OF STRING END STRUCTURE"
                    : `ARRAY [0:${kind === "fixed" ? "1" : "1+0"}] OF ARRAY [0:bound()] OF STRING`};
            END;
        END PROGRAM Partial;`,
    })),
    {
        name: "bounds-evaluated-once",
        source: `PROGRAM Once:
            DECLARE (lo, calls) INTEGER;
            FUNCTION bound() INTEGER:
                SET calls := calls + 1;
                SET lo := 100;
                RETURN 1;
            END FUNCTION bound;
            BEGIN
                DECLARE a ARRAY [lo:bound()] OF INTEGER;
                SET a[0] := 7;
                OUTPUT a[0];
                OUTPUT calls;
            END;
        END PROGRAM Once;`,
        output: "7\n1\n",
    },
    {
        name: "changed-upper-rejected",
        source: `PROGRAM Upper:
            DECLARE n INTEGER;
            SET n := 1;
            BEGIN
                DECLARE a ARRAY [0:n] OF INTEGER;
                SET n := 100;
                SET a[100] := 42;
            END;
        END PROGRAM Upper;`,
        error: "array index out of bounds: 100 not in [0..1]",
    },
    {
        name: "copy-preserves-bounds",
        source: `PROGRAM CopyBounds:
            DECLARE n INTEGER;
            SET n := 1;
            BEGIN
                TYPE Values IS ARRAY [0:n] OF INTEGER;
                DECLARE a Values;
                SET a[1] := 77;
                SET n := 100;
                BEGIN
                    DECLARE b Values;
                    SET b := a;
                    OUTPUT b[1];
                    SET b[100] := 9;
                END;
            END;
        END PROGRAM CopyBounds;`,
        output: "77\n",
        error: "array index out of bounds: 100 not in [0..1]",
    },
    {
        name: "inverted-bounds",
        source: `PROGRAM Invalid:
            DECLARE n INTEGER;
            BEGIN DECLARE a ARRAY [1:n] OF INTEGER; END;
        END PROGRAM Invalid;`,
        error: "invalid array bounds: 1..0",
    },
    {
        name: "wide-arithmetic",
        c: `#include "runtime.c"
            #include <limits.h>
            int main_program() {
                size_t count;
                size_t bytes = array_allocation_size(0, INT_MAX, 1, &count);
                assert(count == (uint64_t)INT_MAX + 1);
                assert(bytes == sizeof(ArrayStorage) + count);
                #if SIZE_MAX > UINT32_MAX
                bytes = array_allocation_size(INT_MIN, INT_MAX, 1, &count);
                assert(count == (uint64_t)UINT32_MAX + 1);
                assert(bytes == sizeof(ArrayStorage) + count);
                ArrayStorage header = {.lo = INT_MIN, .hi = INT_MAX};
                const STR filename = {.data = "wide-arithmetic"};
                assert($array_index(header.data, INT_MAX, &filename, 1, 1) == UINT32_MAX);
                #endif
                return 0;
            }`,
    },
    {
        name: "byte-size-overflow",
        c: `#include "runtime.c"
            int main_program() {
                size_t count;
                array_allocation_size(0, 1, SIZE_MAX, &count);
                return 0;
            }`,
        error: "array allocation too large",
    },
];
try {
    for (const test of cases) {
        let c = test.c ?? compileToC(test.source!, `${test.name}.easy`);
        c = c.replace(/while \(1\)\n(\s*)\{/g,
            "while (1)\n$1{\n$1    assert(live_arrays < 32); assert(live_strings < 32);");
        const file = path.join(directory, `${test.name}.c`);
        const executable = path.join(directory, test.name);
        fs.writeFileSync(file, c);
        for (const optimization of ["-O0", "-O2"]) {
            const cc = spawnSync("clang", ["-std=c23", "-Wall", "-Wextra", "-Werror", optimization,
                "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-DEASY_CHECK_OWNERSHIP",
                "-I", process.cwd(), file, "-o", executable], { encoding: "utf8" });
            if (cc.status !== 0) throw new Error(`${test.name}: ${cc.stderr}`);
            const result = spawnSync(executable, [], { encoding: "utf8", timeout: 10000 });
            if (result.status !== (test.error ? 1 : 0) || result.stdout !== (test.output ?? "") ||
                (test.error ? !result.stderr.includes(test.error) : result.stderr !== "") ||
                /Sanitizer|runtime error:/.test(result.stderr)) {
                throw new Error(`${test.name} ${optimization}: ${result.error ?? result.signal ?? result.status}\n${result.stdout}\n${result.stderr}`);
            }
        }
    }
    console.log("Array checks passed (ASan/UBSan, O0/O2, stored bounds, recursive initialization, checked sizes).");
} finally {
    fs.rmSync(directory, { recursive: true, force: true });
}
