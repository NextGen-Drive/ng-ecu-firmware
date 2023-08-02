#include "LightStripBase.h"

CRGB adjustBrightness(CRGB color, uint8_t brightness) {
  // Scale the brightness of the color using the nscale8_video() function
  color.nscale8_video(brightness);
  return color;
}

LightStripBase* findLightStrip(LightStripId id) {
  size_t arrLength = sizeof(lights) / sizeof(lights[0]);
  LightStripBase* foundElement = nullptr;

  for (size_t i = 0; i < arrLength; i++) {
    if (lights[i]->Id == id) {
      foundElement = lights[i];
      break; // Element found, exit the loop
    }
  }

  return foundElement;
}