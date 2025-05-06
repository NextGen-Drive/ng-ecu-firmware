#pragma once

#include "managers/LedManager.hpp"

constexpr uint16_t STAGE2_DURATION = 1000;  // in ms

class StartupAnimation : public Animation
{
private:
  uint8_t stage = 0;
  uint8_t currentLEDPos = 0;
  unsigned long endBrightnessStageStartTime = 0;
  SharedAnimationParams dpCommon{0, 0, 0, 0, CRGB::Black};

public:
  void startAnimation() override
  {
    currentLEDPos = DASH_BOTTOM_NUM_LEDS;
    fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, CRGB::Black);
    fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, CRGB::Black);
    fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, CRGB::Black);
    fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, CRGB::Black);
    fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, CRGB::Black);
    fill_solid(charging_station_leds, NUM_LEDS, CRGB::Black);
    lastUpdateTime = millis();
  }
  
  void tickAnimation() override
  {
    unsigned long now = millis();

    ITheme *currentTheme = LedManager::CurrentTheme;
    bool isNightMode = LedManager::IS_NIGHT_MODE;

    // Dot animation from left to right
    if (stage == 0 && now - lastUpdateTime >= 7)
    {
      if (currentLEDPos != DASH_BOTTOM_NUM_LEDS)
      {
        dash_bottom_leds[currentLEDPos + 1] = CRGB::Black;
      }
      dash_bottom_leds[currentLEDPos] = adjustBrightness(CRGB::White, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
      currentLEDPos--;
      lastUpdateTime = now;

      if (currentLEDPos == -1)
      {
        stage++;
      }
      // Fade in from right to left
    }
    else if (stage == 1 && now - lastUpdateTime >= 7)
    {
      dash_bottom_leds[currentLEDPos] = adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
      currentLEDPos++;
      lastUpdateTime = now;

      if (currentLEDPos == DASH_BOTTOM_NUM_LEDS + 1)
      {
        stage++;
        endBrightnessStageStartTime = now;
        currentLEDPos = DASH_BOTTOM_NUM_LEDS;

        // Make sure everything was done correctly before as there's no animation for the dash in the next stage
        const CRGB color = adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, color);
      }
      // fade in the charge strip and others
    }
    else if (stage == 2)
    {
      unsigned long elapsedStageTime = now - endBrightnessStageStartTime;

      // Mobile Charging Station
      AnimationManager::fadeIn(elapsedStageTime, STAGE2_DURATION, charging_station_leds, NUM_LEDS, isNightMode ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS, currentTheme->chargingStationColor);

      // Update common params
      dpCommon.duration = STAGE2_DURATION;
      dpCommon.elapsedTime = elapsedStageTime;

      // Circular Door Speaker
      dpCommon.num_leds = NUM_LEDS_DOOR_FL_SPEAKER;
      dpCommon.target_brightness = isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS;
      dpCommon.target_color = currentTheme->doorPocketColorV2;
      // Front Left
      AnimationManager::fadeInSeq(door_fl_leds_speaker, dpCommon);
      // Front Right
      AnimationManager::fadeInSeq(door_fr_leds_speaker, dpCommon);

      // Door Pocket
      // Front Left -> V2
      dpCommon.num_leds = NUM_LEDS_DOOR_FL_POCKET;
      dpCommon.target_brightness = isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS;
      dpCommon.target_color = currentTheme->doorPocketColorV2;
      AnimationManager::fadeInSeq(door_fl_leds_pocket, dpCommon);
      // Front Right -> V1
      dpCommon.num_leds = NUM_LEDS_DOOR_FR_POCKET;
      dpCommon.target_color = currentTheme->doorPocketColor;
      AnimationManager::fadeInSeq(door_fr_leds_pocket, dpCommon);

      lastUpdateTime = now;

      if (elapsedStageTime >= STAGE2_DURATION)
      {
        stage++;
        isCompleted = true;
      }
    }
    else if (stage == 3)
    {
      fill_solid(charging_station_leds, NUM_LEDS, adjustBrightness(currentTheme->chargingStationColor, isNightMode ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS));
      fill_solid(door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(currentTheme->doorSpeakerColor, isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
      fill_solid(door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(currentTheme->doorPocketColor, isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
      fill_solid(dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS));

      fill_solid(door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, adjustBrightness(currentTheme->doorSpeakerColor, isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
      fill_solid(door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, adjustBrightness(currentTheme->doorPocketColorV2, isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
    }
  }
};