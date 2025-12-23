# Touch-to-Ground Tuning Kit

_A field notebook for the "touch a penny, play a string" build. Punk-rock lab notes meet teachable troubleshooting._

## Intent

- Let educators walk a class through touch-to-ground sensing without frying patience or gear.
- Offer swappable resistor/capacitor combos tuned for different room moisture levels.
- Provide a spoken-aloud troubleshooting flow so learners hear the logic, not just the fix.

## Quick Bill of Materials (per string/contact)

| Component | Humid Room Default | Dry Lab Default | Notes |
| --- | --- | --- | --- |
| Sense electrode | Copper tape square (2–3 cm) | Same | Clean with isopropyl before each class. |
| Return electrode | Alligator to large foil plate | Same | Tape to table leg for less trip hazard. |
| Bias resistor | 2.2 MΩ | 4.7 MΩ | Larger value = more sensitivity, more noise. |
| Series resistor (guard) | 220 Ω | 470 Ω | Limits inrush when students touch ground. |
| Shunt capacitor | 10 nF film | 4.7 nF film | Forms RC filter with bias resistor. |
| Reference ground | Instrument sleeve to earth ground | Battery-powered synth case | Keep it consistent across the room. |

## Recommended RC Combos

| Target vibe    | Room humidity | Bias R | Cshunt | RC time constant | Notes                                  |
| -------------- | ------------- | ------ | ------ | ---------------- | -------------------------------------- |
| Crisp plucks   | 30–45%        | 2.2 MΩ | 4.7 nF | ≈10 ms           | Fast decay, great for articulations.   |
| Legato bows    | 45–60%        | 3.3 MΩ | 10 nF  | ≈33 ms           | Gives a lazy envelope that loves CC1.  |
| Grainy scrapes | 60%+          | 4.7 MΩ | 22 nF  | ≈103 ms          | Embrace the noise, ride the threshold. |

> Rule of thumb: double the resistor when the room gets drier; halve the capacitor when you can see condensation.

## Troubleshooting Flow (Humid Room)

```
Start → Quiet output?
  ├─ No → Celebrate; log the combo.
  └─ Yes → Check bias resistor solder.
        ├─ Solid → Swap to lower-value bias (e.g., 1 MΩ).
        └─ Sketchy → Reflow / tighten alligator clip.
              └─ Still dead? → Dry the electrode with a fan, retry.
```

## Troubleshooting Flow (Dry Lab)

```
Start → Wild jitter?
  ├─ No → You're golden; document the room temp + humidity.
  └─ Yes → Add/Increase shunt cap (up to 22 nF).
        ├─ Still jittery → Touch a grounded laptop chassis; if noise drops, improve earth ground.
        └─ Stable-ish → Raise bias resistor (up to 10 MΩ) and retune firmware thresholds.
```

## Classroom Narration Prompts

- “When I say **pluck**, you say **NoteOn velocity burst**—that's literally the MIDI packet we fire.”
- “Bow gestures feed the **CC1 envelope**; ask students to map it to tremolo or filter cut-off and listen for expressive range.”
- “Scrape is our friendly glitch: micro-notes at ~50 velocity. Notice how the RC combo above shapes how gritty it feels.”

## Field Notes Template

| Date | Room | Humidity (%) | RC combo | Threshold tweaks | Surprises |
| ---- | ---- | ------------ | -------- | ---------------- | --------- |
|      |      |              |          |                  |           |

Fill this after each workshop. Punk-rock discipline is still discipline.
