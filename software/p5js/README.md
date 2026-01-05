# StringField p5.js Gesture Debugger

This sketch is a loud, opinionated teaching demo: *data in → motion out*. It
turns StringField gesture JSON into giant state labels, velocity vectors, and a
noisy “projector” layer so the room can see the signal breathe.

If you’ve got hardware, this is your oscilloscope-with-attitude. If you don’t,
smash the keyboard shortcuts and still get the full show.

## Browser + hardware requirements

WebSerial is the gatekeeper. That means **Chrome, Edge, or any Chromium-based
browser**. Firefox and Safari still don’t speak WebSerial (as of today), so the
button will politely sulk.

You also need:

- A secure context: `localhost` or `https://` (no `file://` — browsers hate
  that).
- A device that emits JSON lines over serial at **115200 baud**.

## Connect to the device (exact steps)

1. **Serve the sketch** from a local web server.
   - Example: from the repo root run `python3 -m http.server 8000`, then visit
     `http://localhost:8000/software/p5js/`.
2. **Open the sketch** in Chrome/Edge.
3. **Click “Connect Serial.”**
4. **Choose your device** in the browser’s port picker.
5. **Watch the status readout** flip to `serial: connected` and the visuals come
   alive.

To disconnect, click the same button again (or yank the cable — the sketch will
announce the drama).

## No hardware? Still party.

Keyboard shortcuts are *intentionally* kept alive for classrooms and demos:

- `P` → pluck
- `S` → scrape
- `B` → bow
- `R` → release
- `H` → harmonic
- `M` → mute
- `T` → tremolo
- `V` → vibrato

These trigger the same rendering pipeline as real serial messages, so your
teaching flow doesn’t depend on cables behaving.
