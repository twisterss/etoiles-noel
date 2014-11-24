#include "Stars.h"

void Stars::begin() {
  // Initialize the strip
  strip.begin();
}

void Stars::set(uint8_t star, uint8_t branch, uint32_t color) {
  strip.setPixelColor(getLedFromLocation(star, branch), color);
}

void Stars::setStar(uint8_t star, uint32_t color) {
  for (uint8_t branch = 0; branch < branches_count; branch++) {
    set(star, branch, color);
  }
}

void Stars::setBranch(uint8_t branch, uint32_t color) {
  for (uint8_t star = 0; star < stars_count; star++) {
    set(star, branch, color);
  }
}

uint32_t Stars::get(uint8_t star, uint8_t branch) {
  return strip.getPixelColor(getLedFromLocation(star, branch));
}

void Stars::commit() {
  strip.show();
}

void Stars::clear(uint32_t color) {
  for (uint8_t star = 0; star < stars_count; star++) 
    for (uint8_t branch = 0; branch < branches_count; branch++) 
      set(star, branch, color);
}

uint32_t Stars::color(uint8_t red, uint8_t green, uint8_t blue) {
  return strip.Color(red, green, blue);
}

uint8_t Stars::stars() {
  return stars_count;
}

uint8_t Stars::branches() {
  return branches_count;
}

uint32_t Stars::getLedFromLocation(uint8_t star, uint8_t branch) {
  return star * branches_count + branch;
}

