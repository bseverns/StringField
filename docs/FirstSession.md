# First Session (from plug‑in to projector in ~10 minutes)

You want the **fastest path to “it works”** without skipping the pedagogy. This is the first‑play ritual: a short, auditable chain from hardware → firmware → serial → bridge → visualizer. It’s written like a studio notebook, but it doubles as a teaching script you can read out loud.

## 0. Pre‑flight (2 minutes, no heroics)

- **Hardware on the table:** Sensor stack + Teensy/ESP32 board, USB cable, and whatever you’re using as the “string” (conductive tape, touch pad, ToF, etc.).
- **Software on the laptop:** PlatformIO + Processing installed. Keep it boring on purpose.
- **Why this matters:** You can’t teach sensor/gesture semantics if you’re still fighting drivers.

## 1. Connect hardware (30 seconds)

1. Plug the board into USB.
2. Connect the sensor stack per your build notes (or the default kit wiring in `docs/BuildKit.md`).
3. **Sanity check:** LEDs light, no smoke, no dangling ground.

## 2. Flash the firmware (2 minutes)

Pick the target you actually have in hand:

- **Teensy 4.0:** `pio run -d firmware -e teensy40 -t upload`
- **ESP32‑S3 DevKitC:** `pio run -d firmware -e esp32s3 -t upload`

If upload is clean, you’re already winning. If not, don’t panic—drivers and permissions are 90% of early friction.

## 3. Open a serial monitor (1 minute)

The firmware speaks JSON at **115200 baud**. Use whatever you like:

- `pio device monitor -b 115200`
- Any serial terminal you trust.

You should see boot chatter and JSON lines when you touch the sensor.

## 4. Run the OSC/Serial bridge (2 minutes)

1. Open `software/processing/OSCSerialBridge/OSCSerialBridge.pde` in Processing.
2. Hit **Run**.
3. Tap `n` if the wrong port is selected.

You’re now mirroring gesture JSON to OSC and getting the classroom overlay.

## 5. Open a visualizer (2 minutes)

Pick your flavor:

- **Processing:** `software/processing/StringFieldViz.pde`
- **p5.js:** `software/p5js/sketch.js` (if you want to run in a browser)

Run it and keep it side‑by‑side with the bridge. The point is to make the system legible for the room.

## Known‑good gesture packet (copy/paste test)

If you want a guaranteed “does the pipeline even work?” probe, paste this single line into the serial monitor:

```json
{"gesture":"pluck","value":96}
```

### Expected visual response

- **State name:** `PLUCK` (uppercase in the visualizer ticker)
- **Velocity:** ~`96/127` ≈ `0.76` (you’ll see the velocity bar jump and the history cell print the numeric value)

If the state name appears and the velocity jumps, the pipeline is alive. If not, check the serial port, baud rate, or whether the visualizer is listening to the right stream.

## Next move (if you’re teaching)

Once the room sees the overlay and hears the gesture in the synth, you can introduce **why** the mapping exists. That’s where the punk‑rock part kicks in: we’re not hiding the system, we’re showing it.
