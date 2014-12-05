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

// Minimum delay between two display steps (microseconds)
#define DISPLAY_DELAY 1000
// Number of steps to display initialization
#define INIT_MODE_STEPS 2000

// Button management
#define BUTTON_THRESHOLD 900
#define BUTTON_IGNORED_STEPS 1000

// Avalaible modes
#define MODE_INIT 0
#define MODE_ALL 1
#define MODE_WHITE 2
#define MODE_SHIFTING_COLORS 3
#define MODE_BLINKING_WHITE 4
#define MODE_COLORS 5
#define MODE_TWINKLE 6
#define MODE_ROTATING_COLORS 7
#define MODE_FIRE 8

// Data about modes
// Real modes are selectable by the button
// Simple modes are selectable by automatic modes
#define MODES 9
#define STARTUP_MODE MODE_ALL
#define FIRST_REAL_MODE MODE_ALL
#define FIRST_SIMPLE_MODE MODE_WHITE

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
uint8_t currentMode = STARTUP_MODE;
uint8_t nextMode;
bool newMode = true;
bool forceNewMode = false;

// Button state
bool buttonPushed = false;
uint16_t buttonChangedSteps = 0;

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
 * Display an initial pattern before
 * a mode to identify the current mode
 */
void displayInit(bool newDisplay) {
  if (skipSteps(INIT_MODE_STEPS, newDisplay)) {
    if (newDisplay) {
      // Display the initial mode
      for (uint8_t branch = 0; branch < BRANCHES; branch++) {
        uint32_t color = BLACK;
        if (nextMode >= (branch + 1))
          color+= RED;
        if (nextMode >= (BRANCHES + branch + 1))
          color+= GREEN;
        if (nextMode >= (2*BRANCHES + branch + 1))
          color+= BLUE;
        stars.setBranch(branch, color);
      }
      stars.commit();
    } else {
      // Go the next mode
      currentMode = nextMode;
      forceNewMode = true;
    }
  }
}

/**
 * Display a bit of everything
 */
void displayAll(bool newDisplay) {
  const uint16_t modeDuration = 30000;
  static uint8_t modeIn;
  static uint16_t duration;
  bool newModeIn = false;
  if (newDisplay) {
    modeIn = FIRST_SIMPLE_MODE;
    duration = 0;
    newModeIn = true;
  }
  if (duration >= modeDuration) {
    modeIn+= 1;
    if (modeIn == MODES)
      modeIn = FIRST_SIMPLE_MODE;
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
  const uint32_t colors[] = {YELLOW, RED, PURPLE, BLUE, GREEN};
  static uint8_t shift = 0;
  if (newDisplay) {
    shift = 0;
  }
  if (skipSteps(300, newDisplay)) {
    for (uint8_t branch = 0; branch < BRANCHES; branch++)
      stars.setBranch(branch, colors[(branch + shift) % BRANCHES]);
    stars.commit();
    shift = (shift + 1) % BRANCHES;
  }
}


/**
 * Helper for the fire mode
 * Initiate a fire in a calm branch
 */
void initFire() {
  uint8_t star;
  uint8_t branch;
  do {
    star = random(STARS);
    branch = random(BRANCHES);
  } while (stars.greenComp(stars.get(star, branch)) > 0);
  stars.set(star, branch, stars.color(255, random(1, 20), 0));
}

/**
 * Display a fire effect in stars
 */
void displayFire(bool newDisplay) {
  // Random parameters
  const uint16_t dirChangeProb = 10;
  const uint16_t finishProb = 200;
  // Number of branches on fire
  const uint8_t fires = STARS*2;
  // Current mode is hidden in the red value
  const uint8_t increase = 255;
  const uint8_t decrease = 254;
  const uint8_t increaseRand = 253;
  const uint8_t decreaseRand = 252;
  // Change speed
  const uint8_t speed = 20;
  if (newDisplay) {
    // Initalize random fires
    stars.clear(stars.color(255, 0, 0));
    for (uint8_t i = 0; i < fires; i++)
      initFire();
  }
  if (skipSteps(20, newDisplay)) {
    for (uint8_t star = 0; star < STARS; star++) {
      for (uint8_t branch = 0; branch < BRANCHES; branch++) {
        uint32_t color = stars.get(star, branch);
        uint8_t red = stars.redComp(color);
        uint8_t green = stars.greenComp(color);
        if (green > 0) {
          // There is a fire, keed changing the color
          if (red == increase || red == increaseRand) {
            // Increasing mode
            green+= speed;
          } else if (red == decrease || red == decreaseRand) {
            // Decreasing mode
            green-= speed;
          }
          if (red == increaseRand || red == decreaseRand) {
            // Try changing the random mode
            if (random(dirChangeProb) == 0)
              red = red == increaseRand ? decreaseRand : increaseRand;
            // Try getting out of random mode to decreasing mode
            if (random(finishProb) == 0)
              red = decrease;
          }
          if (green > 255 - speed) {
            // Maximum reached: go to random decreasing mode
            green = 255;
            red = decreaseRand;
          }
          if (green <= speed) {
            // Minimum reached
            if (red == 254) {
              // End this fire and start a new one
              red = increase;
              green = 0;
              initFire();
              } else {
                // Random increasing mode
                red = increaseRand;
                green = 1;
              }
          }
          stars.set(star, branch, stars.color(red, green, 0));
        }
      }
    }
    stars.commit();
  }
}

/**
 * Twinkling white stars
 */
void displayTwinkle(bool newDisplay) {
  const uint8_t minWhite = 20;
  const uint8_t maxWhite = 255;
  if (skipSteps(60, newDisplay)) {
    for (uint8_t star = 0; star < STARS; star++) {
      for (uint8_t branch = 0; branch < BRANCHES; branch++) {
        uint8_t val = random(minWhite, maxWhite+1);
        stars.set(star, branch, stars.color(val, val, val));
      }
    }
    stars.commit();
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
    case MODE_INIT:
      displayInit(newDisplay);
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
    case MODE_FIRE:
      displayFire(newDisplay);
      break;
    case MODE_TWINKLE:
      displayTwinkle(newDisplay);
      break;
  }
}

/**
 * Display one step
 * (no wait) of the current mode
 */
void display() {
  display(currentMode, newMode);
  newMode = forceNewMode;
  forceNewMode = false;
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
      // New button push detected: set the next mode
      // to the mode after the current one
      if (currentMode == MODE_INIT)
        nextMode = nextMode + 1;
      else
        nextMode = currentMode + 1;
      if (nextMode >= MODES)
        nextMode = FIRST_REAL_MODE;
      // Actual mode is init: display the current mode
      currentMode = MODE_INIT;
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

