# OSC/Serial Bridge (Processing)

This bridge reads the firmware's JSON gesture packets over serial and forwards them to OSC targets while projecting classroom overlays. It stays in lockstep with the serial logger workflow in [`tools/serial_logger.py`](../tools/serial_logger.py) and borrows the preset browser narration from the README so your teaching script is consistent.

- **Main sketch:** `software/processing/OSCSerialBridge/OSCSerialBridge.pde`
- **OSC address:** `/stringfield/gesture` with `[gesture (string), value (0-127), normalized (0-1)]`
- **Shortcuts:** `n` = rotate serial port, `p` = send OSC ping, `h` = print handout beats in the console

## Setup
1. Install Processing 4.x and add the **oscP5** library (Sketch → Import Library → Add Library → search "oscP5").
2. Plug in the StringField firmware board (115200 baud JSON). Open the sketch and confirm Processing sees the port in the console (`Serial.list()`).
3. Point your synth/DAW to listen on the OSC port you want (default `127.0.0.1:9000`).
4. Optional: start the serial logger for an auditable trail once the class consents: `python tools/serial_logger.py --port /dev/ttyACM0 --baud 115200 --outfile logs/session.jsonl`.

## Running the bridge
1. Press **Run** in Processing. The overlay will announce the active serial port and OSC target. Tap `n` if you need to cycle ports.
2. Trigger gestures on hardware (or paste packets into the serial monitor) and watch the overlays mirror them. Each line with a `gesture` key forwards to OSC instantly.
3. Hit `p` to sanity‑check your OSC routing; it emits `/stringfield/ping` with a frame counter.
4. Project the sketch in class—the consent panel reminds everyone you're logging with `tools/serial_logger.py` and will delete on request.

## Teaching overlays
- **Consent + logging panel:** Lists the exact ask ("ok to log gestures for ~5 min?") and reminds you to name the file + delete on request. Explicitly references the serial logger so students know what is stored.
- **Handout beats:** Four printable bullets you can read aloud. They tell you to flip on the logger *after* consent, mirror packets to OSC, and run the README preset browser steps so students hear the scale change.
- **Status rail:** Shows the last gesture/value, current serial port + baud, and OSC host/port so you can narrate the routing without tabbing away.

## OSC mapping
- `/stringfield/gesture` payload: `gesture` (string), `value` (0-127), `normalized` (float 0-1).
- `/stringfield/ping` payload: single int frame counter (for routing checks).

## How it aligns with the preset browser
The overlay text explicitly calls out the README runtime note‑set browser. While demoing, have a student paste `{ "notes": [60, 63, 67, 70, 74] }` into the serial monitor (or the logger terminal). The firmware will echo the new set, and the bridge will continue forwarding gesture packets to OSC, making the mapping change audible without a reflash.

## Troubleshooting
- **No serial ports listed:** unplug/replug the board or verify `udev` permissions. The status line will say "No serial ports found" until one appears.
- **OSC not arriving:** press `p` and confirm your synth shows a ping. If not, double‑check host/port and firewall rules.
- **Malformed packets:** the bridge shrugs off lines without a `gesture` key; they won't crash a workshop.
