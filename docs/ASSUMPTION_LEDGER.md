# ASSUMPTION LEDGER

A running list of design assumptions, thresholds, simplifications, and “known unknowns.” Update whenever a choice is made without full measurement.

ID | Area | Assumption | Rationale | Test we owe | Owner | Status
---|---|---|---|---|---|---
A‑001 | Optical | Modulated IR at ~38kHz reduces ambient interference enough for classroom lighting | Common practice with IR receivers | Compare modulated vs DC in gym lighting | TBD | Open
A‑002 | Capacitive | Single‑wire + high‑Z ADC is stable enough if a body ground strap is used | EDU speed & BOM simplicity | Measure drift across students/surfaces | TBD | Open
A‑003 | Gesture | Inter‑onset < 40 ms constitutes “scrape grain” | Matches perceived rasp | Validate with player study | TBD | Open
A‑004 | Mapping | Pentatonic major is easiest for first‑play success | Fewer “wrong” notes | A/B with Dorian | TBD | Open
