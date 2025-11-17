# StringField Minimal (laptop-only lesson sketch)

This folder is the "no excuses" entry point. It lets you narrate the StringField
mapping story with **zero hardware**, just Processing, a laptop trackpad, and a
room full of skeptics. Use it when someone says "sure, but what does the
computer *think* a pluck looks like?"

## What's inside

File | Purpose
---|---
`StringField_minimal.pde` | Processing sketch that mocks the sensor stream with mouse moves, keyboard taps, and slow random drift. Loud comments explain every mapping step.

## Teaching beats
- **Mouse-as-string:** Vertical position picks a virtual string, horizontal speed
decides how hard you "pluck." Press + drag to show it live.
- **Keyboard cheats:** Number keys `1-4` pluck the four strings with staged
velocities, `b` bows (slow energy), `s` scrapes (chaotic burst).
- **Noise narration:** The sketch injects tiny random fluctuations so you can
explain sensor noise/decay without touching hardware.
- **Mapping callouts:** Each rendering step is annotated with why it exists
(e.g., "energy" vs "velocity" vs "state"), so you can read the file out loud.

## Running it
1. Open the sketch in Processing 4.x (Java mode).
2. Hit **Run**. No libraries, no serial ports, no apology slides.
3. Follow the on-screen legend. Invite the class to shout gestures and drive the
keyboard for you.

> Punk-rock note: this is intentionally didactic. Keep it messy enough that
students feel invited to edit live.
