# Easy compiler — project review

## Overall assessment: strong educational project, punches above its weight

For a self-described first compiler, this is polished and coherent. It compiles a non-trivial Algol/PL-family language end-to-end to C, has a real test harness, and runs meaningful programs (Life, Brainfuck, Mastermind, Rule 110, Quine, Hanoi, map colouring). 51 test folders with golden files is serious rigor for a personal project.

## What's good

- **Architecture is right for the goal.** Handwritten recursive-descent parser + tree-walking C emitter is exactly the clearest pedagogical path. Emitting C (rather than LLVM IR or assembly) keeps the focus on semantics, not codegen plumbing.
- **Test discipline.** Golden files per test cover tokens, AST, symbol table, generated C, and runtime output — diffs catch regressions across every compiler stage, not just end-to-end. Parallel workers, `--filter`, `--update` mode, and an optional second PEG parser run (`easy.peg` / `peg.ts`) as a cross-check are well thought out.
- **Realistic C output with sanitizers.** Tests compile with `-Wall -Wextra -Werror -std=c23 -fsanitize=address`, plus Docker for Linux MSAN. That's more hygiene than most hobby compilers.
- **Value semantics + one honest escape hatch.** Deep-copy-everywhere with dynamic arrays as the explicit exception (`runtime.c` `ARRAY`) is a defensible design choice, and it's openly called out in the README.
- **Clean runtime boundary.** `runtime.c` (314 lines) is tight: `$concat`, `$output`, `$index` bounds checks, string tracking via `string_list`, `AUTOFREE_ARRAY` cleanup attribute. The format-string dispatch (`i/r/b/s/S/A`) for concat/output is a nice small trick.
- **Grammar fidelity.** `GRAMMAR.md` mirrors the book's BNF, and the README is candid about what's not semantically supported (`EXTERNAL`, `NAME`, multiple `PROGRAM` segments).

## Weaknesses / things worth improving

- **`easyc.ts` is 2246 lines in a single file.** Splitting lexer / parser / semantic / emitter into modules would make it much more readable as a teaching artifact — right now the file's main audience (learners) has to scroll a lot.
- **String memory model leaks by design.** `make_string` appends every string to a global `string_list` freed only at `$exit`. Fine for short programs, but means long-running Easy programs grow unboundedly. A per-scope arena or refcount would be a nice next exercise.
- **Shallow-copy dynamic arrays are a correctness hazard**, not just a limitation — it silently breaks Easy's documented value semantics. Worth at least a runtime assert or a roadmap item.
- **PEG parser is unused for codegen.** It's tested but dead-weight for the toolchain. Either commit to replacing the RD parser or drop it to reduce surface area.
- **No CI config visible in the repo root** (only `just ci` → local). A GitHub Actions workflow running `just test-docker` would lock in the Linux/MSAN story.
- **Error recovery.** `ParseError` throws on first error — a real teaching compiler benefits from recovering to report multiple diagnostics per run.
- **No type-check pass surfaced as a stage.** A symbol table exists (`test.s` goldens), but a distinct semantic-analysis phase with its own error class would mirror textbook structure more cleanly.

## Bottom line

As an "I wanted to learn compilers" project, it's well past the usual stopping point — it has a working runtime, sanitizer-clean output, a broad test corpus including a Quine, and honest documentation. The main next steps are structural (module split, CI, dropping or committing to PEG) rather than foundational.
