#pragma once

#include "Animation.hpp"
#include "helper/AnimationHelper.hpp"
#include "configs/LedConfig.hpp"
#include "themes/ITheme.hpp"
#include "managers/LedManager.hpp"

constexpr uint16_t TRANSITION_DURATION = 1500;

class NightModeTransition : public Animation {
  public:
   void tickAnimation() override {
     if (this->isCompleted) return;

     ITheme *currentTheme = LedManager::CurrentTheme;

     unsigned long elapsedTime = millis() - animationStartTime;

     transitionStrip(elapsedTime, LedRegistry::charging_station_leds, NUM_LEDS, MOBILE_CHRGR_LED_DAY_BRIGHTNESS, MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS, currentTheme->chargingStationColor);
     transitionStrip(elapsedTime, LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, currentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, currentTheme->doorPocketColor);
     transitionStrip(elapsedTime, LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, DASH_BOTTOM_LED_DAY_BRIGHTNESS, DASH_BOTTOM_LED_NIGHT_BRIGHTNESS, currentTheme->lowerDashboardColor);

     transitionStrip(elapsedTime, LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, DOOR_SPEAKER_DAY_BRIGHTNESS, DOOR_SPEAKER_NIGHT_BRIGHTNESS, currentTheme->doorSpeakerColor);
     transitionStrip(elapsedTime, LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, DOOR_POCKET_DAY_BRIGHTNESS, DOOR_POCKET_NIGHT_BRIGHTNESS, currentTheme->doorPocketColor);

     if (millis() >= animationStartTime + TRANSITION_DURATION) {
       this->isCompleted = true;
     }
   }

   void transitionStrip(unsigned long elapsedTime, CRGB strip[], uint8_t num_leds, uint8_t day_brightness, uint8_t night_brightness, CRGB color) {
     bool isNightMode = LedManager::IS_NIGHT_MODE;
     
     uint8_t targetBrightness = isNightMode ? night_brightness : day_brightness;
     uint8_t initialBrightness = !isNightMode ? night_brightness : day_brightness;

     AnimationManager::fadeTo(elapsedTime, TRANSITION_DURATION, strip, num_leds, initialBrightness, targetBrightness, color);
   }
};