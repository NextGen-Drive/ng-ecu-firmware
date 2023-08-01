#include <FastLED.h>

enum LightStripId {
  DashRing,
  ChargingStation,
  Door_Fr_Speaker,
  Door_Fr_Pocket,
};

const char* lightStripNames[] = {
  "dash_ring",
  "charging_station",
  "door_fr_speaker",
  "door_fr_pocket"
};

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