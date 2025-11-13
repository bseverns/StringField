#!/usr/bin/env python3
"""Serial to CSV logger with explicit opt-in.

INTENT (aligns with the v0.4 "Bridges" milestone in :doc:`docs/Roadmap`):
  - Provide a minimal, inspectable way to log sensor values and gesture labels.
  - Keep the "logging is opt-in and obvious" constraint from :doc:`docs/Vision`.
  - Produce CSV so workshops can open the capture in any spreadsheet without post-processing.

USAGE:
  python tools/serial_logger.py /dev/ttyACM0 115200 > run.csv

  The script pauses for an affirmative "log" confirmation before it will emit
  any rows. Pass ``--start`` when you already have explicit consent recorded.
"""

from __future__ import annotations

import argparse
import csv
import datetime as _dt
import sys
import time
from typing import Iterable


def _import_pyserial():
  try:
    import serial  # type: ignore
  except ModuleNotFoundError as exc:  # pragma: no cover - import-time guard
    print(
        "pyserial is required. Install with `pip install pyserial` before running.",
        file=sys.stderr,
    )
    raise SystemExit(2) from exc
  return serial


def _build_arg_parser() -> argparse.ArgumentParser:
  parser = argparse.ArgumentParser(
      description="Stream newline-delimited serial packets to CSV with timestamps.",
  )
  parser.add_argument("port", help="Serial port path (e.g. /dev/ttyACM0, COM3)")
  parser.add_argument("baud", type=int, help="Baud rate configured on the device")
  parser.add_argument(
      "--start",
      action="store_true",
      help="Skip the interactive consent prompt (use only with documented opt-in).",
  )
  parser.add_argument(
      "--timeout",
      type=float,
      default=0.5,
      help="Read timeout in seconds; keeps the loop responsive to Ctrl+C.",
  )
  parser.add_argument(
      "--encoding",
      default="utf-8",
      help="Character encoding for decoding serial bytes (default: utf-8).",
  )
  parser.add_argument(
      "--comment",
      action="append",
      default=[],
      metavar="TEXT",
      help="Add a '# ' comment line to the output (repeatable).",
  )
  return parser


def _emit_comments(lines: Iterable[str]) -> None:
  for line in lines:
    print(f"# {line}")


def _confirm_start(port: str) -> None:
  prompt = (
      f"Ready to log from {port}.\n"
      "Type 'log' and press Enter once you have consent documented (Ctrl+C to abort): "
  )
  response = input(prompt).strip().lower()
  if response != "log":
    print("Logging cancelled. No data was captured.", file=sys.stderr)
    raise SystemExit(0)


def _open_serial(serial, port: str, baud: int, timeout: float):  # type: ignore
  try:
    return serial.Serial(port=port, baudrate=baud, timeout=timeout)
  except serial.SerialException as exc:  # pragma: no cover - hardware dependent
    print(f"Could not open {port}: {exc}", file=sys.stderr)
    raise SystemExit(3) from exc


def main() -> None:
  args = _build_arg_parser().parse_args()
  serial = _import_pyserial()

  if not args.start:
    _confirm_start(args.port)

  ser = _open_serial(serial, args.port, args.baud, args.timeout)

  with ser:
    ser.reset_input_buffer()
    start = time.monotonic()
    now = _dt.datetime.now(_dt.timezone.utc).isoformat()

    header_comments = [
        "StringField serial_logger",
        f"Session start (UTC): {now}",
        f"Port: {args.port} @ {args.baud} baud",
        "Logging is opt-in; the operator confirmed before capture.",
    ]
    header_comments.extend(args.comment)
    _emit_comments(header_comments)

    writer = csv.writer(sys.stdout)
    writer.writerow(["timestamp_iso", "elapsed_seconds", "line"])
    sys.stdout.flush()

    try:
      while True:
        chunk = ser.readline()
        if not chunk:
          continue
        timestamp = _dt.datetime.now(_dt.timezone.utc).isoformat()
        elapsed = time.monotonic() - start
        decoded = chunk.decode(args.encoding, errors="replace").rstrip("\r\n")
        writer.writerow([timestamp, f"{elapsed:.6f}", decoded])
        sys.stdout.flush()
    except KeyboardInterrupt:  # pragma: no cover - user interaction
      duration = time.monotonic() - start
      print(
          f"\nStopped after {duration:.1f}s. Output saved to your redirected destination.",
          file=sys.stderr,
      )


if __name__ == "__main__":
  main()
