// StringField gesture debugger (p5.js)
// Loud visuals for classroom demos. Reads JSON gesture blobs from WebSerial/WebMIDI
// (or via the key-trigger cheats below) and paints velocity vectors + state history.

let gestureState = 'idle';
let velocity = 0;
let stateHistory = [];
let noiseLayer;

// -- Lifecycle -----------------------------------------------------------------
// setup() runs once when the sketch boots. Treat it like lab warm-up: lay out
// the canvas, fonts, and buffer layers that give us the noisy projector texture.
function setup() {
  createCanvas(960, 540);
  frameRate(60);
  textFont('monospace');
  noiseLayer = createGraphics(width, height);
  noiseLayer.clear();
  // TODO: hook up navigator.serial or WebMIDI; call ingestSerialLine(line) per message.
}

// draw() fires 60 times per second. Every call repaints the entire scene so the
// class can literally see velocity energy dissipate (via the decay multiplier at
// the bottom of the function).
function draw() {
  background(18, 12, 40);
  drawNoise();
  drawStringWave();
  drawVelocityVector();
  drawStateLabel();
  drawHistory();
  velocity *= 0.92;
}

// Cosmetic grain layer: gives the projector output a tactile feel so the class
// remembers this is a physical-instrument proxy, not sterile UI chrome.
function drawNoise() {
  noiseLayer.clear();
  noiseLayer.strokeWeight(1);
  for (let i = 0; i < 120; i++) {
    const alpha = random(8, 24);
    noiseLayer.stroke(240, 40, 160, alpha);
    const x = random(width);
    const len = random(20, 120);
    const y = random(height);
    noiseLayer.line(x, y, x + len, y + random(-20, 20));
  }
  image(noiseLayer, 0, 0);
}

// Hero visual: a fake string whose amplitude maps directly to the current
// velocity value. The three-part color story (gold string, cyan shimmer) mirrors
// the firmware narration of pluck/bow energy.
function drawStringWave() {
  const amp = map(velocity, 0, 1, 2, 180);
  stroke(255, 245, 160);
  strokeWeight(6);
  line(60, height * 0.55, width - 60, height * 0.55);
  noFill();
  stroke(140, 220, 255);
  strokeWeight(3);
  beginShape();
  for (let x = 60; x <= width - 60; x += 14) {
    const t = map(x, 60, width - 60, 0, TWO_PI);
    const y = height * 0.55 + sin(t * 3 + frameCount * 0.08) * amp * 0.5;
    vertex(x, y);
  }
  endShape();
}

// Vector meter for velocity. The arrow length is derived from the normalized
// velocity so educators can point and say "velocity 90 → arrow near the top".
function drawVelocityVector() {
  const centerX = width * 0.25;
  const baseY = height * 0.75;
  const len = map(velocity, 0, 1, 10, height * 0.35);
  stroke(255, 90, 90);
  strokeWeight(10);
  line(centerX, baseY, centerX, baseY - len);
  fill(255, 90, 90);
  noStroke();
  triangle(centerX - 20, baseY - len + 30, centerX + 20, baseY - len + 30, centerX, baseY - len - 10);
  fill(255);
  textSize(24);
  textAlign(CENTER, CENTER);
  text(`velocity → ${nf(velocity * 127, 1, 0)}`, centerX, baseY + 40);
}

// Giant uppercase label so even the back row knows which gesture state is live.
function drawStateLabel() {
  textAlign(LEFT, TOP);
  fill(255);
  textSize(54);
  text(`gesture: ${gestureState.toUpperCase()}`, 60, 40);
}

// Rolling six-event history. Each block shows gesture + velocity so students can
// correlate sensor motion with the discrete states they hear.
function drawHistory() {
  const cellWidth = 120;
  const baseY = height - 80;
  textSize(20);
  textAlign(CENTER, CENTER);
  for (let i = 0; i < stateHistory.length; i++) {
    const item = stateHistory[i];
    const x = 80 + i * cellWidth;
    const alpha = map(i, 0, stateHistory.length, 255, 90);
    fill(30, 210, 200, alpha);
    noStroke();
    rect(x, baseY - 20, cellWidth - 16, 44, 12);
    fill(0);
    text(`${item.state}\n${nf(item.velocity * 127, 1, 0)}`, x + (cellWidth - 16) / 2, baseY + 2);
  }
}

// Central helper: store the new gesture + velocity, clamp to sane ranges, and
// keep the history list tidy. Everything funnels through here so the class can
// track how keyboard shortcuts and serial events feed the viz.
function registerState(state, vel) {
  gestureState = state;
  velocity = constrain(vel, 0, 1);
  stateHistory.unshift({ state: state.toUpperCase(), velocity: velocity });
  if (stateHistory.length > 6) {
    stateHistory.pop();
  }
}

// Called whenever WebSerial/WebMIDI hands us a full line of JSON. We parse it,
// shrug off errors (bad packets shouldn't crash the workshop), and hand the
// result to the handler below.
function ingestSerialLine(line) {
  try {
    const msg = JSON.parse(line);
    handleGestureBlob(msg);
  } catch (err) {
    // shrug; keep the punk spirit but ignore malformed lines
  }
}

// Interpret the gesture JSON from firmware. `value` is 0-127; we convert to
// 0-1 so the rest of the drawing logic stays normalized.
function handleGestureBlob(msg) {
  if (!msg || !msg.gesture) return;
  const normalized = constrain((msg.value || 0) / 127, 0, 1);
  registerState(msg.gesture, normalized);
}

// Quick keyboard cheats so students can rehearse the visuals without hardware.
// 'P' → pluck, 'S' → scrape, 'B' → bow, 'R' → release,
// 'H' → harmonic, 'M' → mute, 'T' → tremolo, 'V' → vibrato.
function keyPressed() {
  if (key === 'p' || key === 'P') {
    registerState('pluck', random(0.6, 1.0));
  }
  if (key === 's' || key === 'S') {
    registerState('scrape', random(0.3, 0.7));
  }
  if (key === 'b' || key === 'B') {
    registerState('bow', random(0.2, 0.9));
  }
  if (key === 'r' || key === 'R') {
    registerState('release', 0);
  }
  if (key === 'h' || key === 'H') {
    registerState('harmonic', random(0.3, 0.6));
  }
  if (key === 'm' || key === 'M') {
    registerState('mute', random(0.05, 0.2));
  }
  if (key === 't' || key === 'T') {
    registerState('tremolo', random(0.4, 0.8));
  }
  if (key === 'v' || key === 'V') {
    registerState('vibrato', random(0.4, 0.9));
  }
}
