# Classroom handouts: OSC/Serial Bridge

Print these and toss them on music stands. They keep the “punk‑rock but consent‑first” vibe while steering the class through the same flow the overlay shows.

## Quick consent prompts (say verbatim)
1. "Is everyone ok with us logging **gestures** for the next 5 minutes? The logger is `tools/serial_logger.py` writing CSV rows of the JSON packets—no audio."
2. "We'll name the file after this session and trash it if anyone wants it gone."
3. "You can watch the packets mirrored to OSC on the projector; nothing hidden."

## Serial logger cheat card
- Start: `python tools/serial_logger.py /dev/ttyACM0 115200 > logs/session.csv` (only add `--start` if consent is already recorded)
- Stop: `ctrl+c` when you're done. Announce the stop out loud.
- Share: show the log path on screen before closing Processing.

## Preset browser cameo (from the README)
- Show the JSON snippet: `{ "notes": [60, 63, 67, 70, 74] }`.
- Have a student paste it into the serial terminal (or logger terminal) mid‑lesson.
- Narrate: "No reflash needed—firmware echoes the set and the OSC bridge keeps forwarding gestures so you hear the new scale instantly."

## Run-of-show beats
- Warmup: tap `p` for a ping, `n` until you see the StringField port, and use the keyboard cheats in the visualization to explain pluck/bow/scrape labels.
- Consent: read the prompts above and only then start the logger.
- Play: forward gestures to your OSC synth, let the class predict the next state based on the ticker.
- Switch it up: use the preset browser JSON to swap note sets live; ask the room what changed.
- Debrief: stop logging, show the saved log path, and delete on request.
