"""Speak each beat of script.json with the local Kokoro model.

Writes work/audio/<n>.wav and work/audio/durations.json, which capture.ts
reads so the recording waits for each line to finish.
"""
import json
import os

import soundfile as sf
from kokoro_onnx import Kokoro

script = json.load(open("script.json"))
os.makedirs("work/audio", exist_ok=True)
kokoro = Kokoro(".cache/kokoro-v1.0.onnx", ".cache/voices-v1.0.bin")
durations = []
for index, beat in enumerate(script["beats"]):
    samples, rate = kokoro.create(beat["say"], voice=script["voice"], speed=script["speed"], lang=script["lang"])
    sf.write(f"work/audio/{index}.wav", samples, rate)
    durations.append(len(samples) / rate)
json.dump(durations, open("work/audio/durations.json", "w"))
words = sum(len(beat["say"].split()) for beat in script["beats"])
print(f"Narrated {len(durations)} beats: {words} words, {sum(durations):.1f} s of speech.")
