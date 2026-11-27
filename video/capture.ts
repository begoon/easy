// Records the playground demo to work/raw.webm and writes work/timeline.json
// (when each narration line starts). It serves ../docs locally and drives it
// in headless Chrome with a fresh profile, so the first run really downloads
// the C compiler from jsDelivr. Each beat of script.json starts its line and
// its actions together, and lasts as long as the slower of the two.

import { mkdir, readFile, writeFile } from "node:fs/promises";
import puppeteer, { type Page } from "puppeteer-core";

type Action =
    | ["card", string, string]
    | ["uncard"]
    | ["box", string[], string]
    | ["clear"]
    | ["cursor", string]
    | ["pick", string]
    | ["run"]
    | ["finish"]
    | ["hold", number];
type Script = { beats: { scene: string; say: string; do: Action[] }[] };

const LEAD = 0.35; // seconds from a beat's start to its first word
const GAP = 0.3; // silence after a line before the next beat
const INTRO = 0.8; // still frame before the first beat
const TAIL = 1.0; // still frame after the last beat

const dir = import.meta.dir;
const docs = `${dir}/../docs`;
const script: Script = JSON.parse(await readFile(`${dir}/script.json`, "utf8"));
const durations: number[] = JSON.parse(await readFile(`${dir}/work/audio/durations.json`, "utf8"));
if (durations.length !== script.beats.length) throw new Error("narration is stale: run narrate.py");

const server = Bun.serve({
    hostname: "127.0.0.1",
    port: 0,
    async fetch(request) {
        const path = new URL(request.url).pathname;
        const file = Bun.file(`${docs}${path.endsWith("/") ? `${path}index.html` : path}`);
        return (await file.exists()) ? new Response(file) : new Response("not found", { status: 404 });
    },
});

const sleep = (seconds: number) => new Promise((resolve) => setTimeout(resolve, seconds * 1000));

async function act(page: Page, action: Action) {
    const [name, ...args] = action;
    switch (name) {
        case "card":
            return page.evaluate((t, s) => demo.card(t, s), args[0] as string, args[1] as string);
        case "uncard":
            return page.evaluate(() => demo.uncard());
        case "box":
            return page.evaluate((s, t) => demo.box(s, t), args[0] as string[], args[1] as string);
        case "clear":
            return page.evaluate(() => demo.clear());
        case "cursor":
            return page.evaluate((s) => demo.moveTo(s), args[0] as string);
        case "pick":
            return page.evaluate((n) => demo.pick(n), args[0] as string);
        case "run":
            await page.evaluate(() => demo.moveTo("#run"));
            await page.evaluate(() => demo.click());
            return page.click("#run");
        case "finish": {
            await page.waitForFunction(
                () => /exit|failed|stopped/.test(document.querySelector("#run-title")!.textContent ?? ""),
                { timeout: 300_000, polling: 100 },
            );
            const title = await page.$eval("#run-title", (e) => e.textContent ?? "");
            if (!/exit 0/.test(title)) throw new Error(`run did not succeed: ${title}`);
            return;
        }
        case "hold":
            return sleep(args[0] as number);
    }
}

await mkdir(`${dir}/work`, { recursive: true });
const browser = await puppeteer.launch({
    executablePath: process.env.CHROME_PATH ?? "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
    headless: true,
    args: ["--hide-scrollbars"],
});
try {
    const page = await browser.newPage();
    page.on("pageerror", (error) => console.error("page error:", error));
    // Laid out at 1280x720 and captured at 1.5x: 1920x1080 frames.
    await page.setViewport({ width: 1280, height: 720, deviceScaleFactor: 1.5 });
    await page.goto(`http://127.0.0.1:${server.port}/`, { waitUntil: "networkidle0" });
    await page.waitForFunction(() => (document.querySelector("#c-output")?.textContent ?? "").length > 100);
    await page.addStyleTag({ path: `${dir}/overlay.css` });
    await page.addScriptTag({ path: `${dir}/overlay.js` });

    const recorder = await page.screencast({ path: `${dir}/work/raw.webm`, fps: 30 });
    const start = performance.now();
    const now = () => (performance.now() - start) / 1000;
    const beats: { audio: number; at: number }[] = [];
    await sleep(INTRO);
    for (const [index, beat] of script.beats.entries()) {
        const begin = now();
        beats.push({ audio: index, at: begin + LEAD });
        console.log(`${begin.toFixed(1).padStart(5)} s  [${beat.scene}] ${beat.say}`);
        for (const action of beat.do) await act(page, action);
        await sleep(Math.max(0, begin + LEAD + durations[index]! + GAP - now()));
    }
    await sleep(TAIL);
    const length = now();
    await recorder.stop();
    await writeFile(`${dir}/work/timeline.json`, JSON.stringify({ length, beats }, null, 2));
    console.log(`Recorded ${length.toFixed(1)} s.`);
} finally {
    await browser.close();
    server.stop(true);
}

declare const demo: {
    card(title: string, subtitle: string): Promise<void>;
    uncard(): Promise<void>;
    box(selectors: string[], text: string): Promise<void>;
    clear(): Promise<void>;
    moveTo(selector: string): Promise<void>;
    click(): Promise<void>;
    pick(name: string): Promise<void>;
};
