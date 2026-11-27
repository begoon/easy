"""Mix the narration onto the recording: work/raw.webm -> easy-playground.mp4.

Each line from work/audio/ is placed at the time capture.ts started it
(work/timeline.json), so the voice stays in step with what is on screen
even when the compiler download takes longer or shorter than last time.
"""
import json
import subprocess

FADE = 0.4
OUTPUT = "easy-playground.mp4"

timeline = json.load(open("work/timeline.json"))
length = timeline["length"]
beats = timeline["beats"]

command = ["ffmpeg", "-v", "error", "-y", "-i", "work/raw.webm"]
for beat in beats:
    command += ["-i", f"work/audio/{beat['audio']}.wav"]
graph = (
    f"[0:v]fps=30,scale=1920:1080:flags=lanczos,"
    f"fade=t=in:st=0:d={FADE},fade=t=out:st={length - FADE:.3f}:d={FADE},format=yuv420p[v];"
)
for n, beat in enumerate(beats, start=1):
    graph += f"[{n}:a]aresample=48000,adelay={int(beat['at'] * 1000)}:all=1[a{n}];"
graph += "".join(f"[a{n}]" for n in range(1, len(beats) + 1))
graph += f"amix=inputs={len(beats)}:normalize=0,apad,atrim=0:{length:.3f}[a]"
command += [
    "-filter_complex", graph, "-map", "[v]", "-map", "[a]", "-t", f"{length:.3f}",
    "-c:v", "libx264", "-crf", "18", "-preset", "slow", "-r", "30",
    "-c:a", "aac", "-b:a", "160k", "-ac", "2", "-movflags", "+faststart", OUTPUT,
]
subprocess.run(command, check=True)
duration = subprocess.run(
    ["ffprobe", "-v", "error", "-show_entries", "format=duration", "-of", "csv=p=0", OUTPUT],
    check=True, capture_output=True, text=True,
).stdout.strip()
print(f"Wrote video/{OUTPUT}: {float(duration):.1f} s.")
