# Play Paradigms (pluck / bow / scrape)

> Treat these as *verbs* the sensor engine tries to name. Implementations should keep thresholds & state machines explicit in code.

## Pluck
- Rapid threshold crossing + short decay → **Note On**; release on return/pause.
- Parameters: **attack velocity proxy**, **dampen time**, **retrigger guard**.

## Bow
- Sustained near‑threshold energy; direction from differential (e.g., two sensors).
- Parameters: **bow speed**, **pressure proxy**, **grain** (noise vs. smooth).

## Scrape
- Intermittent micro‑onsets; map density → **roll rate** or **grain size**.
- Parameters: **min inter‑onset**, **jitter tolerance**, **smear amount**.

## Extended Techniques
- **Mute/Palm:** short contact before pluck → choke previous note.
- **Harmonics:** shallow angle/low amplitude over threshold → transposed partial.
- **Slides/Portamento:** sustained over threshold + position change → pitch bend.
- **Tremolo/Vibrato:** small periodic deltas → LFO depth/rate.

See also: `docs/SensingSurvey.md` for modality‑specific affordances/limitations.
