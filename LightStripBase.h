#include <FastLED.h>
#include "Config.h"

class LightStripBase {
  public:
    LightStripId Id;

    CRGB leds[0];

    /*
    * Starts initializing the led strip
    */
    virtual void Initialize() = 0;

    const char* getName() {
      return lightStripNames[Id];
    }
};