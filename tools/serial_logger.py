#!/usr/bin/env python3
"""Serial to CSV logger (stub)

INTENT:
  - Provide a minimal, inspectable way to log sensor values and gesture labels.
  - Logging is OPT-IN and must be started explicitly.

USAGE:
  python tools/serial_logger.py /dev/ttyACM0 115200 > run.csv

"""
import sys, time

def main():
  if len(sys.argv) < 3:
    print("Usage: serial_logger.py PORT BAUD", file=sys.stderr)
    sys.exit(1)
  # TODO: import serial; read lines; write CSV rows with timestamps
  print("# Stub: implement with pyserial; do not log without explicit consent.")

if __name__ == "__main__":
  main()
