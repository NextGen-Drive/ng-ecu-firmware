#pragma once

#include "managers/LedManager.hpp"

constexpr uint16_t STAGE2_DURATION = 1000; // in ms

class StartupAnimation : public Animation
{
private:
  SharedAnimationParams dpCommon{0, 0, 0, 0, CRGB::Black};

  struct
  {
    uint8_t stage : 2;
    bool isDone : 1;
    int16_t currentLEDPos;
    uint16_t endBrightnessStageStartTime; // 0‑65 535 ms (wrap every 65 s)
  } state;

public:
  void startAnimation() override
  {
    state.stage = 0;
    state.isDone = false;
    state.currentLEDPos = DASH_BOTTOM_NUM_LEDS;
    state.endBrightnessStageStartTime = 0;
    fill_solid(LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, CRGB::Black);
    fill_solid(LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, CRGB::Black);
    fill_solid(LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, CRGB::Black);
    fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, CRGB::Black);
    fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, CRGB::Black);
    fill_solid(LedRegistry::charging_station_leds, NUM_LEDS, CRGB::Black);
    lastUpdateTime = millis();
  }

  void tickAnimation() override
  {
    unsigned long now = millis();

    ITheme *currentTheme = LedManager::CurrentTheme;
    bool isNightMode = LedManager::IS_NIGHT_MODE;

    // Dot animation from left to right
    if (state.stage == 0 && now - lastUpdateTime >= 7)
    {
      if (state.currentLEDPos >= 0 && state.currentLEDPos < DASH_BOTTOM_NUM_LEDS - 1)
      {
        LedRegistry::dash_bottom_leds[state.currentLEDPos + 1] = CRGB::Black;
      }
      LedRegistry::dash_bottom_leds[state.currentLEDPos] = adjustBrightness(CRGB::White, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
      state.currentLEDPos--;
      lastUpdateTime = now;

      if (state.currentLEDPos == -1)
      {
        state.currentLEDPos = 0;
        state.stage++;
      }
    }
    else if (state.stage == 1 && now - lastUpdateTime >= 7)
    {
      LedRegistry::dash_bottom_leds[state.currentLEDPos] = adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
      state.currentLEDPos++;
      lastUpdateTime = now;

      if (state.currentLEDPos == DASH_BOTTOM_NUM_LEDS + 1)
      {
        state.stage++;
        state.endBrightnessStageStartTime = now;
        state.currentLEDPos = DASH_BOTTOM_NUM_LEDS;

        const CRGB color = adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS);
        fill_solid(LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, color);
      }
    }
    else if (state.stage == 2)
    {
      unsigned long elapsedStageTime = now - state.endBrightnessStageStartTime;

      // Mobile Charging Station
      AnimationManager::fadeIn(elapsedStageTime, STAGE2_DURATION, LedRegistry::charging_station_leds, NUM_LEDS, isNightMode ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS, currentTheme->chargingStationColor);

      // Update common params
      dpCommon.duration = STAGE2_DURATION;
      dpCommon.elapsedTime = elapsedStageTime;

      // Circular Door Speaker
      dpCommon.num_leds = NUM_LEDS_DOOR_FL_SPEAKER;
      dpCommon.target_brightness = isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS;
      dpCommon.target_color = currentTheme->doorPocketColorV2;
      // Front Left
      AnimationManager::fadeInSeq(LedRegistry::door_fl_leds_speaker, dpCommon);
      // Front Right
      AnimationManager::fadeInSeq(LedRegistry::door_fr_leds_speaker, dpCommon);

      // Door Pocket
      // Front Left -> V2
      dpCommon.num_leds = NUM_LEDS_DOOR_FL_POCKET;
      dpCommon.target_brightness = isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS;
      dpCommon.target_color = currentTheme->doorPocketColorV2;
      // Front Right -> V1
      AnimationManager::fadeInSeq(LedRegistry::door_fl_leds_pocket, dpCommon);

      dpCommon.num_leds = NUM_LEDS_DOOR_FR_POCKET;
      dpCommon.target_color = currentTheme->doorPocketColor;
      AnimationManager::fadeInSeq(LedRegistry::door_fr_leds_pocket, dpCommon);

      lastUpdateTime = now;

      if (elapsedStageTime >= STAGE2_DURATION)
      {
        state.stage++;
        isCompleted = true;
      }
    }
    else if (state.stage == 3)
    {
      fill_solid(LedRegistry::charging_station_leds, NUM_LEDS, adjustBrightness(currentTheme->chargingStationColor, isNightMode ? MOBILE_CHRGR_LED_NIGHT_BRIGHTNESS : MOBILE_CHRGR_LED_DAY_BRIGHTNESS));
      fill_solid(LedRegistry::door_fr_leds_speaker, NUM_LEDS_DOOR_FR_SPEAKER, adjustBrightness(currentTheme->doorSpeakerColor, isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
      fill_solid(LedRegistry::door_fr_leds_pocket, NUM_LEDS_DOOR_FR_POCKET, adjustBrightness(currentTheme->doorPocketColor, isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
      fill_solid(LedRegistry::dash_bottom_leds, DASH_BOTTOM_NUM_LEDS, adjustBrightness(currentTheme->lowerDashboardColor, isNightMode ? DASH_BOTTOM_LED_NIGHT_BRIGHTNESS : DASH_BOTTOM_LED_DAY_BRIGHTNESS));

      fill_solid(LedRegistry::door_fl_leds_speaker, NUM_LEDS_DOOR_FL_SPEAKER, adjustBrightness(currentTheme->doorSpeakerColor, isNightMode ? DOOR_SPEAKER_NIGHT_BRIGHTNESS : DOOR_SPEAKER_DAY_BRIGHTNESS));
      fill_solid(LedRegistry::door_fl_leds_pocket, NUM_LEDS_DOOR_FL_POCKET, adjustBrightness(currentTheme->doorPocketColorV2, isNightMode ? DOOR_POCKET_NIGHT_BRIGHTNESS : DOOR_POCKET_DAY_BRIGHTNESS));
    }
  }
};
