// Colours Easy source for the editor. Follows the lexer in easyc.ts
// (keywords come from it directly) but never throws: an unterminated
// string or comment simply runs to the end of the text, so the editor
// stays coloured while the user is mid-edit.

import { KEYWORDS } from "../easyc.ts";

const TYPES = new Set(["INTEGER", "REAL", "BOOLEAN", "STRING"]);
const CONSTANTS = new Set(["TRUE", "FALSE"]);
const BUILTINS = new Set(["LENGTH", "CHARACTER", "SUBSTR", "FIX", "FLOAT"]);

export function escHtml(s: string): string {
    return s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

const span = (cls: string, text: string) => `<span class="${cls}">${escHtml(text)}</span>`;

// End index of a /* */ comment starting at `i`; nests like the lexer.
function blockCommentEnd(text: string, i: number): number {
    let depth = 0;
    while (i < text.length) {
        if (text.startsWith("/*", i)) {
            depth++;
            i += 2;
        } else if (text.startsWith("*/", i)) {
            i += 2;
            if (--depth === 0) return i;
        } else i++;
    }
    return text.length;
}

function stringEnd(text: string, i: number): number {
    i++;
    while (i < text.length) {
        if (text[i] === '"') {
            if (text[i + 1] === '"') i += 2;
            else return i + 1;
        } else i++;
    }
    return text.length;
}

const NUMBER = /\d+(?:[.eE][\d+\-eE]*)?/y;
const WORD = /[A-Za-z_][A-Za-z0-9_]*/y;

function matchAt(re: RegExp, text: string, i: number): string {
    re.lastIndex = i;
    return re.exec(text)![0];
}

export function highlightEasy(text: string): string {
    let out = "";
    let plain = 0;
    let i = 0;
    const emit = (end: number, cls: string) => {
        out += escHtml(text.slice(plain, i)) + span(cls, text.slice(i, end));
        i = plain = end;
    };
    while (i < text.length) {
        const c = text[i]!;
        if (c === "/" && text[i + 1] === "*") emit(blockCommentEnd(text, i), "hl-c");
        else if (c === "/" && text[i + 1] === "/") {
            const nl = text.indexOf("\n", i);
            emit(nl === -1 ? text.length : nl, "hl-c");
        } else if (c === '"') emit(stringEnd(text, i), "hl-s");
        else if (/\d/.test(c)) {
            emit(i + matchAt(NUMBER, text, i).length, "hl-n");
        } else if (/[A-Za-z_]/.test(c)) {
            const word = matchAt(WORD, text, i);
            const cls = TYPES.has(word)
                ? "hl-t"
                : CONSTANTS.has(word)
                  ? "hl-n"
                  : KEYWORDS.has(word)
                    ? "hl-k"
                    : BUILTINS.has(word)
                      ? "hl-p"
                      : "";
            if (cls) emit(i + word.length, cls);
            else i += word.length;
        } else i++;
    }
    out += escHtml(text.slice(plain));
    return out;
}
