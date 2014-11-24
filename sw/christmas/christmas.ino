/**
 * This is the main sketch for using the stars for christmas
 */
 
#include <Adafruit_NeoPixel.h>
#include "Stars.h"

// Hardware configuration
#define STRIP_PIN 6

// Stars configuration
#define STARS 2
#define BRANCHES 5

// Stars LEDs
Stars stars(STRIP_PIN, STARS, BRANCHES);

void setup() {
  stars.begin();
}

void loop() {
  stars.clear(stars.color(255, 255, 255));
  stars.commit();
  delay(1000);
  stars.setBranch(0, stars.color(255, 0, 0));
  stars.commit();
  delay(100);
  stars.setBranch(1, stars.color(255, 255, 0));
  stars.commit();
  delay(100);
  stars.setBranch(2, stars.color(0, 255, 0));
  stars.commit();
  delay(100);
  stars.setBranch(3, stars.color(0, 255, 255));
  stars.commit();
  delay(100);
  stars.setBranch(4, stars.color(0, 0, 255));
  stars.commit();
  delay(1000);
  stars.setStar(0, stars.color(255, 0, 0));
  stars.commit();
  delay(500);
  stars.setStar(1, stars.color(0, 255, 0));
  stars.commit();
  delay(1000);
}

