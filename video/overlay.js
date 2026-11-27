// Injected into the playground by capture.ts: a spotlight with a label,
// full-screen title cards, a visible cursor and a stand-in for the example
// menu. Every helper returns a Promise that settles when its animation ends.
window.demo = (() => {
  const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));
  const element = (cls, parent) => {
    const node = document.createElement("div");
    node.className = cls;
    parent.append(node);
    return node;
  };

  const layer = element("", document.body);
  layer.id = "demo-layer";
  const spot = element("demo-spot", layer);
  const label = element("demo-label", layer);
  const card = element("demo-card", layer);
  const menu = element("demo-menu", layer);
  const cursor = element("demo-cursor", layer);
  cursor.innerHTML =
    '<svg viewBox="0 0 24 24" width="26" height="26"><path d="M3 2 L3 19 L7.5 14.5 L10.5 21.5 L13.5 20.2 L10.6 13.4 L17 13.4 Z" fill="#fff" stroke="#111" stroke-width="1.4" stroke-linejoin="round"/></svg>';

  const PAD = 6;
  let cursorAt = { x: 640, y: 420 };

  function bounds(selectors) {
    const rects = selectors.map((s) => document.querySelector(s).getBoundingClientRect());
    const left = Math.min(...rects.map((r) => r.left));
    const top = Math.min(...rects.map((r) => r.top));
    const right = Math.max(...rects.map((r) => r.right));
    const bottom = Math.max(...rects.map((r) => r.bottom));
    return { left, top, right, bottom, width: right - left, height: bottom - top };
  }

  let follow = null;

  function placeLabel(x, y, w, h) {
    // Inside the box near its top for big areas, beside or below small ones.
    const big = h > 160;
    const labelWidth = label.offsetWidth;
    let lx = big ? x + 18 : x + w + 14;
    let ly = big ? y + 18 : y + h / 2 - 19;
    if (lx + labelWidth > innerWidth - 12) {
      lx = Math.max(12, Math.min(x + w, innerWidth - 12) - labelWidth);
      ly = y + h + 12;
    }
    Object.assign(label.style, { left: `${lx}px`, top: `${ly}px` });
  }

  // `text` starting with "@" mirrors the boxed element's own text live
  // (minus anything up to an em dash), e.g. the run status line.
  async function box(selectors, text) {
    follow?.disconnect();
    follow = null;
    const r = bounds(selectors);
    const x = Math.max(4, r.left - PAD);
    const y = Math.max(4, r.top - PAD);
    const w = Math.min(innerWidth - 4 - x, r.right + PAD - x);
    const h = Math.min(innerHeight - 4 - y, r.bottom + PAD - y);
    Object.assign(spot.style, { left: `${x}px`, top: `${y}px`, width: `${w}px`, height: `${h}px` });
    spot.classList.add("on");
    if (text.startsWith("@")) {
      const source = document.querySelector(selectors[0]);
      const update = () => {
        label.textContent = (source.textContent ?? "").replace(/^.*?—\s*/, "");
        placeLabel(x, y, w, h);
      };
      follow = new MutationObserver(update);
      follow.observe(source, { childList: true, characterData: true, subtree: true });
      update();
    } else {
      label.textContent = text;
      placeLabel(x, y, w, h);
    }
    label.classList.add("on");
    await sleep(650);
  }

  async function clear() {
    follow?.disconnect();
    follow = null;
    spot.classList.remove("on");
    label.classList.remove("on");
    await sleep(400);
  }

  async function showCard(title, subtitle) {
    card.innerHTML = "";
    const h1 = document.createElement("h1");
    h1.textContent = title;
    const p = document.createElement("p");
    p.textContent = subtitle;
    card.append(h1, p);
    card.classList.add("on");
    await sleep(500);
  }

  async function hideCard() {
    card.classList.remove("on");
    await sleep(500);
  }

  function centre(selector) {
    const r = document.querySelector(selector).getBoundingClientRect();
    return { x: r.left + r.width / 2, y: r.top + r.height / 2 };
  }

  async function moveTo(selector) {
    cursorAt = centre(selector);
    cursor.style.opacity = "1";
    // The arrow's tip is at (3, 2) in its 24-unit box.
    cursor.style.transform = `translate(${cursorAt.x - 3}px, ${cursorAt.y - 2}px)`;
    await sleep(750);
  }

  async function click() {
    const ripple = element("demo-ripple", layer);
    Object.assign(ripple.style, { left: `${cursorAt.x}px`, top: `${cursorAt.y}px` });
    setTimeout(() => ripple.remove(), 600);
    await sleep(250);
  }

  // Opens a look-alike of the example menu, steps down to `name`, then
  // makes the real selection through the <select>'s change event.
  async function pick(name) {
    await moveTo("#example");
    await click();
    const names = (window.easyExamples ?? []).map((e) => e.name);
    const at = names.indexOf(name);
    const first = Math.max(0, Math.min(at - 3, names.length - 8));
    const shown = names.slice(first, first + 8);
    menu.innerHTML = "";
    const rows = shown.map((n) => {
      const row = document.createElement("div");
      row.textContent = n;
      menu.append(row);
      return row;
    });
    const r = document.querySelector("#example").getBoundingClientRect();
    Object.assign(menu.style, { left: `${r.left}px`, top: `${r.bottom + 2}px` });
    menu.classList.add("on");
    await sleep(350);
    const target = rows[shown.indexOf(name)];
    const rowBox = target.getBoundingClientRect();
    cursorAt = { x: rowBox.left + 40, y: rowBox.top + rowBox.height / 2 };
    cursor.style.transform = `translate(${cursorAt.x - 3}px, ${cursorAt.y - 2}px)`;
    await sleep(500);
    target.classList.add("hit");
    await sleep(350);
    await click();
    menu.classList.remove("on");
    const select = document.querySelector("#example");
    select.value = name;
    select.dispatchEvent(new Event("change"));
    await sleep(700);
  }

  return { box, clear, card: showCard, uncard: hideCard, moveTo, click, pick };
})();
