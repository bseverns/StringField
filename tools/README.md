# Tools (Field Notes Edition)

This folder carries the small-but-essential helpers that let StringField hit the
v0.4 “Bridges” milestone from the roadmap while keeping faith with the vision
document’s privacy constraints.

## `serial_logger.py`

*Why it exists*: workshops need a transparent way to capture sensor streams so
players can annotate gestures, train classifiers, or debug mappings. The vision
calls out “logging opt-in and obvious,” so the logger refuses to run until the
operator types `log` (or passes `--start` once consent is already on paper).

*How to use it*

```bash
pip install pyserial
python tools/serial_logger.py /dev/ttyACM0 115200 --comment "opt-in: class A" > take01.csv
```

*What you get*: CSV with UTC timestamps, elapsed seconds, and the raw serial
line—ready to drop into a spreadsheet or notebook. Comment lines (prefixed with
`# `) narrate the session context so future you knows who, what, and why.

*Teaching tip*: mirror the capture on a projector, narrate the consent step out
loud, and let students call out when to stop logging. It reinforces agency and
ties directly back to the community-tested milestone plan.
