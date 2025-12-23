// StringFieldViz (Processing)
// Gestural debugger for overhead projectors. Paints noisy gradients, velocity
// arrows, and a state ticker based on JSON lines streamed from the firmware (or
// the keys below).

import processing.data.*;

int w = 960, h = 600;
float velocity = 0;
String gestureState = "IDLE";
ArrayList<HistoryItem> history = new ArrayList<HistoryItem>();
PGraphics noiseLayer;

// -- Lifecycle
// ----------------------------------------------------------------- setup() is
// Processing's boot hook. Use it to size the canvas, pick a legible font, and
// prep the off-screen noise layer we reuse every frame.
void setup() {
  size(960, 600);
  frameRate(60);
  textFont(createFont("Inconsolata", 32));
  noiseLayer = createGraphics(w, h);
}

// draw() runs 60 FPS. Every call completely redraws the scene so motion blur
// and velocity decay feel alive on a projector.
void draw() {
  background(18, 12, 40);
  drawNoise();
  drawStringWave();
  drawVelocityVector();
  drawStateLabel();
  drawHistory();
  velocity *= 0.92;
}

// Add TV-static energy: a translucent jitter layer that keeps the visualization
// from feeling sterile. Students can tweak counts/alpha live to see feedback.
void drawNoise() {
  noiseLayer.beginDraw();
  noiseLayer.clear();
  noiseLayer.strokeWeight(1);
  for (int i = 0; i < 140; i++) {
    float alpha = random(8, 26);
    noiseLayer.stroke(240, 40, 160, alpha);
    float x = random(w);
    float len = random(20, 140);
    float y = random(h);
    noiseLayer.line(x, y, x + len, y + random(-30, 30));
  }
  noiseLayer.endDraw();
  image(noiseLayer, 0, 0);
}

// Render the stylized "string". Its amplitude is tied to the velocity variable
// so we can narrate "listen: louder pluck, higher wave" in real time.
void drawStringWave() {
  float amp = map(velocity, 0, 1, 4, 180);
  stroke(255, 245, 160);
  strokeWeight(6);
  line(60, h * 0.55, w - 60, h * 0.55);
  noFill();
  stroke(140, 220, 255);
  strokeWeight(3);
  beginShape();
  for (int x = 60; x <= w - 60; x += 14) {
    float t = map(x, 60, w - 60, 0, TWO_PI);
    float y = h * 0.55 + sin(t * 3 + frameCount * 0.08) * amp * 0.5;
    vertex(x, y);
  }
  endShape();
}

// Vertical arrow showing instantaneous velocity. Easy call-out for educators to
// connect the sensor energy to MIDI velocity numbers.
void drawVelocityVector() {
  float centerX = w * 0.25;
  float baseY = h * 0.75;
  float len = map(velocity, 0, 1, 12, h * 0.35);
  stroke(255, 90, 90);
  strokeWeight(10);
  line(centerX, baseY, centerX, baseY - len);
  noStroke();
  fill(255, 90, 90);
  triangle(centerX - 24, baseY - len + 36, centerX + 24, baseY - len + 36,
           centerX, baseY - len - 8);
  fill(255);
  textAlign(CENTER, CENTER);
  textSize(28);
  text("velocity → " + nf(velocity * 127, 1, 0), centerX, baseY + 48);
}

// Huge state label for the back row. The uppercase treatment matches the p5.js
// sketch so both tools share a visual vocabulary.
void drawStateLabel() {
  textAlign(LEFT, TOP);
  fill(255);
  textSize(42);
  text("gesture: " + gestureState, 40, 20);
}

// Rolling ticker of the last few gestures. Doubles as a debugging log when
// teaching: "see, two scrapes then a bow".
void drawHistory() {
  float cellWidth = 120;
  float baseY = h - 60;
  textSize(16);
  textAlign(CENTER, CENTER);
  for (int i = 0; i < history.size(); i++) {
    HistoryItem item = history.get(i);
    float alpha = map(i, 0, history.size(), 255, 90);
    fill(30, 210, 200, alpha);
    noStroke();
    float x = 90 + i * cellWidth;
    rect(x, baseY - 24, cellWidth - 20, 52, 12);
    fill(0);
    text(item.state + "\n" + nf(item.velocity * 127, 1, 0),
         x + (cellWidth - 20) / 2, baseY + 2);
  }
}

// Core state mutation helper. Stores the new gesture/velocity, clamps to [0,1],
// and prunes the history so it stays readable on a classroom projector.
void registerState(String state, float vel) {
  gestureState = state.toUpperCase();
  velocity = constrain(vel, 0, 1);
  history.add(0, new HistoryItem(gestureState, velocity));
  if (history.size() > 7) {
    history.remove(history.size() - 1);
  }
}

// Parse an incoming JSON line from Serial (or a log file). We keep it tiny so
// students can trace the error paths without a debugger.
void ingestLine(String line) {
  JSONObject json = parseJSONObject(line);
  if (json == null)
    return;
  if (!json.hasKey("gesture"))
    return;
  float rawValue = json.hasKey("value") ? json.getFloat("value") : 0;
  float vel = constrain(rawValue / 127.0, 0, 1);
  registerState(json.getString("gesture"), vel);
}

// Keyboard cheats for rehearsal demos (same bindings as the p5 version).
void keyPressed() {
  if (key == 'p' || key == 'P') {
    registerState("pluck", random(0.6, 1.0));
  }
  if (key == 's' || key == 'S') {
    registerState("scrape", random(0.3, 0.7));
  }
  if (key == 'b' || key == 'B') {
    registerState("bow", random(0.2, 0.9));
  }
  if (key == 'r' || key == 'R') {
    registerState("release", 0);
  }
}

class HistoryItem {
  String state;
  float velocity;
  // Lightweight record type; nothing fancy. Keeping it explicit helps Java
  // newbies.
  HistoryItem(String state, float velocity) {
    this.state = state;
    this.velocity = velocity;
  }
}
