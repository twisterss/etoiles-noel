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
#define STARS 11
#define BRANCHES 5

// Minimum delay between two display states (microseconds)
#define DISPLAY_DELAY 1000

#define BUTTON_THRESHOLD 900
#define BUTTON_IGNORED_STEPS 1000

// Avalaible modes
#define MODES 6
#define MODE_ALL 0
#define MODE_WHITE 1
#define MODE_COLORS 2
#define MODE_BLINKING_WHITE 3
#define MODE_SHIFTING_COLORS 4
#define MODE_ROTATING_COLORS 5

// Colors
#define BLACK stars.color(0, 0, 0)
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
uint16_t buttonChangedSteps = 0;

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
 * Utility to do something every n steps.
 * Returns true when cleared.
 * Returns true every n calls.
 */
bool skipSteps(uint16_t steps, bool clear) {
  static uint16_t step = 0;
  step++;
  if (clear || step >= steps) {
    step = 0;
    return true;
  }
  return false;
}

/**
 * Display a bit of everything
 */
void displayAll(bool newDisplay) {
  static uint8_t modeIn;
  static uint16_t duration;
  bool newModeIn = false;
  if (newDisplay) {
    modeIn = 1;
    duration = 0;
    newModeIn = true;
  }
  if (duration >= 30000) {
    modeIn+= 1;
    if (modeIn == MODES)
      modeIn = 1;
    newModeIn = true;
    duration = 0;
  }
  display(modeIn, newModeIn);
  duration++;
}


/**
 * All stars are white
 */
void displayWhite(bool newDisplay) {
  if (newDisplay) {
    stars.clear(WHITE);
    stars.commit();
  }
}

/**
 * Stars are one of 5 colors
 */
void displayColors(bool newDisplay) {
  const uint8_t colorsCount = 5;
  const uint32_t colors[] = {RED, GREEN, PURPLE, YELLOW, BLUE};
  if (newDisplay) {
    for (uint8_t i = 0; i < STARS; i++)
      stars.setStar(i, colors[i % colorsCount]);
    stars.commit();
  }
}

/**
 * 1/3 of stars are switched off
 * others are white
 */
void displayBlinkingWhite(bool newDisplay) {
  const uint8_t stepsCount = 3;
  static uint8_t step = 0;
  if (newDisplay) {
    step = 0;
  }
  if (skipSteps(200, newDisplay)) {
    for (uint8_t star = 0; star < STARS; star++) {
      if (star % stepsCount == step)
        stars.setStar(star, BLACK);
      else
        stars.setStar(star, WHITE);
    }
    stars.commit();
    step = (step + 1) % stepsCount;
  }
}

/**
 * Stars are one of 5 colors.
 * Colors shift periodically between stars
 */
void displayShiftingColors(bool newDisplay) {
  const uint8_t colorsCount = 5;
  const uint32_t colors[] = {RED, GREEN, PURPLE, YELLOW, BLUE};
  static uint8_t step = 0;
  if (newDisplay) {
    step = 0;
  }
  if (skipSteps(1000, newDisplay)) {
    for (uint8_t i = 0; i < STARS; i++)
      stars.setStar(i, colors[(i+step) % colorsCount]);
    stars.commit();
    step = (step + 1) % colorsCount;
  }
}

/**
 * Each branch has a different color,
 * colors rotate between branches,
 * all stars are identical
 */
void displayRotatingColors(bool newDisplay) {
  const uint8_t stepsCount = 50;
  const uint32_t colors[] = {YELLOW, RED, RED, RED, RED};
  static uint8_t shift = 0;
  static uint8_t step = 0;
  if (newDisplay) {
    step = 0;
    shift = 0;
  }
  if (skipSteps(20, newDisplay)) {
    for (uint8_t branch = 0; branch < BRANCHES; branch++)
      stars.setBranch(branch, transitionColor(step, stepsCount, colors[(branch + shift) % BRANCHES], colors[(branch + shift + 1) % BRANCHES]));
    stars.commit();
    step++;
    if (step >= stepsCount) {
      step = 0;
      shift++;
      if (shift >= BRANCHES)
        shift = 0;
    }
  }
}

/**
 * Display one step
 * (no wait) of a given mode
 */
void display(uint8_t mode, bool newDisplay) {
  switch(mode) {
    case MODE_ALL:
      displayAll(newDisplay);
      break;
    case MODE_WHITE:
      displayWhite(newDisplay);
      break;
    case MODE_COLORS:
      displayColors(newDisplay);
      break;
    case MODE_BLINKING_WHITE:
      displayBlinkingWhite(newDisplay);
      break;
    case MODE_SHIFTING_COLORS:
      displayShiftingColors(newDisplay);
      break;
    case MODE_ROTATING_COLORS:
      displayRotatingColors(newDisplay);
      break;
  }
}

/**
 * Display one step
 * (no wait) of the current mode
 */
void display() {
  display(currentMode, newMode);
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

