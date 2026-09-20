// Run with: bun test-ownership.ts
import * as fs from "node:fs";
import * as os from "node:os";
import * as path from "node:path";
import { execFileSync } from "node:child_process";
import { compileToC } from "./easyc.ts";

const directory = fs.mkdtempSync(path.join(os.tmpdir(), "easy-ownership-"));
try {
    const source = fs.readFileSync("tests/string_ownership/test.easy", "utf8");
    const expected = fs.readFileSync("tests/string_ownership/x/test.output", "utf8");
    const cases: { name: string; source: string; expected: string; input?: string }[] = [
        { name: "normal", source, expected },
        {
            name: "input",
            source: `PROGRAM InputOwnership:
                DECLARE (s, saved) STRING;
                SET s := CHARACTER(65);
                SET saved := s;
                INPUT s;
                INPUT s;
                OUTPUT s || saved;
            END PROGRAM InputOwnership;`,
            input: "first second\n",
            expected: "secondA\n",
        },
        { name: "exit", source: source.replace("END PROGRAM Ownership;", "EXIT; END PROGRAM Ownership;"), expected },
        {
            name: "nested-exit",
            source: `PROGRAM Stop:
                DECLARE s STRING;
                PROCEDURE stop(value STRING):
                    DECLARE local STRING;
                    SET local := value || CHARACTER(66);
                    BEGIN
                        DECLARE n INTEGER;
                        SET n := 2;
                        BEGIN
                            DECLARE values ARRAY [0:n] OF STRING;
                            SET values[0] := local;
                            EXIT;
                        END;
                    END;
                END PROCEDURE stop;
                SET s := CHARACTER(65);
                CALL stop(s || CHARACTER(67));
            END PROGRAM Stop;`,
            expected: "",
        },
    ];
    for (const test of cases) {
        // Check bounded live memory at each loop iteration, not merely at exit.
        // The runtime's EASY_CHECK_OWNERSHIP checks both counters reach zero.
        const code = compileToC(test.source, `${test.name}.easy`).replace(
            /while \(1\)\n(\s*)\{/g,
            "while (1)\n$1{\n$1    assert(live_strings < 32); assert(live_arrays < 8);",
        );
        const cFile = path.join(directory, `${test.name}.c`);
        const executable = path.join(directory, test.name);
        fs.writeFileSync(cFile, code);
        for (const optimization of ["-O0", "-O2"]) {
            execFileSync("clang", ["-std=c23", "-Wall", "-Wextra", "-Werror", optimization,
                "-fsanitize=address,undefined", "-DEASY_CHECK_OWNERSHIP", "-I", process.cwd(),
                cFile, "-o", executable], { stdio: "inherit" });
            const actual = execFileSync(executable, { encoding: "utf8", input: test.input });
            if (actual !== test.expected) throw new Error(`${test.name}: unexpected output ${JSON.stringify(actual)}`);
        }
    }
    console.log("Ownership checks passed (ASan/UBSan, O0/O2, bounded live allocations, clean shutdown).");
} finally {
    fs.rmSync(directory, { recursive: true, force: true });
}
