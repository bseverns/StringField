# Sensing Options Survey

| Modality | Parts (examples) | Signal shape | Pros | Cons | Notes |
| --- | --- | --- | --- | --- | --- |
| **Optical (reflective IR)** | TCRT5000/TSSP77038 + emitter | intensity vs. distance | cheap, robust, fast | ambient light, surface albedo | add modulated light + bandpass; shield from stray IR |
| **Time‑of‑Flight** | VL53L0X/L1X | mm‑range distance | linear, small | limited range/angles | great for “bow speed”; may need averaging |
| **Capacitive (single‑wire)** | wire + high‑Z ADC or dedicated IC (MPR121/AT42QT)\*\* | delta‑C (touch/proximity) | invisible “string”, low parts count | noise, grounding issues | guard traces; body grounding assumptions explicit |
| **MaKey‑style touch‑to‑ground** | alligator lead to body/foil | on/off or resistance | ultra‑quick protos, expressive taps | depends on player’s body ground | teach safety & signal path explicitly |
| **Piezo / Contact mic** | piezo disk + bias | onset‑rich transients | great for pluck/scrape | fragile mount, needs clamping & rectifier | diode clamp + RC envelope |
| **Hall / Flex (later)** | A3144 hall + magnet, flex sensor | position/pressure proxy | alternate mappings | mechanical complexity | optional expansion |

> Implementation note: All sensor paths conform to a common `Sensor` interface so gesture & mapping code doesn’t change when you swap hardware.

## Sensor field notes (fast links)

Each of these is a short, hands‑on guide with wiring notes, RC recipes, calibration rituals, and “what the gestures should feel like.”

- Start here for the index: [Sensor Field Notes](Sensors/README.md)
- [Optical Reflective IR](Sensors/OpticalReflective.md)
- [Capacitive Single‑Wire](Sensors/CapacitiveSingleWire.md)
- [MaKey‑style Touch‑to‑Ground](Sensors/MakeyTouch.md)
- [Time‑of‑Flight Proximity](Sensors/TimeOfFlight.md)
- [Piezo / Contact Mic](Sensors/Piezo.md)
- [PIR Motion](Sensors/PIR.md)
- [Electret Mic (Analog)](Sensors/ElectretMic.md)
- [I²S Digital Mic](Sensors/I2SMic.md)
