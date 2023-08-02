#include <FastLED.h>
#include "LightStripBase.h"
#include "Config.h"

template<template<uint8_t Data_Pin, EOrder RGB_ORDER>class LED_Type, uint8_t Num_Leds, uint8_t Data_Pin>
class LightStrip : public LightStripBase {
  public:
    CRGB leds[Num_Leds];

    LightStrip(LightStripId id) {
      this->Id = id;
    }

    /*
    * Starts initializing the led strip
    */
    void Initialize() {
      Serial.print("Initializing light strip "); 
      Serial.print(this->getName());
      Serial.println("...");

      FastLED.addLeds<LED_Type, Data_Pin>(leds, Num_Leds);
    }

    uint8_t getNumLeds() {
      return Num_Leds;
    }
};

LightStripBase* lights[] = {
  //          Chip     Num LEDs  Data Pin
  &LightStrip<WS2811,  30,       8         >(LightStripId::DashRing),
  &LightStrip<WS2811,  30,       12        >(LightStripId::ChargingStation),
  &LightStrip<WS2811,  30,       4         >(LightStripId::Door_Fr_Speaker),
  &LightStrip<WS2812B, 7,        7         >(LightStripId::Door_Fr_Pocket),
};