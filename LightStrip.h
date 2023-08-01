#include <FastLED.h>
#include "LightStripBase.h"

#define NUM_LEDS 30
#define DATA_PIN 8
#define DATA_PIN_CHARGING_STATION_LIGHT 12
#define NUM_LED_MIDDLE 16

#define DATA_PIN_DOOR_FR 4
#define NUM_LEDS_DOOR_FR 7

#define IS_DASH_LED_ENABLED false

#define MOBILE_CHRGR_LED_DAY_BRIGHTNESS 255
#define MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS 255

#define DATA_PIN_DOOR_FR_SPEAKER 4
#define NUM_LEDS_DOOR_FR_SPEAKER 30

#define DATA_PIN_DOOR_FR_POCKET 7
#define NUM_LEDS_DOOR_FR_POCKET 7

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
};

LightStripBase* lights[] = {
  //          Chip    Num LEDs  Data Pin
  &LightStrip<WS2811, NUM_LEDS, DATA_PIN>(LightStripId::DashRing),
  &LightStrip<WS2811, NUM_LEDS, DATA_PIN_CHARGING_STATION_LIGHT>(LightStripId::ChargingStation),
  &LightStrip<WS2811, NUM_LEDS_DOOR_FR_SPEAKER, DATA_PIN_DOOR_FR_SPEAKER>(LightStripId::Door_Fr_Speaker),
  &LightStrip<WS2811, NUM_LEDS_DOOR_FR_POCKET, DATA_PIN_DOOR_FR_POCKET>(LightStripId::Door_Fr_Pocket),
};