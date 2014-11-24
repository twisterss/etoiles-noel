/**
 * Represents the star display
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Stars {
  
  private:
  
  /**
   * LED strip
   */
  Adafruit_NeoPixel strip;
  
  /**
   * Number of stars
   */
  uint8_t stars_count;
  
  /**
   * Number of branches per star
   */
  uint8_t branches_count;
  
  /**
   * Get the LED number from a star location
   */
  uint32_t getLedFromLocation(uint8_t star, uint8_t branch);
  
  public:
  
  /**
   * Pixels constructor
   */
  Stars(uint8_t dataPin, uint8_t stars, uint8_t branches):
    strip(stars*branches, dataPin, NEO_GRB + NEO_KHZ800),
    stars_count(stars),
    branches_count(branches)
  {
  }
  
  /**
   * Begin controlling the pixels
   */
  void begin();
  
  /**
   * Set the color of a pixel in local memory
   */
  void set(uint8_t star, uint8_t branch, uint32_t color);
  
  /**
   * Set the color of all branches of a star
   */
  void setStar(uint8_t star, uint32_t color);
  
  /**
   * Set the color of one branch for all stars
   */
  void setBranch(uint8_t branch, uint32_t color);
  
  /**
   * Get the color of a pixel in local memory
   */
  uint32_t get(uint8_t star, uint8_t branch);
  
  /**
   * Display what is in local memory
   */
  void commit();
  
  /**
   * Set all stars to black
   */
  void clear(uint32_t color = 0);
  
  /**
   * Get a color properly coded
   */
  uint32_t color(uint8_t red, uint8_t green, uint8_t blue);
  
  /**
   * Get the number of stars
   */
  uint8_t stars();
  
  /**
   * Get the number of branches per star
   */
  uint8_t branches();
};
