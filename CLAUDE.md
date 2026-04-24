# easy — compiler notes for Claude

An educational compiler for **Easy**, a small Algol/PL-family language from
Wetherell's *Etudes for Programmers* (1978). Written in TypeScript, emits C,
which Clang/GCC then compiles to a native binary. The `runtime.c` file is the
bare-minimum runtime.

## Layout

- `easyc.ts` — single-file compiler (lexer + recursive-descent parser + tree-
  walking C emitter). Entry is `run(argv)` for the CLI; `compileToC(source,
  filename)` is the pure function used by the playground.
- `peg.ts` + `easy.peg` — experimental PEG parser, run as a cross-check by
  `test.ts` but not used for codegen.
- `runtime.c` — ~300 lines of C runtime (`STR`, `ARRAY`, `$concat`, `$output`,
  `$index`, `string_list`/`AUTOFREE_ARRAY` cleanup).
- `test.ts` — parallel golden-file test harness over `tests/*/`.
- `tests/<name>/test.easy` — source; `tests/<name>/x/*.*` — goldens
  (`test.c`, `test.output`, optional `test.tokens`, `test.json`, `test.s`,
  `test.peg.json`, `test.input`).
- `docs/` — browser playground (static files + bundled `playground.js`).
- `docs/examples/*.easy` — mirror of every `tests/*/test.easy`, fetched at
  runtime by the playground.

## Common commands

- `just` / `just test-compiler` — full test suite (compile + run + golden diff).
- `just quick` — compile-only, skips running the produced binaries.
- `just update` — refresh goldens (`UPDATE=1`).
- `just run NAME` — compile and execute `tests/NAME/test.easy`.
- `just test-docker` — Linux/MSAN container run.
- `just playground` — build the bundle and serve `docs/` on
  `http://localhost:8000`.
- `bun run build:playground` — rebuild `docs/playground.js` only.

## Playground architecture

- `docs/playground.ts` imports `compileToC` from `../easyc.ts` directly.
  `bun build --target=browser` bundles both into `docs/playground.js`.
- `easyc.ts` still imports `node:fs` / `node:path` / `node:process` /
  `node:child_process`; those only fire inside `run()`, so the browser bundle
  loads cleanly (bun stubs them). Do not add top-level code that actually
  calls into those modules.
- `docs/examples.js` is a plain `<script>` loaded at runtime — editing the
  example list does **not** require rebuilding the bundle. Example source
  files live in `docs/examples/` and are fetched on demand.
- Tabs, theme, and filename are persisted in `localStorage` under
  `easy-playground:*` keys.

## Conventions that matter

- Value semantics everywhere — structs, strings, fixed-size arrays are
  deep-copied on assignment, argument pass, and return. **Dynamic arrays
  (size unknown at compile time) are the one exception**: shallow-copied,
  called out in the README.
- Strings are tracked in a global `string_list` and freed at `$exit`. Fine
  for short programs; don't expect long-running Easy programs to be tight.
- Test goldens live in `tests/<name>/x/`; `.gitignore` keeps the sibling
  `tests/<name>/*.c|*.s|*.tokens|...` build artifacts out of git but
  whitelists the `x/` directory.
- `EXTERNAL` / `NAME` / multiple `PROGRAM` segments are parsed but not
  semantically supported — see the README.

## Before editing `easyc.ts`

- The file is ~2250 lines and single-module by intent (pedagogical). If you
  add exports for external consumers (e.g., the playground), keep the
  top-level import set node-only; defer anything browser-hostile into
  `run()`.
- The parser throws on the first error (`ParseError`). Don't silently swallow
  them — the playground relies on their `toString()` formatting.
