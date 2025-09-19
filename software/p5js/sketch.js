// p5.js sketch stub for browser-based viz/bridge.
// Map incoming MIDI/Serial to a simple string animation.

let energy = 0;

function setup() {
  createCanvas(800, 400);
  frameRate(60);
  // TODO: WebMIDI / WebSerial hookup
}

function draw() {
  background(255);
  stroke(0);
  line(40, height/2, width-40, height/2);
  noFill();
  strokeWeight(2);
  beginShape();
  for (let x = 40; x <= width-40; x += 10) {
    const t = map(x, 40, width-40, 0, TWO_PI);
    const y = height/2 + Math.sin(t*3 + frameCount*0.1) * map(energy, 0, 1, 0, 120);
    vertex(x, y);
  }
  endShape();
  energy *= 0.95;
}
