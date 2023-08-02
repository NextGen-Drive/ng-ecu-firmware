// CONFIG_H prevents multiple definitions of same types, etc.
// as this file is included in multiple other files
#ifndef CONFIG_H
#define CONFIG_H

#define IS_DASH_LED_ENABLED false

#define MOBILE_CHRGR_LED_DAY_BRIGHTNESS 255
#define MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS 255

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