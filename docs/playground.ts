import { compileToC, ParseError, LexerError, CompilerError, GenerateError } from "../easyc.ts";
import { BUILD_TIME } from "./build-info.ts";
import runtimeC from "../runtime.c" with { type: "text" };
import { CompileFailure, compileToWasm, runWasm } from "./run.ts";
import { highlightEasy } from "./highlight-easy.ts";

// The "Example" dropdown is populated from a runtime-loaded manifest.
// docs/examples.js defines `window.easyExamples` as
// `[{ name, filename, input? }, ...]` and is loaded by a classic `<script>`
// tag before this module runs. Each entry's `source` is a Promise kicked
// off immediately so tab-switching feels instant. `input`, when present,
// names a file in examples/ whose text prefills the "input" pane.
interface Example {
    name: string;
    filename: string;
    source: Promise<string>;
    input: Promise<string>;
    resolvedSource?: string;
}
interface ExampleManifestEntry {
    name: string;
    filename: string;
    input?: string;
}
declare global {
    interface Window {
        easyExamples?: ExampleManifestEntry[];
    }
}

const fetchExample = (f: string): Promise<string> =>
    fetch(`examples/${f}`).then((r) => r.text());

const EXAMPLES: Example[] = (window.easyExamples ?? []).map((e) => {
    const ex: Example = {
        name: e.name,
        filename: e.filename,
        source: fetchExample(e.filename),
        input: e.input ? fetchExample(e.input).catch(() => "") : Promise.resolve(""),
    };
    ex.source.then(
        (s) => {
            ex.resolvedSource = s;
            renderTabs();
        },
        () => {},
    );
    return ex;
});

function tabMatchesExample(t: Pick<Tab, "filename" | "source">): boolean {
    const ex = EXAMPLES.find((e) => e.filename === t.filename);
    return !!ex && ex.resolvedSource === t.source;
}

const TABS_KEY = "easy-playground:tabs";
const ACTIVE_KEY = "easy-playground:active";
const THEME_KEY = "easy-playground:theme";
const FORMAT_KEY = "easy-playground:format";
const DEFAULT_FILENAME = "program.easy";
const DEFAULT_EXAMPLE = "sieve";

type OutputFormat = "easy" | "c";
const OUTPUT_FORMATS: readonly OutputFormat[] = ["easy", "c"];
const DEFAULT_FORMAT: OutputFormat = "easy";

interface Tab {
    filename: string;
    source: string;
    // Standard input for the program, saved per tab.
    input: string;
}

let tabs: Tab[] = [];
let active = 0;

type Theme = "dark" | "light";

function applyTheme(t: Theme) {
    document.body.classList.toggle("theme-light", t === "light");
}

function loadTheme(): Theme {
    try {
        return localStorage.getItem(THEME_KEY) === "dark" ? "dark" : "light";
    } catch {
        return "light";
    }
}

function saveTheme(t: Theme) {
    try {
        localStorage.setItem(THEME_KEY, t);
    } catch {}
}

const source = document.getElementById("source") as HTMLTextAreaElement;
const cOut = document.getElementById("c-output") as HTMLPreElement;
const cTitle = document.getElementById("c-title") as HTMLDivElement;
const errorEl = document.getElementById("error") as HTMLDivElement;
const select = document.getElementById("example") as HTMLSelectElement;
const confirmModal = document.getElementById("confirm-modal") as HTMLDivElement;
const confirmMessage = document.getElementById("confirm-message") as HTMLParagraphElement;
const confirmOk = document.getElementById("confirm-ok") as HTMLButtonElement;
const confirmCancel = document.getElementById("confirm-cancel") as HTMLButtonElement;
const uploadBtn = document.getElementById("upload-btn") as HTMLButtonElement;
const downloadBtn = document.getElementById("download-btn") as HTMLButtonElement;
const downloadFormatSel = document.getElementById("download-format") as HTMLSelectElement;
const resetBtn = document.getElementById("reset") as HTMLButtonElement;
const themeBtn = document.getElementById("theme") as HTMLButtonElement;
const fileInput = document.getElementById("file-input") as HTMLInputElement;
const sourceModal = document.getElementById("source-modal") as HTMLDivElement;
const sourceModalContent = document.getElementById("source-modal-content") as HTMLPreElement;
const sourceModalClose = document.getElementById("source-modal-close") as HTMLButtonElement;
const filenameInput = document.getElementById("filename") as HTMLInputElement;
const tabsEl = document.getElementById("tabs") as HTMLDivElement;
const sourceHighlight = document.getElementById("source-highlight") as HTMLPreElement;
const runButton = document.getElementById("run") as HTMLButtonElement;
const stdinInput = document.getElementById("stdin") as HTMLTextAreaElement;
const runOutput = document.getElementById("run-output") as HTMLPreElement;
const runTitle = document.getElementById("run-title") as HTMLDivElement;

function easyName(): string {
    return filenameInput.value.trim() || DEFAULT_FILENAME;
}

function stem(name: string): string {
    return name.replace(/\.[^.]*$/, "") || name;
}

function outputName(format: OutputFormat): string {
    return `${stem(easyName())}.${format}`;
}

for (const ex of EXAMPLES) {
    const opt = document.createElement("option");
    opt.value = ex.name;
    opt.textContent = ex.name;
    select.appendChild(opt);
}

select.addEventListener("change", async () => {
    const ex = EXAMPLES.find((e) => e.name === select.value);
    if (!ex) return;
    const [exSource, exInput] = await Promise.all([ex.source, ex.input]);
    tabs[active]!.source = source.value;
    const uniqueName = uniqueFilename(ex.filename);
    tabs.push({ filename: uniqueName, source: exSource, input: exInput });
    active = tabs.length - 1;
    source.value = exSource;
    showTabRun();
    filenameInput.value = uniqueName;
    lastGoodName = uniqueName;
    source.scrollTop = 0;
    saveTabs();
    renderTabs();
    onChange();
    source.focus();
});

function uniqueFilename(base: string): string {
    if (!tabs.some((t, i) => i !== active && t.filename === base)) return base;
    const m = base.match(/^(.*?)(\.[^.]*)?$/);
    const s = m ? m[1]! : base;
    const ext = m && m[2] ? m[2] : "";
    let n = 2;
    while (tabs.some((t, i) => i !== active && t.filename === `${s}-${n}${ext}`)) n++;
    return `${s}-${n}${ext}`;
}

function deselectExample() {
    if (select.value) select.value = "";
}

source.addEventListener("input", deselectExample);
filenameInput.addEventListener("input", deselectExample);

let confirmResolver: ((ok: boolean) => void) | null = null;

function askConfirm(message: string): Promise<boolean> {
    confirmMessage.textContent = message;
    confirmModal.hidden = false;
    confirmOk.focus();
    return new Promise((resolve) => {
        confirmResolver = resolve;
    });
}

function closeConfirm(result: boolean) {
    confirmModal.hidden = true;
    const r = confirmResolver;
    confirmResolver = null;
    if (r) r(result);
}

confirmOk.addEventListener("click", () => closeConfirm(true));
confirmCancel.addEventListener("click", () => closeConfirm(false));
confirmModal.addEventListener("click", (e) => {
    if (e.target === confirmModal) closeConfirm(false);
});

window.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && !confirmModal.hidden) closeConfirm(false);
    if (e.key === "Enter" && !confirmModal.hidden) {
        e.preventDefault();
        closeConfirm(true);
    }
    if (e.key === "Escape" && !sourceModal.hidden) closeSourceModal();
    if (e.key === "Enter" && (e.ctrlKey || e.metaKey) && confirmModal.hidden) {
        e.preventDefault();
        runButton.click();
    }
});

function closeSourceModal() {
    sourceModal.hidden = true;
}

function openRuntimeModal() {
    sourceModalContent.innerHTML = highlightC(runtimeC);
    sourceModal.hidden = false;
}

sourceModalClose.addEventListener("click", closeSourceModal);
sourceModal.addEventListener("click", (e) => {
    if (e.target === sourceModal) closeSourceModal();
});

cOut.addEventListener("click", (e) => {
    const el = (e.target as HTMLElement).closest("[data-runtime-link]");
    if (!el) return;
    e.preventDefault();
    openRuntimeModal();
});

let lastC: string | null = null;

function setError(msg: string) {
    errorEl.textContent = msg;
    errorEl.classList.add("visible");
}

function clearError() {
    errorEl.textContent = "";
    errorEl.classList.remove("visible");
}

function formatCompileError(e: unknown): string {
    if (e instanceof ParseError) return String(e);
    if (e instanceof LexerError || e instanceof GenerateError || e instanceof CompilerError) {
        return e.message;
    }
    return (e as Error).message ?? String(e);
}

// A trailing newline in a <pre> adds no line box, so pad it to keep the
// overlay as tall as the textarea's content.
function renderSourceHighlight() {
    sourceHighlight.innerHTML = highlightEasy(source.value) + "\n";
    syncSourceScroll();
}

function syncSourceScroll() {
    sourceHighlight.scrollTop = source.scrollTop;
    sourceHighlight.scrollLeft = source.scrollLeft;
}

source.addEventListener("scroll", syncSourceScroll);

function runPipeline() {
    renderSourceHighlight();
    const src = source.value;
    const file = easyName();

    let cText: string;
    try {
        cText = compileToC(src, file);
    } catch (e) {
        lastC = null;
        cOut.textContent = "";
        cTitle.textContent = "output.c";
        setError(formatCompileError(e));
        updateDownloadEnabled();
        updateRunEnabled();
        return;
    }

    clearError();
    lastC = cText;
    cOut.innerHTML = highlightC(cText);
    const lines = cText.split(/\r?\n/).length;
    cTitle.textContent = `${stem(file)}.c — ${cText.length} B, ${lines} lines`;
    updateDownloadEnabled();
    updateRunEnabled();
}

const C_KEYWORDS = new Set([
    "int", "void", "char", "short", "long", "float", "double",
    "signed", "unsigned", "bool", "const", "static", "extern",
    "volatile", "inline", "register", "restrict", "auto",
    "if", "else", "for", "while", "do", "break", "continue",
    "return", "switch", "case", "default", "goto",
    "sizeof", "typeof", "struct", "union", "enum", "typedef",
    "_Bool", "_Atomic", "true", "false", "NULL",
]);
const C_TYPES = new Set([
    "STR", "ARRAY", "INTEGER", "REAL", "BOOLEAN", "STRING",
    "FILE", "size_t", "time_t",
]);

const C_TOKEN = /(\/\/[^\n]*)|(\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\])*")|('(?:\\.|[^'\\])*')|(^[ \t]*#[^\n]*)|(\b0x[0-9a-fA-F]+\b|\b\d+(?:\.\d+)?\b)|([A-Za-z_$][A-Za-z0-9_$]*)/gm;

function escHtml(s: string): string {
    return s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function highlightC(text: string): string {
    let out = "";
    let last = 0;
    C_TOKEN.lastIndex = 0;
    let m: RegExpExecArray | null;
    while ((m = C_TOKEN.exec(text))) {
        if (m.index > last) out += escHtml(text.slice(last, m.index));
        const full = m[0];
        if (m[1] || m[2]) out += `<span class="hl-c">${escHtml(full)}</span>`;
        else if (m[3] || m[4]) out += `<span class="hl-s">${escHtml(full)}</span>`;
        else if (m[5]) {
            if (/^\s*#\s*include\s+"runtime\.c"/.test(full)) {
                out += `<span class="hl-p hl-include" data-runtime-link="1" title="click to view runtime.c">${escHtml(full)}</span>`;
            } else {
                out += `<span class="hl-p">${escHtml(full)}</span>`;
            }
        }
        else if (m[6]) out += `<span class="hl-n">${escHtml(full)}</span>`;
        else if (m[7]) {
            if (C_KEYWORDS.has(full)) out += `<span class="hl-k">${escHtml(full)}</span>`;
            else if (C_TYPES.has(full)) out += `<span class="hl-t">${escHtml(full)}</span>`;
            else out += escHtml(full);
        }
        last = m.index + full.length;
    }
    out += escHtml(text.slice(last));
    return out;
}

// ---- run: C -> .wasm (in-browser clang) -> execute (WASI worker) ----

const RUN_HINT = "press run (Ctrl/⌘+Enter) to compile and execute in the browser";
let running: AbortController | null = null;

function setRunTitle(text: string, state: "" | "running" | "failed" = "") {
    runTitle.textContent = text;
    runTitle.className = "pane-title" + (state ? ` ${state}` : "");
}

function updateRunEnabled() {
    runButton.disabled = !running && lastC === null;
}

function setRunning(on: boolean) {
    runButton.textContent = on ? "stop" : "run";
    runButton.classList.toggle("running", on);
    updateRunEnabled();
}

// Output arrives in bursts from the worker; render once per frame and
// keep following the bottom unless the user has scrolled up.
let pendingOutput: [string, string][] = [];
let outputFrame = 0;

function appendOutput(kind: "stdout" | "stderr" | "note", text: string) {
    pendingOutput.push([kind, text]);
    outputFrame ||= requestAnimationFrame(flushOutput);
}

function flushOutput() {
    outputFrame = 0;
    const follow = runOutput.scrollHeight - runOutput.scrollTop - runOutput.clientHeight < 24;
    const fragment = document.createDocumentFragment();
    for (const [kind, text] of pendingOutput) {
        if (kind === "stdout") {
            fragment.append(text);
        } else {
            const span = document.createElement("span");
            span.className = kind;
            span.textContent = text;
            fragment.append(span);
        }
    }
    pendingOutput = [];
    runOutput.append(fragment);
    if (follow) runOutput.scrollTop = runOutput.scrollHeight;
}

function clearOutput() {
    pendingOutput = [];
    if (outputFrame) cancelAnimationFrame(outputFrame);
    outputFrame = 0;
    runOutput.textContent = "";
}

function noteOnNewLine(text: string) {
    flushOutput();
    const pre = runOutput.textContent ?? "";
    appendOutput("note", (pre && !pre.endsWith("\n") ? "\n" : "") + text);
}

// Called whenever the editor shows a different tab: stop any run and
// show that tab's input with an empty output pane.
function showTabRun() {
    const previous = running;
    running = null;
    previous?.abort();
    setRunning(false);
    stdinInput.value = tabs[active]?.input ?? "";
    clearOutput();
    setRunTitle("output");
    appendOutput("note", RUN_HINT);
}

stdinInput.addEventListener("input", () => {
    tabs[active]!.input = stdinInput.value;
    saveTabs();
});

async function run() {
    if (running) {
        running.abort();
        return;
    }
    if (lastC === null) return;
    const controller = new AbortController();
    const { signal } = controller;
    running = controller;
    setRunning(true);
    clearOutput();
    const c = lastC;
    const stdin = stdinInput.value;
    try {
        const wasm = await compileToWasm(c, (text) => setRunTitle(`output — ${text}`, "running"), signal);
        setRunTitle("output — running…", "running");
        const start = performance.now();
        const status = await runWasm(wasm, stdin, appendOutput, signal);
        const seconds = ((performance.now() - start) / 1000).toFixed(2);
        setRunTitle(`output — exit ${status} · ${seconds} s`, status === 0 ? "" : "failed");
    } catch (e) {
        if (signal.aborted) {
            if (running === controller) {
                setRunTitle("output — stopped", "failed");
                noteOnNewLine("stopped");
            }
        } else if (e instanceof CompileFailure) {
            setRunTitle("output — C compilation failed", "failed");
            appendOutput("stderr", e.message);
        } else {
            setRunTitle("output — failed", "failed");
            noteOnNewLine("");
            appendOutput("stderr", (e as Error).message ?? String(e));
        }
    } finally {
        if (running === controller) {
            running = null;
            setRunning(false);
        }
    }
}

runButton.addEventListener("click", () => void run());

function saveTabs() {
    try {
        localStorage.setItem(TABS_KEY, JSON.stringify(tabs));
        localStorage.setItem(ACTIVE_KEY, String(active));
    } catch {}
}

function save() {
    tabs[active]!.source = source.value;
    saveTabs();
}

function renderTabs() {
    tabsEl.innerHTML = "";
    tabs.forEach((t, i) => {
        const el = document.createElement("div");
        const live = i === active ? source.value : t.source;
        const matches = tabMatchesExample({ filename: t.filename, source: live });
        el.className = "tab" + (i === active ? " active" : "") + (matches ? " example" : " modified");
        el.title = t.filename;
        const name = document.createElement("span");
        name.textContent = t.filename || "(untitled)";
        el.appendChild(name);
        const close = document.createElement("button");
        close.type = "button";
        close.className = "close";
        close.textContent = "×";
        close.title = "close tab";
        close.addEventListener("click", (e) => {
            e.stopPropagation();
            void closeTab(i);
        });
        el.appendChild(close);
        el.addEventListener("click", () => switchTab(i));
        tabsEl.appendChild(el);
    });
    const add = document.createElement("button");
    add.type = "button";
    add.className = "tab-add";
    add.textContent = "+";
    add.title = "new tab";
    add.addEventListener("click", () => newTab());
    tabsEl.appendChild(add);
}

function nextUntitled(): string {
    let n = 1;
    while (tabs.some((t) => t.filename === `untitled-${n}.easy`)) n++;
    return `untitled-${n}.easy`;
}

function switchTab(i: number) {
    if (i === active || i < 0 || i >= tabs.length) return;
    tabs[active]!.source = source.value;
    active = i;
    source.value = tabs[active]!.source;
    showTabRun();
    filenameInput.value = tabs[active]!.filename;
    lastGoodName = tabs[active]!.filename;
    source.scrollTop = 0;
    saveTabs();
    renderTabs();
    deselectExample();
    runPipeline();
    source.focus();
}

function newTab() {
    tabs[active]!.source = source.value;
    tabs.push({ filename: nextUntitled(), source: "", input: "" });
    active = tabs.length - 1;
    source.value = "";
    showTabRun();
    filenameInput.value = tabs[active]!.filename;
    lastGoodName = tabs[active]!.filename;
    source.scrollTop = 0;
    saveTabs();
    renderTabs();
    deselectExample();
    runPipeline();
    source.focus();
}

async function closeTab(i: number) {
    const current = i === active ? source.value : tabs[i]!.source;
    const matchesExample = tabMatchesExample({
        filename: tabs[i]!.filename,
        source: current,
    });
    if (current.trim().length > 0 && !matchesExample) {
        const ok = await askConfirm(`Close "${tabs[i]!.filename}"? Its content will be lost.`);
        if (!ok) return;
    }
    if (tabs.length === 1) {
        tabs[0] = { filename: DEFAULT_FILENAME, source: "", input: "" };
        active = 0;
        source.value = "";
        filenameInput.value = tabs[0]!.filename;
        lastGoodName = tabs[0]!.filename;
    } else {
        tabs.splice(i, 1);
        if (active > i) active--;
        else if (active === i && active >= tabs.length) active = tabs.length - 1;
        source.value = tabs[active]!.source;
        filenameInput.value = tabs[active]!.filename;
        lastGoodName = tabs[active]!.filename;
    }
    showTabRun();
    saveTabs();
    renderTabs();
    deselectExample();
    runPipeline();
}

let lastGoodName = "";
filenameInput.addEventListener("focus", () => {
    lastGoodName = filenameInput.value;
});
filenameInput.addEventListener("input", () => {
    tabs[active]!.filename = filenameInput.value;
    saveTabs();
    renderTabs();
});
filenameInput.addEventListener("change", () => {
    const val = filenameInput.value.trim();
    const dup = tabs.findIndex((t, i) => i !== active && t.filename === val);
    if (!val || dup !== -1) {
        if (dup !== -1) alert(`A tab named "${val}" already exists.`);
        filenameInput.value = lastGoodName;
        tabs[active]!.filename = lastGoodName;
    } else {
        filenameInput.value = val;
        tabs[active]!.filename = val;
        lastGoodName = val;
    }
    saveTabs();
    renderTabs();
});

function onChange() {
    save();
    runPipeline();
    renderTabs();
}

source.addEventListener("input", onChange);

function downloadBlob(data: BlobPart, name: string, type: string) {
    const blob = new Blob([data], { type });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = name;
    document.body.appendChild(a);
    a.click();
    a.remove();
    URL.revokeObjectURL(url);
}

function loadFormat(): OutputFormat {
    try {
        const v = localStorage.getItem(FORMAT_KEY);
        if (v && (OUTPUT_FORMATS as readonly string[]).includes(v)) {
            return v as OutputFormat;
        }
    } catch {}
    return DEFAULT_FORMAT;
}

function saveFormat(f: OutputFormat) {
    try {
        localStorage.setItem(FORMAT_KEY, f);
    } catch {}
}

function selectedFormat(): OutputFormat {
    return downloadFormatSel.value as OutputFormat;
}

// .easy is always available (downloads source); .c requires a successful compile.
function updateDownloadEnabled() {
    const fmt = selectedFormat();
    if (fmt === "easy") {
        downloadBtn.disabled = false;
        return;
    }
    downloadBtn.disabled = lastC === null;
}

downloadFormatSel.value = loadFormat();
updateDownloadEnabled();
downloadFormatSel.addEventListener("change", () => {
    saveFormat(selectedFormat());
    updateDownloadEnabled();
});

downloadBtn.addEventListener("click", () => {
    const fmt = selectedFormat();
    if (fmt === "easy") {
        downloadBlob(source.value, easyName(), "text/plain");
        return;
    }
    if (lastC === null) return;
    downloadBlob(lastC, outputName("c"), "text/plain");
});

uploadBtn.addEventListener("click", () => fileInput.click());

resetBtn.addEventListener("click", async () => {
    const ok = await askConfirm("Reset the current tab to the default example? This replaces its content.");
    if (!ok) return;
    const def = EXAMPLES.find((e) => e.name === DEFAULT_EXAMPLE) ?? EXAMPLES[0];
    if (!def) return;
    const [defSource, defInput] = await Promise.all([def.source, def.input]);
    const uniqueName = uniqueFilename(def.filename);
    tabs[active] = { filename: uniqueName, source: defSource, input: defInput };
    source.value = defSource;
    showTabRun();
    filenameInput.value = uniqueName;
    lastGoodName = uniqueName;
    select.value = def.name;
    source.scrollTop = 0;
    saveTabs();
    renderTabs();
    onChange();
    source.focus();
});

fileInput.addEventListener("change", async () => {
    const f = fileInput.files?.[0];
    if (!f) return;
    const text = await f.text();
    const uniqueName = uniqueFilename(f.name);
    tabs.push({ filename: uniqueName, source: text, input: "" });
    active = tabs.length - 1;
    source.value = text;
    showTabRun();
    filenameInput.value = uniqueName;
    lastGoodName = uniqueName;
    source.scrollTop = 0;
    fileInput.value = "";
    saveTabs();
    renderTabs();
    onChange();
    source.focus();
});

const buildTimeEl = document.getElementById("build-time");
if (buildTimeEl && BUILD_TIME) buildTimeEl.textContent = BUILD_TIME;

themeBtn.addEventListener("click", () => {
    const next: Theme = document.body.classList.contains("theme-light") ? "dark" : "light";
    applyTheme(next);
    saveTheme(next);
});

applyTheme(loadTheme());

async function loadTabsFromStorage(): Promise<void> {
    try {
        const raw = localStorage.getItem(TABS_KEY);
        if (raw) {
            const parsed = JSON.parse(raw);
            if (Array.isArray(parsed) && parsed.length > 0) {
                tabs = parsed.map((t) => ({
                    filename: String(t.filename ?? DEFAULT_FILENAME),
                    source: String(t.source ?? ""),
                    input: String(t.input ?? ""),
                }));
                const a = Number(localStorage.getItem(ACTIVE_KEY) ?? 0) | 0;
                active = a < 0 || a >= tabs.length ? 0 : a;
                return;
            }
        }
    } catch {}
    const def = EXAMPLES.find((e) => e.name === DEFAULT_EXAMPLE) ?? EXAMPLES[0];
    const src = (await def?.source) ?? "";
    const input = (await def?.input) ?? "";
    const name = def?.filename ?? DEFAULT_FILENAME;
    tabs = [{ filename: name, source: src, input }];
    active = 0;
    saveTabs();
}

(async () => {
    await loadTabsFromStorage();
    source.value = tabs[active]!.source;
    showTabRun();
    filenameInput.value = tabs[active]!.filename;
    lastGoodName = tabs[active]!.filename;
    renderTabs();
    onChange();
})();
