# Playground demo video

`easy-playground.mp4` (about 73 s, 1920x1080) is a narrated demo of the web
playground: a roundup of its panes, then sieve, bf, fizzbuzz and rule_110
loaded from the example menu and run. Rebuild it with:

    just video

The video is not committed (`.gitignore`). It is on YouTube as an unlisted
video: https://youtu.be/pCCgxgRruZQ (a rebuild does not update that copy; it
has to be uploaded again).

## What the recipe does

1. `just build-playground` rebuilds `docs/`, so the video shows the current
   playground.
2. The Kokoro voice model (about 350 MB) is downloaded once into `.cache/`.
3. `narrate.py` speaks each beat of `script.json` with that model (voice
   `bm_george`) into `work/audio/<n>.wav`, plus `durations.json`. Nothing
   is sent to a cloud service.
4. `capture.ts` serves `../docs` locally and drives it in headless Chrome
   with a fresh profile, recording `work/raw.webm`. Each beat starts its
   line and its actions together and lasts as long as the slower of the two,
   so the first run shows the real, cold download of the C compiler from
   jsDelivr. It writes the moment each line starts to `work/timeline.json`.
5. `build.py` places each line at that moment, mixes them onto the
   recording, adds fades and encodes `easy-playground.mp4` with ffmpeg.

Requires Bun, uv, ffmpeg, Google Chrome (`CHROME_PATH` overrides its
location) and network access (jsDelivr, and the model on first use). The
length varies by a second or two with download speed.

## Changing the video

| To change | Edit |
| --- | --- |
| What is said | `say` in `script.json` |
| What happens during a line | `do` in `script.json` (actions below) |
| Voice or speaking speed | `voice`, `speed` in `script.json` |
| Spotlight, labels, cards, cursor, example menu | `overlay.css`, `overlay.js` |
| Pauses between lines | `LEAD`, `GAP`, `INTRO`, `TAIL` in `capture.ts` |
| Fades, encoding | `build.py` |

Actions, run in order while the line plays:

| Action | Effect |
| --- | --- |
| `["card", title, subtitle]` / `["uncard"]` | full-screen title card on / off |
| `["box", [selectors], label]` | spotlight around the elements with a label; a label of `"@"` mirrors the element's live text (the run status) |
| `["clear"]` | remove the spotlight |
| `["cursor", selector]` | move the cursor onto an element |
| `["pick", name]` | open the example menu and choose `name` |
| `["run"]` | click run |
| `["finish"]` | wait until the run ends; fails the build unless it exits 0 |
| `["hold", seconds]` | wait |

Headless Chrome does not draw the native `<select>` popup, so `pick` shows a
look-alike menu, then sets the real `<select>`; everything else is the real
playground. The overlay is injected only into the recording and never ships
with the site.
