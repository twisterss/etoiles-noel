/**
 * This is the main sketch for using the stars for christmas
 */
 
#include <Adafruit_NeoPixel.h>
#include "Stars.h"

// Hardware configuration
#define STRIP_PIN 6
#define BUTTON_PIN A0
#define UNCONNECTED_ANALOG_PIN A1

// Stars configuration
#define STARS 3
#define BRANCHES 5

// Minimum delay between two display states (microseconds)
#define DISPLAY_DELAY 1000

#define BUTTON_THRESHOLD 990
#define BUTTON_IGNORED_STEPS 200

// Avalaible modes
#define MODES 2
#define MODE_WHITE 0
#define MODE_ROTATING_COLORS 1

// Colors
#define WHITE stars.color(255, 255, 255)
#define RED stars.color(255, 0, 0)
#define GREEN stars.color(0, 255, 0)
#define BLUE stars.color(0, 0, 255)
#define YELLOW stars.color(255, 255, 0)
#define PURPLE stars.color(255, 0, 255)

// Stars LEDs
Stars stars(STRIP_PIN, STARS, BRANCHES);

// Current display mode
uint8_t currentMode = 0;
bool newMode = true;

// Button state
bool buttonPushed = false;
uint8_t buttonChangedSteps = 0;

/**
 * Mix 2 colors together.
 * If step = 0, this is color1, if step = maxStep, this is color2, between, this is a mix
 */
uint32_t transitionColor(uint8_t step, uint8_t maxStep, uint32_t color1, uint32_t color2 = 0) {
  uint16_t r1, g1, b1, r2, g2, b2;
  r1 = (uint8_t) (color1 >> 16); g1 = (uint8_t) (color1 >> 8); b1 = (uint8_t) color1;
  r2 = (uint8_t) (color2 >> 16); g2 = (uint8_t) (color2 >> 8); b2 = (uint8_t) color2;
  r1 *= maxStep - step; g1*= maxStep - step; b1 *= maxStep - step;
  r2 *= step; g2*= step; b2 *= step;
  r1+= r2; g1+= g2; b1+= b2;
  r1/= maxStep; g1/= maxStep; b1/= maxStep;
  return stars.color(r1, g1, b1);
}

/**
 * Wheel of simple colors
 */
uint32_t wheel(uint8_t wheelPos) {
  if(wheelPos < 85) {
    return stars.color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if(wheelPos < 170) {
    wheelPos -= 85;
    return stars.color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
    wheelPos -= 170;
    return stars.color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

/**
 * Utility to so sometheing every n steps.
 * Returns true every n steps
 */
bool skipSteps(uint16_t steps) {
  static uint16_t step = 0;
  step++;
  if (step >= steps) {
    step = 0;
    return true;
  }
  return false;
}

/**
 * All stars are white
 */
void displayWhite(bool newDisplay) {
  if (newDisplay) {
    stars.clear(stars.color(255, 255, 255));
    stars.commit();
  }
}

/**
 * Each branch has a different color,
 * colors rotate between branches,
 * all stars are identical
 */
void displayRotatingColors(bool newDisplay) {
  static uint8_t step = 0;
  if (newDisplay) {
    step = 0;
  }
  if (skipSteps(20)) {
    for (uint8_t branch = 0; branch < BRANCHES; branch++) {
      uint16_t offset = branch;
      offset<<= 8;
      offset/= BRANCHES;
      stars.setBranch(branch, wheel(step+offset));
    }
    stars.commit();
    step++;
  }
}

/**
 * Display one step
 * (no wait)
 */
void display() {
  switch(currentMode) {
    case MODE_WHITE:
      displayWhite(newMode);
      break;
    case MODE_ROTATING_COLORS:
      displayRotatingColors(newMode);
      break;
  }
  newMode = false;
}

/**
 * Check the button to 
 * detect mode changes
 */
void checkButton() {
  // Clean up the signal
  bool pushedNow = analogRead(BUTTON_PIN) < BUTTON_THRESHOLD;
  if (pushedNow != buttonPushed)
    buttonChangedSteps++;
  if (buttonChangedSteps >= BUTTON_IGNORED_STEPS) {
    // Button changed
    buttonPushed = !buttonPushed;
    buttonChangedSteps = 0;
    if (buttonPushed) {
      // New button push detected: go to next mode
      currentMode = (currentMode + 1) % MODES;
      newMode = true;
    }
  }
}

void setup() {
  // Watchdog initialization
  //wdt_enable(WDTO_4S);
  // Button initialization
  pinMode(BUTTON_PIN, INPUT);
  // Stars initialization
  stars.begin();
  // Random generator initialization
  randomSeed(analogRead(UNCONNECTED_ANALOG_PIN));
}

void loop() {
  // Check the button
  checkButton();
  // Display periodically
  static unsigned long lastDisplayEvent = 0;
  const unsigned long timeNow = micros();
  const unsigned long duration = timeNow - lastDisplayEvent;
  if (duration >= DISPLAY_DELAY) {
    display();
    lastDisplayEvent = timeNow;
    // Tell the watchdog the program is running properly
    //wdt_reset();
  }
}

