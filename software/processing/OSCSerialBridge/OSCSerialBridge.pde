// StringField OSC/Serial Bridge (Processing)
// Forwards firmware gesture packets (JSON over serial) to OSC while projecting
// educator-friendly overlays. Built for the classroom: explicit consent prompts,
// reminder banners about the serial logger, and call-outs that echo the README's
// preset browser steps.

import processing.serial.*;
import processing.data.*;
import oscP5.*;
import netP5.*;

// --- Configurable bits -------------------------------------------------------
// Pick the OSC target you want to drive (e.g., SuperCollider, Pd, Ableton). The
// bridge sends a single address: /stringfield/gesture with [gesture, value, normalized].
String oscHost = "127.0.0.1";
int oscPort = 9000;
int oscListenPort = 12001; // used only so oscP5 can initialize; we don't listen.

// Serial defaults. Change the port index at runtime with the 'n' key.
int serialBaud = 115200;
int serialPortIndex = 0;
Serial serialPort;

// --- State -------------------------------------------------------------------
OscP5 oscP5;
NetAddress oscDestination;
String lastGesture = "(waiting)";
float lastRawValue = 0;
float lastNormalized = 0;
String lastLine = "no packets yet";
String statusMessage = "press 'n' to pick a serial port";
int frameCounter = 0;

String[] consentPrompts = {
  "Ask: ok to log gestures for ~5 min? (tools/serial_logger.py → CSV of JSON)",
  "Name the file + exit plan before logging; delete on request.",
  "Narrate the preset browser: paste JSON notes over serial, no reflash needed.",
};

String[] lessonBeats = {
  "1) Warmup: keyboard cheats in viz → explain pluck/bow/scrape labels",
  "2) Flip on logger after consent; mirror packets to OSC synth.",
  "3) Swap note sets with the README preset browser steps, listen for change.",
  "4) Pause, review ticker, ask students to predict the next state.",
};

String loggerCommand = "python tools/serial_logger.py /dev/ttyACM0 115200 > logs/session.csv";
String presetBrowserSnippet = "{ \"notes\": [60, 63, 67, 70, 74] }";

// --- Lifecycle ---------------------------------------------------------------
void setup() {
  size(1100, 720);
  frameRate(60);
  textFont(createFont("Inconsolata", 20));
  oscP5 = new OscP5(this, oscListenPort);
  oscDestination = new NetAddress(oscHost, oscPort);
  openSerialPort(serialPortIndex);
}

void draw() {
  background(12, 8, 26);
  drawBackdrop();
  drawStatusPanel();
  drawConsentPanel();
  drawLessonBeats();
  drawLoggerCard();
  drawPresetBrowserCard();
  frameCounter++;
}

// --- Serial + OSC plumbing ---------------------------------------------------
void openSerialPort(int index) {
  if (serialPort != null) {
    serialPort.stop();
  }
  if (Serial.list().length == 0) {
    statusMessage = "No serial ports found. Plug in firmware + reopen.";
    serialPort = null;
    return;
  }
  int clamped = index % Serial.list().length;
  serialPortIndex = clamped;
  serialPort = new Serial(this, Serial.list()[serialPortIndex], serialBaud);
  serialPort.bufferUntil('\n');
  statusMessage = "Reading from " + Serial.list()[serialPortIndex];
}

void serialEvent(Serial port) {
  String line = port.readStringUntil('\n');
  if (line == null) return;
  line = trim(line);
  if (line.length() == 0) return;
  lastLine = line;
  ingestGesture(line);
}

void ingestGesture(String line) {
  JSONObject json = parseJSONObject(line);
  if (json == null || !json.hasKey("gesture")) {
    statusMessage = "Ignored line (not gesture JSON).";
    return;
  }

  lastGesture = json.getString("gesture");
  lastRawValue = json.hasKey("value") ? json.getFloat("value") : 0;
  lastNormalized = constrain(lastRawValue / 127.0, 0, 1);

  OscMessage msg = new OscMessage("/stringfield/gesture");
  msg.add(lastGesture);
  msg.add(lastRawValue);
  msg.add(lastNormalized);
  oscP5.send(msg, oscDestination);
  statusMessage = "Forwarded gesture → OSC at " + oscHost + ":" + oscPort;
}

// --- Overlays ----------------------------------------------------------------
void drawBackdrop() {
  noStroke();
  for (int y = 0; y < height; y++) {
    float t = map(y, 0, height, 0, 1);
    int c = lerpColor(color(16, 10, 50), color(40, 12, 80), t);
    stroke(c);
    line(0, y, width, y);
  }
  noStroke();
  fill(255, 40);
  for (int i = 0; i < 120; i++) {
    float x = random(width);
    float len = random(12, 90);
    float y = random(height);
    rect(x, y, len, 1 + random(3), 4);
  }
}

void drawStatusPanel() {
  fill(255);
  textSize(28);
  text("StringField OSC/Serial Bridge", 30, 36);
  textSize(18);
  text(statusMessage, 30, 66);

  textSize(16);
  float y = 110;
  text("OSC target → " + oscHost + ":" + oscPort, 30, y); y += 24;
  text("Serial port → " + (serialPort == null ? "none" : Serial.list()[serialPortIndex]) + " @ " + serialBaud + " baud", 30, y); y += 24;
  text("Last gesture → " + lastGesture, 30, y); y += 24;
  text("Last value → " + nf(lastRawValue, 1, 0) + " (norm " + nf(lastNormalized, 1, 2) + ")", 30, y); y += 24;
  text("Last line → " + truncate(lastLine, 70), 30, y);

  fill(180, 240, 255);
  text("Keys: 'n' next serial port · 'p' ping OSC · 'h' copy handout steps", 30, height - 30);
}

void drawConsentPanel() {
  float panelX = width * 0.55;
  float panelY = 60;
  float panelW = width * 0.4;

  fill(20, 150);
  noStroke();
  rect(panelX - 20, panelY - 30, panelW, 210, 14);

  fill(255);
  textSize(22);
  text("Consent + logging reminders", panelX, panelY);
  textSize(16);
  float y = panelY + 30;
  for (int i = 0; i < consentPrompts.length; i++) {
    text("• " + consentPrompts[i], panelX, y);
    y += 24;
  }
}

void drawLessonBeats() {
  float panelX = width * 0.55;
  float panelY = 320;
  float panelW = width * 0.4;

  fill(20, 150);
  noStroke();
  rect(panelX - 20, panelY - 30, panelW, 200, 14);

  fill(255);
  textSize(22);
  text("Handout beats (print + riff)", panelX, panelY);
  textSize(16);
  float y = panelY + 30;
  for (int i = 0; i < lessonBeats.length; i++) {
    text("• " + lessonBeats[i], panelX, y);
    y += 24;
  }
}

void drawLoggerCard() {
  float panelX = 30;
  float panelY = height * 0.6;

  fill(20, 170);
  noStroke();
  rect(panelX - 10, panelY - 32, width * 0.44, 120, 12);

  fill(255);
  textSize(20);
  text("Serial logger workflow (consent-first)", panelX, panelY);
  textSize(15);
  text("Ask first, then run → " + loggerCommand, panelX, panelY + 24);
  text("Pass --start only if consent already documented. Streams CSV for audits.", panelX, panelY + 44);
  text("Logger pairs with overlays: projector shows what we're saving and why.", panelX, panelY + 64);
}

void drawPresetBrowserCard() {
  float panelX = 30;
  float panelY = height * 0.78;

  fill(20, 170);
  noStroke();
  rect(panelX - 10, panelY - 32, width * 0.44, 120, 12);

  fill(255);
  textSize(20);
  text("Preset browser cameo (README flow)", panelX, panelY);
  textSize(15);
  text("Paste into serial/ logger terminal → " + presetBrowserSnippet, panelX, panelY + 24);
  text("Firmware echoes the set; bridge keeps forwarding gestures to OSC synth.", panelX, panelY + 44);
  text("Narrate the change: no reflash, just a new scale on the fly.", panelX, panelY + 64);
}

String truncate(String src, int maxLen) {
  if (src == null) return "";
  if (src.length() <= maxLen) return src;
  return src.substring(0, maxLen - 3) + "...";
}

// --- Interactions ------------------------------------------------------------
void keyPressed() {
  if (key == 'n' || key == 'N') {
    openSerialPort(serialPortIndex + 1);
  }
  if (key == 'p' || key == 'P') {
    OscMessage msg = new OscMessage("/stringfield/ping");
    msg.add(frameCounter);
    oscP5.send(msg, oscDestination);
    statusMessage = "Pinged OSC target (" + oscHost + ":" + oscPort + ")";
  }
  if (key == 'h' || key == 'H') {
    println("Handout beats →");
    for (String beat : lessonBeats) {
      println("  " + beat);
    }
  }
}
