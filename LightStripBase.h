#ifndef LIGHT_STRIP_BASE_H
#define LIGHT_STRIP_BASE_H

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

    virtual uint8_t getNumLeds() = 0;
};

#endif