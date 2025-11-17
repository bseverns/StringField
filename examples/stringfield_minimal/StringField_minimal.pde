/*
 StringField_minimal (Processing)
 --------------------------------
 Laptop-only teaching sketch for the StringField project. There is zero physical
 hardware involved: every "sensor" reading is faked using mouse motion,
 keyboard taps, and a slow random walk that stands in for noisy voltages.

 Why exist?
 - Let students see *mapping* (gesture → numbers → graphics) without plugging
   anything in.
 - Serve as a punk-rock sandbox where you can live-code changes while narrating.
 - Provide a tiny, readable Processing file with lots of intent comments.
*/

// --- Scene + data model -------------------------------------------------------
int canvasW = 960;
int canvasH = 620;

// Four "virtual" strings. Each one tracks energy (0-1) and gesture state.
String[] stringLabels = {"low drone", "low-mid", "high-mid", "sparkle"};
float[] stringEnergy = {0, 0, 0, 0};
String[] stringState = {"idle", "idle", "idle", "idle"};

// Shared HUD state mirrors what the firmware would broadcast as JSON.
String currentGesture = "IDLE";
float currentVelocity = 0; // normalized 0..1
int currentString = -1;    // which virtual string was touched last

// Autopilot timer gives you background motion even if you never touch the trackpad.
int autopilotCountdown = 180; // frames until the next automatic "gesture"

void setup() {
  size(canvasW, canvasH);
  surface.setTitle("StringField minimal laptop sketch");
  frameRate(60);
  textFont(createFont("Inconsolata", 24));
}

void draw() {
  background(12, 9, 28);
  injectSensorNoise();
  decayEnergy();
  drawStrings();
  drawHUD();
  tickAutopilot();
}

// --- Fake sensor model --------------------------------------------------------
// Real sensors never sit perfectly still, so we add a whisper of jitter to the
// energy values. Talking point: "see how the line never hits zero? that's the
// baseline drift we're compensating for in firmware".
void injectSensorNoise() {
  for (int i = 0; i < stringEnergy.length; i++) {
    float jitter = randomGaussian() * 0.002;
    stringEnergy[i] = constrain(stringEnergy[i] + jitter, 0, 1);
  }
}

// Every frame, energy bleeds off. This is the mapping equivalent of a
// low-pass / release envelope so plucks ring out and then vanish.
void decayEnergy() {
  for (int i = 0; i < stringEnergy.length; i++) {
    stringEnergy[i] *= 0.94; // tweak live while narrating decay curves
    if (stringEnergy[i] < 0.02) {
      stringState[i] = "idle";
    }
  }
}

// --- Rendering ---------------------------------------------------------------
void drawStrings() {
  float gutter = 80;
  float spacing = (canvasH - gutter * 2) / (stringEnergy.length - 1);
  strokeWeight(6);
  for (int i = 0; i < stringEnergy.length; i++) {
    float y = gutter + i * spacing;
    // Base string
    stroke(70, 210, 255, 200);
    line(80, y, canvasW - 80, y);
    // Wave overlay: amplitude represents current energy/velocity
    noFill();
    stroke(255, 240, 120);
    float amp = map(stringEnergy[i], 0, 1, 2, 110);
    beginShape();
    for (int x = 80; x <= canvasW - 80; x += 12) {
      float t = map(x, 80, canvasW - 80, 0, TWO_PI);
      float wobble = sin(t * 2.8 + frameCount * 0.12 + i) * amp;
      vertex(x, y + wobble);
    }
    endShape();

    // Label each string directly on the canvas so you can point to it.
    fill(230);
    textAlign(RIGHT, CENTER);
    text(stringLabels[i] + " (" + stringState[i] + ")", 70, y);
  }
}

void drawHUD() {
  // Upper-left: running narration of the last gesture.
  fill(255);
  textAlign(LEFT, TOP);
  textSize(28);
  text("gesture → " + currentGesture, 40, 24);
  textSize(20);
  text("velocity → " + nf(currentVelocity * 127, 1, 0), 40, 60);
  if (currentString >= 0) {
    text("string → " + stringLabels[currentString], 40, 86);
  }

  // Bottom notes: remind future-you of the controls without hunting README.md.
  textAlign(LEFT, BOTTOM);
  String legend = "Mouse press = pluck (vertical chooses string, horizontal speed = energy)."
                  + "  Keys 1-4 = fixed plucks | B = bow | S = scrape.";
  text(legend, 40, canvasH - 30);
}

// --- Control mappings --------------------------------------------------------
int pickStringFromY(float y) {
  float gutter = 80;
  float spacing = (canvasH - gutter * 2) / (stringEnergy.length - 1);
  int idx = round((y - gutter) / spacing);
  return constrain(idx, 0, stringEnergy.length - 1);
}

void registerGesture(String gestureName, int idx, float velocity) {
  currentGesture = gestureName.toUpperCase();
  currentVelocity = constrain(velocity, 0, 1);
  currentString = idx;
  stringEnergy[idx] = currentVelocity;
  stringState[idx] = gestureName;
}

// Mouse as pluck proxy. Drag horizontally for bigger velocity numbers.
void mousePressed() {
  int idx = pickStringFromY(mouseY);
  float velocity = constrain(abs(mouseX - pmouseX) / (float)canvasW * 5.0, 0.08, 1.0);
  registerGesture("pluck", idx, velocity);
}

// Keep updating while the mouse drags so you can narrate "continuous control".
void mouseDragged() {
  mousePressed();
}

void keyPressed() {
  if (key >= '1' && key <= '4') {
    int idx = key - '1';
    float vel = map(idx, 0, 3, 0.3, 0.9);
    registerGesture("pluck", idx, vel);
  }
  if (key == 'b' || key == 'B') {
    int idx = (int)random(stringEnergy.length);
    registerGesture("bow", idx, random(0.15, 0.45));
  }
  if (key == 's' || key == 'S') {
    int idx = (int)random(stringEnergy.length);
    registerGesture("scrape", idx, random(0.4, 1.0));
  }
  if (key == 'r' || key == 'R') {
    for (int i = 0; i < stringEnergy.length; i++) {
      stringEnergy[i] = 0;
      stringState[i] = "idle";
    }
    currentGesture = "IDLE";
    currentVelocity = 0;
    currentString = -1;
  }
}

// --- Autopilot ---------------------------------------------------------------
// Classrooms get quiet. This timer triggers a fake gesture every few seconds so
// the visualization never freezes (and you can narrate "here's what noise would
// look like if nobody touched the strings").
void tickAutopilot() {
  autopilotCountdown--;
  if (autopilotCountdown > 0) return;
  autopilotCountdown = (int)random(90, 220);

  int idx = (int)random(stringEnergy.length);
  float gestureRoll = random(1);
  if (gestureRoll < 0.5) {
    registerGesture("pluck", idx, random(0.2, 0.8));
  } else if (gestureRoll < 0.8) {
    registerGesture("bow", idx, random(0.1, 0.4));
  } else {
    registerGesture("scrape", idx, random(0.5, 1.0));
  }
}
