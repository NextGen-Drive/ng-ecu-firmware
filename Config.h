// CONFIG_H prevents multiple definitions of same types, etc.
// as this file is included in multiple other files
#ifndef CONFIG_H
#define CONFIG_H

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

#endif