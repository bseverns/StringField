// StringFieldViz (Processing)
// A tiny visual that draws a “string” and reacts to gesture messages.
// This is a stub for classroom demos; expand with OSC or Serial parsing later.

int w = 800, h = 400;
float energy = 0;

void setup() {
  size(w, h);
  frameRate(60);
}

void draw() {
  background(255);
  // Draw baseline string
  stroke(0);
  line(40, h/2, w-40, h/2);
  // Visualize energy as displacement
  float amp = map(energy, 0, 1, 0, 120);
  strokeWeight(2);
  noFill();
  beginShape();
  for (int x = 40; x <= w-40; x += 10) {
    float t = map(x, 40, w-40, 0, TWO_PI);
    float y = h/2 + sin(t*3 + frameCount*0.1) * amp;
    vertex(x, y);
  }
  endShape();
  // Decay
  energy *= 0.95;
}

// TODO: Hook up to Serial/OSC; for now press keys to simulate gestures.
void keyPressed() {
  if (key == 'p') energy = 1.0;  // pluck
  if (key == 's') energy = 0.7;  // scrape
  if (key == 'b') energy = min(1.0, energy + 0.05);  // bow
}
